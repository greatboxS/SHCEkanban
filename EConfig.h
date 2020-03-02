// EConfig.h

#ifndef _ECONFIG_h
#define _ECONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "BTypedef.h"
#include "LocalBackup.h"
#include "JsonGetMember.h"
#include "OTAConfig.h"


#define OTA_FUNCTION_SELECTED T3
#define TOUCH_THRESHOLD 40

volatile bool wifi_connected;
volatile int WINDOWS_STATE = 0;

void Interrupt_OTA_Touch()
{
	WINDOWS_STATE += 1; // update flash
	Serial.print(".");
}
void Touch_init() {
	touchAttachInterrupt(OTA_FUNCTION_SELECTED, Interrupt_OTA_Touch, TOUCH_THRESHOLD);
}
//
bool WIFI_Scan();
void WIFI_Connect2();
void UpdateServerIP(char* server, uint16_t port);

void NextionUpdateWIFI();
void NextionUpdateCartID();

void Setting_SaveServerInformation(char* server, uint8_t len);
void Setting_SaveServerInformation(uint16_t port);
void Setting_ReadServerInformation();
void Setting_SaveServerInformation(char* server, uint8_t len, uint16_t port);
void Setting_SaveEkanbanId(int id);
void Setting_AddWiFiAccount(char* ssid, char* password);
void Setting_ReadWiFiSource();
void Local_ReadData(WiFiClient& _client);
void Json_DeserializeReceivedData();

void Nextion_PageLoadingEvent(uint8_t _pageId, uint8_t _componentId, uint8_t _eventType);
void BUT_NEXT1_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_NEXT2_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_CONFIRM_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_MBX_YES_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_MBX_NO_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_SAVE_IP_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_SAVE_ID_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_ADD_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_RESET_CALLBACK(NextionEventType type, INextionTouchable* widget);
void BUT_MAIN_CALLBACK(NextionEventType type, INextionTouchable* widget);


void WiFiEvent(WiFiEvent_t event);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

HttpHeader_t httpHeader;
WIFI_Source_t WIFI_Source;
EKanban_t EKanban;
WIFI_Profile_t CurrentWIFI;
DateTime_t DATETIME;
char ssid[32];
char password[32];


void IOConfig()
{
	pinMode(SCREEN_LIGHT_CONTROL_PIN, OUTPUT);
	digitalWrite(SCREEN_LIGHT_CONTROL_PIN, HIGH);
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, LOW);
}
// Start up function
void System_startup()
{
	IOConfig();
	delay(1000);
	Bnextion.init();

	Bnextion.PAGE_LOADING.root_attachCallback(Nextion_PageLoadingEvent);
	Bnextion.BUT_NEXT1.attachCallback(BUT_NEXT1_CALLBACK);
	Bnextion.BUT_NEXT2.attachCallback(BUT_NEXT2_CALLBACK);
	Bnextion.BUT_SAVE_ID.attachCallback(BUT_SAVE_ID_CALLBACK);
	Bnextion.BUT_SAVE_IP.attachCallback(BUT_SAVE_IP_CALLBACK);
	Bnextion.BUT_MAIN.attachCallback(BUT_MAIN_CALLBACK);
	Bnextion.BUT_RESET.attachCallback(BUT_RESET_CALLBACK);
	Bnextion.BUT_ADD.attachCallback(BUT_ADD_CALLBACK);
	Bnextion.BUT_MBX_NO.attachCallback(BUT_MBX_NO_CALLBACK);
	Bnextion.BUT_MBX_YES.attachCallback(BUT_MBX_YES_CALLBACK);
	Bnextion.BUT_CONFIRM.attachCallback(BUT_CONFIRM_CALLBACK);

	EClient.init(Local_ReadData);

	Setting_ReadServerInformation();

	Touch_init();

	Setting_ReadWiFiSource();

	NextionUpdateCartID();

	//WiFi.onEvent(WiFiEvent);
	//WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
	//WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
	//	Serial.print("WiFi lost connection. Reason: ");
	//	Serial.println(info.disconnected.reason);
	//	}, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
}


