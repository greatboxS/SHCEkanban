
#ifndef B_TYPEDEF_H
#define B_TYPEDEF_H

#include <ArduinoJson.h>
#include <vector>
#include"JsonGetMember.h"
#include "BNextion.h"
#include "LocalClient.h"

#define NEXTION_BAUD 115200
#define NEXTION_TIMEOUT 200

BNextion Bnextion(NEXTION_BAUD, NEXTION_TIMEOUT);
LocalClient EClient;

typedef struct {
	char code[32];		//code
	int dataLength;
	char buf[1024 * 5];
}HttpHeader_t;

typedef struct {
	uint8_t arr[6];
	//uint8_t day;	0
	//uint8_t month;1
	//uint8_t year; 2

	//uint8_t hour; 3
	//uint8_t min;  4
	//uint8_t sec;  5

	volatile uint8_t localSec;
	volatile uint8_t localMin;
	volatile uint8_t localHour;
}DateTime_t;



//-------------------------------------------------------------
// PO COmponent List
//-------------------------------------------------------------
typedef struct {
	char Name[64];
}PartInfo_t;

//-------------------------------------------------------------
// Size Information
//-------------------------------------------------------------
typedef struct {
	uint8_t SizeId;
	uint8_t SizeQty;
	void clear()
	{
		SizeId = 0;
		SizeQty = 0;
	}
}SizeInfo_t;

//-------------------------------------------------------------
// PO information 
//-------------------------------------------------------------

typedef struct Interface_t {
	long id;
	char PoNumber[50];
	char Line[5];
	char Model[10];
	char ModelName[50];
	char Article[10];
	char SequenceNo[50];
	char CartQty[50];
	char PoQty[50];
};
struct EKanban_t {

	bool confirm = false;
	bool ScreenOff;
	uint16_t TotalCartQty;
	uint16_t ID;
	Interface_t Interface;
	std::vector<PartInfo_t> PartList;
	std::vector<SizeInfo_t> SizeList;

	char confirmMessage[32];

public:
	void clear()
	{
		Interface.id = 0;
		memset(Interface.PoNumber, 0, sizeof(Interface.PoNumber));
		memset(Interface.Line, 0, sizeof(Interface.Line));
		memset(Interface.Model, 0, sizeof(Interface.Model));
		memset(Interface.ModelName, 0, sizeof(Interface.ModelName));
		memset(Interface.SequenceNo, 0, sizeof(Interface.SequenceNo));
		memset(Interface.CartQty, 0, sizeof(Interface.CartQty));
		memset(Interface.PoQty, 0, sizeof(Interface.PoQty));
		memset(Interface.Article, 0, sizeof(Interface.Article));
		confirm = false;
		ScreenOff = false;
		ID = 0;
		PartList.clear();
		SizeList.clear();
	}

	void GetData()
	{
		char url[64]{ 0 };
		snprintf(url, sizeof(url), "api-getdata/%d", ID);
		Serial.println(url);
		String s(url);
		EClient.begin_Request(s);
	}

	void Confirm()
	{
		char url[64]{ 0 };
		uint32_t confirmQty = Bnextion.getPage_numberProperty(SizeP, "n1");
		snprintf(url, sizeof(url), "api-confirm/%d/%d", ID, confirmQty);
		String s(url);
		EClient.begin_Request(s);
	}

	void DeserializeJsonObject(JsonDocument& jsondoc)
	{
		Serial.println("DeserializeJsonObject");
		int start = millis();
		JsonParse_Element(jsondoc, "ScreenOff", ScreenOff);
		JsonParse_Element(jsondoc, "TotalCartQty", TotalCartQty);
		Serial.printf("ScreenOff: %s\r\n", ScreenOff ? "true" : "false");
		JsonObject JsonObj = jsondoc.getMember("Interface");
		GetPoInfoObject(JsonObj);
		JsonArray JsonArray = jsondoc.getMember("PartList");
		GetComponentObject(JsonArray);
		JsonArray = jsondoc.getMember("SizeList");
		GetSizeObject(JsonArray);
		int stop = millis();

		if (ScreenOff)
		{
			digitalWrite(SCREEN_LIGHT_CONTROL_PIN, LOW);
		}
		else
			digitalWrite(SCREEN_LIGHT_CONTROL_PIN, HIGH);

		Serial.printf("Deserialize donde, Excuted time: %d\r\n", stop - start);
	}