void Local_ReadData(WiFiClient& _client)
{
	int receivedBytes = _client.available();
	memset(httpHeader.buf, 0, sizeof(httpHeader.buf));
	_client.readBytesUntil('\r\n', httpHeader.buf, receivedBytes);

	if (strstr(httpHeader.buf, "HTTP") != nullptr)
	{
		memccpy(httpHeader.code, httpHeader.buf, 0, sizeof(httpHeader.code));
		Serial.println(httpHeader.code);
	}

	if (strstr(httpHeader.buf, "{") != nullptr)
	{
		// Get the responsed data from server
		Json_DeserializeReceivedData();
	}
}

void Json_DeserializeReceivedData()
{
	DynamicJsonDocument JsonDoc = DynamicJsonDocument(1024 * 5);
	String eop = "";
	DeserializationError error = deserializeJson(JsonDoc, httpHeader.buf);
	eop = JsonDoc.getMember("eop").as<const char*>();
	Serial.printf("Error code: %s\r\n", error.c_str());

	if (eop == "EKANBAN_GET_INTERFACE")
	{
		uint32_t oldId = EKanban.Interface.id;

		EKanban.DeserializeJsonObject(JsonDoc);

		if (oldId != EKanban.Interface.id)
			EKanban.ClearScreen();

		EKanban.UpdateNextionScreen();
	}

	if (eop == "EKANBAN_CONFIRM_ITEM")
	{
		EKanban.UpdateConfirmMessage(JsonDoc);
		EKanban.NextionUpdateConfirm();
	}

	JsonDoc.clear();
}

void NextionUpdateCartID()
{
	EEPROM.begin(1024);
	EKanban.ID = lcal::read<uint16_t>(0);
	EEPROM.end();
	Bnextion.setPage_numberProperty(Comp1P, "n0", EKanban.ID);
}

void NextionUpdateWIFI()
{
	Bnextion.setPage_numberProperty(Comp1P, "WIFI", wifi_connected);
}

void BUT_NEXT1_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_NEXT1_CALLBACK");
	if (EKanban.PartList.size() > 15)
	{
		Bnextion.GotoPage(Comp2P);
		EKanban.NextionSetComponent();
	}

	else
		Bnextion.GotoPage(SizeP);
}

void Nextion_PageLoadingEvent(uint8_t _pageId, uint8_t _componentId, uint8_t _eventType)
{
	Serial.println("Nextion_PageLoadingEvent");
	Serial.print("PageId: ");
	Serial.println(_pageId);
	Serial.print("ComponentId: ");
	Serial.println(_componentId);

	if (_componentId != 0xF0)
		return;

	switch (_pageId)
	{
	case SettingP:
		Setting_ReadWiFiSource();
		break;

	default:
		break;
	}
}

void BUT_NEXT2_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_NEXT2_CALLBACK");
	Bnextion.GotoPage(SizeP);
}

void BUT_CONFIRM_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_CONFIRM_CALLBACK");
	EKanban.Confirm();
}

void BUT_MBX_YES_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_MBX_YES_CALLBACK");
}

void BUT_MBX_NO_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_MBX_NO_CALLBACK");
}

void BUT_SAVE_IP_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_SAVE_IP_CALLBACK");
	char server[20];
	uint16_t port;
	Bnextion.getPage_stringProperty(SettingP, "SERVER_IP", server, sizeof(server));
	port = Bnextion.getPage_numberProperty(SettingP, "PORT");
	Setting_SaveServerInformation(server, sizeof(server), port);
	EClient.SetupHost(server, port);
	Serial.printf("New serverIp:%s, port:%d\r\n", server, port);
	UpdateServerIP(server, port);
}

void BUT_SAVE_ID_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_SAVE_ID_CALLBACK");
	EKanban.ID = (uint16_t)Bnextion.getPage_numberProperty(SettingP, "n1");
	Serial.printf("New ID:%d\r\n", EKanban.ID);
	EEPROM.begin(1024);
	lcal::write(EKanban.ID, CART_NUMBER_ADDRESS);
	EEPROM.end();
	Bnextion.setPage_numberProperty(Comp1P, Bnextion.NexNumber(0), EKanban.ID);
}

void BUT_ADD_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_ADD_CALLBACK");
	char ssid[32]{ 0 };
	char password[32]{ 0 };
	Bnextion.getPage_stringProperty(SettingP, Bnextion.NexText(SET_TXT_USER_ID), ssid, sizeof(ssid));
	Bnextion.getPage_stringProperty(SettingP, Bnextion.NexText(SET_TXT_PASS_ID), password, sizeof(password));

	Serial.printf("New ssid:%s, password:%s\r\n", ssid, password);
	Setting_AddWiFiAccount(ssid, password);
	Setting_ReadWiFiSource();
	Bnextion.setPage_stringProperty(SettingP, Bnextion.NexText(SET_TXT_USER_ID), "");
	Bnextion.setPage_stringProperty(SettingP, Bnextion.NexText(SET_TXT_PASS_ID), "");

}

void BUT_RESET_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_RESET_CALLBACK");
	ESP.restart();
}

void BUT_MAIN_CALLBACK(NextionEventType type, INextionTouchable* widget)
{
	Serial.println("BUT_MAIN_CALLBACK");
	Bnextion.GotoPage(Comp1P);
}

void UpdateServerIP(char* server, uint16_t port)
{
	Bnextion.setPage_stringProperty(SettingP, "SERVER_IP", server);
	Bnextion.setPage_numberProperty(SettingP, "PORT", port);
}


void WiFiEvent(WiFiEvent_t event)
{
	Serial.printf("[WiFi-event] event: %d\n", event);

	switch (event) {
	case SYSTEM_EVENT_WIFI_READY:
		Serial.println("WiFi interface ready");
		break;
	case SYSTEM_EVENT_SCAN_DONE:
		Serial.println("Completed scan for access points");
		break;
	case SYSTEM_EVENT_STA_START:
		Serial.println("WiFi client started");
		break;
	case SYSTEM_EVENT_STA_STOP:
		Serial.println("WiFi clients stopped");
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		Serial.println("Connected to access point");
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		Serial.println("Disconnected from WiFi access point");
		break;
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
		Serial.println("Authentication mode of access point has changed");
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		Serial.print("Obtained IP address: ");
		Serial.println(WiFi.localIP());
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
		Serial.println("Lost IP address and IP address is reset to 0");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
		Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
		Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
		Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_PIN:
		Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
		break;
	case SYSTEM_EVENT_AP_START:
		Serial.println("WiFi access point started");
		break;
	case SYSTEM_EVENT_AP_STOP:
		Serial.println("WiFi access point  stopped");
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		Serial.println("Client connected");
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		Serial.println("Client disconnected");
		break;
	case SYSTEM_EVENT_AP_STAIPASSIGNED:
		Serial.println("Assigned IP address to client");
		break;
	case SYSTEM_EVENT_AP_PROBEREQRECVED:
		Serial.println("Received probe request");
		break;
	case SYSTEM_EVENT_GOT_IP6:
		Serial.println("IPv6 is preferred");
		break;
	case SYSTEM_EVENT_ETH_START:
		Serial.println("Ethernet started");
		break;
	case SYSTEM_EVENT_ETH_STOP:
		Serial.println("Ethernet stopped");
		break;
	case SYSTEM_EVENT_ETH_CONNECTED:
		Serial.println("Ethernet connected");
		break;
	case SYSTEM_EVENT_ETH_DISCONNECTED:
		Serial.println("Ethernet disconnected");
		break;
	case SYSTEM_EVENT_ETH_GOT_IP:
		Serial.println("Obtained IP address");
		break;
	default: break;
	}
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}


//---------------------------------------------------------------------------
bool WIFI_Scan()
{
	Serial.println("WiFi Scaning...");
	WiFi.disconnect(true);
	int totalChannel = WiFi.scanNetworks();

	if (totalChannel == WIFI_SCAN_RUNNING || totalChannel == WIFI_SCAN_FAILED)
		return false;

	Serial.print("Total:");
	Serial.println(totalChannel);

	bool APfound = false;
	WIFI_Profile_t WiFiProfile(nullptr, nullptr);
	WIFI_Source.BeginNewScaning();

	for (size_t i = 0; i < totalChannel; i++)
	{
		WiFiProfile.ssid = "";
		WiFi.getNetworkInfo(i, WiFiProfile.ssid, WiFiProfile.EncryptionType, WiFiProfile.RSSI, WiFiProfile.BSSID, WiFiProfile.Channel);
		WIFI_Source.WiFi_Checking(WiFiProfile);
		Serial.printf("No.[%d], SSID:%s, Signal:%d\r\n", i, WiFiProfile.ssid.c_str(), WiFiProfile.RSSI);
	}

	Serial.println("WiFi scan done");
	return true;
}