	void ClearScreen()
	{
		Bnextion.setPage_stringProperty(Comp1P, "t19", "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_PO), "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_PO), "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_PO_QTY), "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_SEQUENCE), "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_SEQUENCE_QTY), "");
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_LINE), "");

		int temp = 0;
		for (size_t i = 0; i < 30; i++)
		{
			if (i < 15)
				Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_BEGIN_COMP + i), "");
			else
			{
				Bnextion.setPage_stringProperty(Comp2P, Bnextion.NexText(COMP2_TXT_BEGIN_COMP + temp), "");
				temp++;
			}

		}

		for (size_t i = 5; i <= 34; i++)
		{
			Bnextion.setPage_stringProperty(SizeP, Bnextion.NexText(i), "");
			Bnextion.setPage_PropertyBackgroundColor(SizeP, Bnextion.NexText(i), NEX_COL_WHITE);
		}
	}

	void UpdateConfirmMessage(JsonDocument& jsondoc)
	{
		Serial.println("UpdateConfirmMessage");
		JsonParse_Element(jsondoc, "EMessage", confirmMessage, sizeof(confirmMessage));
	}

	void NextionUpdateConfirm()
	{
		Bnextion.setPage_stringProperty(Comp1P, "t19", confirmMessage);
		Bnextion.GotoPage(5);
		Bnextion.setPage_stringProperty(5, "t0", confirmMessage);
	}

	void UpdateNextionScreen()
	{
		Serial.println("Nextion_Update");
		Bnextion.setPage_numberProperty(Comp1P, Bnextion.NexText(COMP1_NUM_CART), ID);
		Bnextion.setPage_stringAsNumberProperty(Comp1P, "t20", TotalCartQty);
		Bnextion.setPage_numberProperty(Comp1P, "n0", ID);
		NextionSendInterface();
		NextionSetComponent();
		NextionSetSize();
	}

	void NextionSendInterface()
	{
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_PO), Interface.PoNumber);
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_PO_QTY), Interface.PoQty);
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_SEQUENCE), Interface.SequenceNo);
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_SEQUENCE_QTY), Interface.CartQty);
		Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_LINE), Interface.Line);
	}

	void NextionSetComponent()
	{
		int temp = 0;
		for (size_t i = 0; i < PartList.size(); i++)
		{
			if (i < 14)
				Bnextion.setPage_stringProperty(Comp1P, Bnextion.NexText(COMP1_TXT_BEGIN_COMP + i), PartList.at(i).Name);
			else
			{
				Bnextion.setPage_stringProperty(Comp2P, Bnextion.NexText(COMP2_TXT_BEGIN_COMP + temp), PartList.at(i).Name);
				temp++;
			}
		}
	}

	void NextionSetSize()
	{
		for (size_t i = 0; i < SizeList.size(); i++)
		{
			Bnextion.setPage_stringAsNumberProperty(SizeP, Bnextion.NexText(SizeList.at(i).SizeId + 2), SizeList.at(i).SizeQty);
			Bnextion.setPage_PropertyBackgroundColor(SizeP, Bnextion.NexText(SizeList.at(i).SizeId + 2), NEX_COL_YELLOW);
		}
	}