bool WiFiConnectTo(char* ssid, char* password, uint32_t channel, uint8_t* bssid)
{
	Serial.printf("Connect to: %s\r\n", ssid);

	WiFi.mode(WIFI_MODE_STA);
	WiFi.disconnect();
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);

	if (channel != 0 && bssid != NULL)
	{
		WiFi.begin(ssid, password, channel, bssid);
	}
	else
		WiFi.begin(ssid, password);


	int timeout = 10000;
	while (timeout > 0 && WiFi.status() != WL_CONNECTED)
	{
		delay(10);
		timeout -= 10;
		Serial.print(".");
		Bnextion.Listening();
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		wifi_connected = true;
		if (WiFi.isConnected())
			EKanban.GetData();
	}
	else
		wifi_connected = false;

	Serial.printf("WIFI connect: %s\r\n", wifi_connected ? "Connected" : "Failed");

	return wifi_connected;
}

void WIFI_Connect2()
{
	if (!WIFI_Scan())
		return;

	CurrentWIFI = WIFI_Source.GetBestWiFiChannel();

	bool connectStatus = WiFiConnectTo(CurrentWIFI.SSID, CurrentWIFI.Password, CurrentWIFI.Channel, CurrentWIFI.BSSID);

	if (connectStatus)
		return;

	String oldSSID = String(CurrentWIFI.SSID);

	for (size_t i = 0; i < WIFI_Source.List.size(); i++)
	{
		if (WIFI_Source.List.at(i) == oldSSID)
			continue;

		if (!WIFI_Source.List.at(i).Found)
			continue;

		CurrentWIFI = WIFI_Source.List.at(i);

		bool status = WiFiConnectTo(CurrentWIFI.SSID, CurrentWIFI.Password, CurrentWIFI.Channel, CurrentWIFI.BSSID);
		if (status)
			break;
	}
}

void Setting_SaveEkanbanId(int id)
{
}

void Setting_AddWiFiAccount(char* ssid, char* password)
{
	touch_pad_intr_disable();
	Serial.println("Setting_AddWifiAccount");

	WIFI_Source.CurrentIndex = lcal::SaveWifiAccount(ssid, password, WIFI_Source.TotalAccount, WIFI_Source.MaximumSize);
	touch_pad_intr_enable();
}

void Setting_ReadWiFiSource()
{
	touch_pad_intr_disable();
	Serial.println("Setting_ReadWiFiSource");
	WIFI_Source.List.clear();
	for (size_t i = 0; i < WIFI_Source.TotalAccount; i++)
	{
		WIFI_Profile_t currentProfile;
		lcal::ReadWifiAccount(currentProfile.SSID, currentProfile.Password, i, WIFI_Source.MaximumSize);

		Serial.printf("Acount [%d], SSID: %s, PW: %s\r\n", i, currentProfile.SSID, currentProfile.Password);

		WIFI_Source.List.emplace_back(currentProfile);
	}

	touch_pad_intr_enable();

	WIFI_Source.UpdateNextionWiFi();
}

void Setting_ReadServerInformation()
{
	touch_pad_intr_disable();
	char server[20];
	uint16_t port;
	Serial.println("Setting_ReadServerInformation");
	lcal::ReadServerInformation(server, sizeof(server), port);
	Serial.printf("Read server: %s, port: %d\r\n", server, port);
	String serverStr = String(server).c_str();
	EClient.SetupHost(serverStr, port);
	UpdateServerIP(server, port);
	touch_pad_intr_enable();
}

void Setting_SaveServerInformation(char* server, uint8_t len, uint16_t port)
{
	touch_pad_intr_disable();
	Serial.println("Setting_WriteServerInformation");
	Serial.printf("Save servre: %s, port: %d\r\n", server, port);
	lcal::SaveServerInformation(server, len, port);
	touch_pad_intr_enable();
}

void Setting_SaveServerInformation(char* server, uint8_t len)
{
	touch_pad_intr_disable();
	Serial.println("Setting_WriteServerInformation");
	Serial.printf("Save servre: %s\r\n", server);
	lcal::WriteServerIp(server, len);
	touch_pad_intr_enable();
}

void Setting_SaveServerInformation(uint16_t port)
{
	touch_pad_intr_disable();
	Serial.println("Setting_WriteServerInformation");
	Serial.printf("Save port: %d\r\n", port);
	lcal::WriteServerPort(port);
	touch_pad_intr_enable();
}

#endif