private:
	void GetComponentObject(JsonArray& JsocnArray)
	{
		Serial.println("GetComponentObject");
		PartList.clear();
		for (size_t i = 0; i < JsocnArray.size(); i++)
		{
			PartInfo_t Part;
			JsonParse_Element(JsocnArray, i, "Name", Part.Name, sizeof(Part.Name));
			Serial.printf("Part [%d], Name: %s\r\n", i, Part.Name);
			PartList.emplace_back(Part);
		}
	}

	void GetSizeObject(JsonArray& JsocnArray)
	{
		Serial.println("GetSizeObject");
		SizeList.clear();
		for (size_t i = 0; i < JsocnArray.size(); i++)
		{
			SizeInfo_t Size;
			JsonParse_Element(JsocnArray, i, "SizeId", Size.SizeId);
			JsonParse_Element(JsocnArray, i, "SizeQty", Size.SizeQty);
			Serial.printf("Size [%d], sizeId: %d, sizeQty: %d\r\n", i, Size.SizeId, Size.SizeQty);
			SizeList.emplace_back(Size);
		}
	}

	void GetPoInfoObject(JsonDocument& jsondoc)
	{
		Serial.println("GetPoInfoObject");
		JsonParse_Element(jsondoc, "id", Interface.id);
		JsonParse_Element(jsondoc, "PoNumber", Interface.PoNumber, sizeof(Interface.PoNumber));
		JsonParse_Element(jsondoc, "Line", Interface.Line, sizeof(Interface.Line));
		JsonParse_Element(jsondoc, "Model", Interface.Model, sizeof(Interface.Model));
		JsonParse_Element(jsondoc, "ModelName", Interface.ModelName, sizeof(Interface.ModelName));
		JsonParse_Element(jsondoc, "Article", Interface.Article, sizeof(Interface.Article));
		JsonParse_Element(jsondoc, "CartQty", Interface.CartQty, sizeof(Interface.CartQty));
		JsonParse_Element(jsondoc, "SequenceNo", Interface.SequenceNo, sizeof(Interface.SequenceNo));
		JsonParse_Element(jsondoc, "PoQty", Interface.PoQty, sizeof(Interface.PoQty));

	}

	void GetPoInfoObject(JsonObject& jsondoc)
	{
		Serial.println("GetPoInfoObject");
		JsonParse_Element(jsondoc, "id", Interface.id);
		JsonParse_Element(jsondoc, "PoNumber", Interface.PoNumber, sizeof(Interface.PoNumber));
		JsonParse_Element(jsondoc, "Line", Interface.Line, sizeof(Interface.Line));
		JsonParse_Element(jsondoc, "Model", Interface.Model, sizeof(Interface.Model));
		JsonParse_Element(jsondoc, "ModelName", Interface.ModelName, sizeof(Interface.ModelName));
		JsonParse_Element(jsondoc, "Article", Interface.Article, sizeof(Interface.Article));
		JsonParse_Element(jsondoc, "CartQty", Interface.CartQty, sizeof(Interface.CartQty));
		JsonParse_Element(jsondoc, "SequenceNo", Interface.SequenceNo, sizeof(Interface.SequenceNo));
		JsonParse_Element(jsondoc, "PoQty", Interface.PoQty, sizeof(Interface.PoQty));

		Serial.printf("id: %d\r\n", Interface.id);
		Serial.printf("PoNumber: %s\r\n", Interface.PoNumber);
		Serial.printf("Line: %s\r\n", Interface.Line);
		Serial.printf("PoQty: %s\r\n", Interface.PoQty);
		Serial.printf("SequenceNo: %s\r\n", Interface.SequenceNo);
		Serial.printf("CartQty: %s\r\n", Interface.CartQty);

	}
};
//


typedef struct {
	bool globalRequest;
	bool getHis;
	bool RequestTimeOut;

}Flag_t;

// Error 
typedef struct {
	String Request;
	char Message[100];
}Error_t;

enum Request_n
{
	IDLE,
	GET_USER_INFO,
	GET_SCHEDULE_INFO,
	GET_PO_INFO,
	GET_SEQ_INFO,
	GET_PART_INFO,
	POST_START_CUTTING,
	POST_SIZE,
	POST_STOP_CUTTING,
	GET_HiSTORIES,
	GET_TIME,

};
enum ShoeSize_n
{
	Unknow = 1,
	_T,
	_1,
	_1T,
	_2,
	_2T,
	_3,
	_3T,
	_4,
	_4T,
	_5,
	_5T,
	_6,
	_6T,
	_7,
	_7T,
	_8,
	_8T,
	_9,
	_9T,
	_10,
	_10T,
	_11,
	_11T,
	_12,
	_12T,
	_13,
	_13T,
	_14,
	_14T,
	_15,
	_15T
};

enum RespType_t {
	resp_unknow,
	detected_resp,
	seq_list_resp,
	seq_size_resp,
	confirm_resp,
	confirm_ok,

	RESP_SEARCHING_PO,
	RESP_GET_COMPONENT,
	RESP_GET_SEQUENCE,
	RESP_GET_HISTORIES,
	RESP_GET_SIZES,
	RESP_GET_USER_INFO,
	RESP_GET_TIME,

	RESP_POST_SIZE,
	RESP_POST_START_CUT,
	RESP_POST_STOP_CUT,
	REQUEST_ERROR

};

typedef struct WIFI_Profile_t {

	WIFI_Profile_t() {

	}

	WIFI_Profile_t(char* ssid, char* password)
	{
		if (ssid != nullptr)
			memccpy(SSID, ssid, 0, sizeof(SSID));
		if (password != nullptr)
			memccpy(Password, password, 0, sizeof(Password));
	}

	//WIFI_Profile_t operator = (WIFI_Profile_t& other)
	//{
	//	memccpy(SSID, other.SSID, 0, sizeof(SSID));
	//	memccpy(Password, other.Password, 0, sizeof(Password));
	//	return *this;
	//}

	bool Found = false;
	char SSID[32];
	char Password[32];
	int32_t RSSI = 0;
	uint8_t* BSSID = 0;
	int32_t Channel = 0;
	uint8_t EncryptionType = 0;
	String ssid = "";

	void ClearStatus()
	{
		Found = false;
		RSSI = 0;
		BSSID = NULL;
		Channel = 0;
		EncryptionType = 0;
	}

	void Renew()
	{
		ClearStatus();
		memset(SSID, 0, sizeof(SSID));
		memset(SSID, 0, sizeof(Password));
	}

	bool operator == (char* ssid)
	{
		if (strncmp(SSID, ssid, sizeof(SSID)) == 0)
		{
			Found = true;
			return true;
		}
		else return false;
	}

	bool operator == (String& ssid)
	{
		String s(SSID);
		//memccpy(temp, ssid.c_str(), 0, sizeof(temp));
		if (s == ssid)
		{
			return true;
		}
		else return false;
	}

	bool operator == (WIFI_Profile_t& other)
	{
		if (strcmp(SSID, other.SSID) == 0)
			if (strcmp(Password, other.Password) == 0)
				return true;

		return false;
	}

	void Print()
	{
		Serial.print("SSID:");
		Serial.println(SSID);
		Serial.print("Password:");
		Serial.println(Password);
	}

	void CheckingWIFI(WIFI_Profile_t& wifiProfile)
	{
		if (*this == wifiProfile)
		{
			Found = true;
		}
	}

	void UpdateProfile(WIFI_Profile_t& wifiProfile)
	{
		RSSI = wifiProfile.RSSI;
		BSSID = wifiProfile.BSSID;
		Channel = wifiProfile.Channel;
		EncryptionType = wifiProfile.EncryptionType;
	}

	void UpdateSSID(char* ssid)
	{
		memccpy(SSID, ssid, 0, sizeof(SSID));
	}

	void UpdatePassword(char* password)
	{
		memccpy(Password, password, 0, sizeof(Password));
	}

	void UpdateProfile(char* ssid, char* password)
	{
		UpdateSSID(ssid);
		UpdatePassword(password);
	}
};

#define List(i) List.at(i)

typedef struct WIFI_Source_t {

	const  uint8_t TotalAccount = 5;
	std::vector<WIFI_Profile_t> List;
	uint8_t CurrentIndex;
	const uint8_t MaximumSize = 32;
	const uint8_t Maximum = 5;

	WIFI_Source_t()
	{
		WiFi_Defalut.UpdateProfile("SHC-INT-H", "20182019");
		SetupWiFi.UpdateProfile("TechTeam", "20182019");
	}

	WIFI_Profile_t WiFi_Defalut;
	WIFI_Profile_t SetupWiFi;

	size_t AddWiFi(char* ssid, char* password)
	{
		WIFI_Profile_t temp(ssid, password);

		if (List.size() < Maximum)
		{
			List.emplace_back(temp);
		}
		else
		{
			List.insert(List.begin(), 1, temp);
		}
		return List.size();
	}

	size_t AddWiFi(WIFI_Profile_t& wifi_profile)
	{
		if (List.size() < Maximum)
		{
			List.emplace_back(wifi_profile);
		}
		else
		{
			List.insert(List.begin(), 1, wifi_profile);
		}

		return List.size();
	}

	void BeginNewScaning()
	{
		for (size_t i = 0; i < List.size(); i++)
		{
			List.at(i).ClearStatus();
		}
	}

	//void WiFi_Checking(WIFI_Profile_t& wifiProfile)
	//{
	//	for (size_t i = 0; i < List.size(); i++)
	//	{
	//		List.at(i).CheckingWIFI(wifiProfile);
	//	}
	//}

	void WiFi_Checking(char* ssid)
	{
		for (size_t i = 0; i < List.size(); i++)
		{
			List.at(i) == ssid;
		}
	}

	void WiFi_Checking(WIFI_Profile_t& wifiProfile)
	{
		Serial.println(List.size());
		for (size_t i = 0; i < List.size(); i++)
		{
			Serial.printf("Checking WiFi: %s\r\n", wifiProfile.ssid.c_str());
			if (List.at(i) == wifiProfile.ssid)
			{
				List.at(i).UpdateProfile(wifiProfile);
				List.at(i).Found = true;
				Serial.printf("Found WiFi: %s\r\n", wifiProfile.ssid.c_str());
			}
		}
	}

	WIFI_Profile_t& GetBestWiFiChannel()
	{
		Serial.println("GetBestWiFiChannel");

		int32_t power = -100;
		uint8_t index = 0;
		bool found = false;
		Serial.println(List.size());
		for (size_t i = 0; i < List.size(); i++)
		{
			Serial.printf("Check index[%d]\r\n", i);
			if (List(i).Found)
			{
				found = true;
				Serial.printf("signal[%d]\r\n", List(i).RSSI);

				if (power < List(i).RSSI)
				{
					Serial.printf("Found [%d], ssid:%s\r\n", i, List(i).SSID);
					power = List(i).RSSI;
					index = i;
				}
			}
		}

		if (!found)
			return WiFi_Defalut;
		else
			return List(index);
	}

	WIFI_Profile_t& GetWiFiChannel(uint8_t index)
	{
		if (List.size() < index)
			return WiFi_Defalut;

		return List.at(index);
	}

	void UpdateNextionWiFi()
	{
		for (size_t i = 0; i < List.size(); i++)
		{
			Bnextion.setPage_stringProperty(SettingP, Bnextion.NexText(i * 2 + SET_TXT_BEGIN_WIFI_ID),
				List(i).SSID);
			Bnextion.setPage_stringProperty(SettingP, Bnextion.NexText(i * 2 + SET_TXT_BEGIN_WIFI_ID + 1),
				List(i).Password);
		}
	}

};

#endif // !B_TYPEDEF_H

