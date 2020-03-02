#include <NextionVariableString.h>
#include <NextionVariableNumeric.h>
#include <NextionText.h>
#include <NextionPicture.h>
#include <NextionPage.h>
#include <NextionNumber.h>
#include <NextionButton.h>
#include <Nextion.h>
#include <vector>
#include <INextionTouchable.h>
// Select which serial port using for Nextion Screen
#define USE_DEFAULT_SERIAL false

//// Hardware serial 
#define NEXTION_RX_PIN FUNC_GPIO16_GPIO16
#define NEXTION_TX_PIN FUNC_GPIO17_GPIO17

// define max length of nextion property name
#define PROPERTY_NAME_MAX_LEN 24

#define TOTAL_NEXTION_PAGE 7
#define TOTAL_NEXTION_BUTTON 5
#define TOTAL_NEXTION_NUMBER 5
#define TOTAL_NEXTION_VARIABLE 5
#define TOTAL_NEXTION_TEXT 100
#define TOTAL_NEXTION_POTEXT 15

// Easy array call
#define NexPage(x) Page[x]
#define NexPageNumber(x) PageNumber[x]
#define NexNumber(x) NexNumber[x]
#define NexText(x) NexText[x]
#define NexButton(x) NexButton[x]
#define NexPo(x) NexPo[x]
#define NexVa(x) NexVa[x]

// Page Id 
#define Comp1P		0
#define Comp2P		1
#define SizeP		2
#define MbxP		3
#define SettingP	4
// 

// page 1
#define COMP1_BUT_NEXT_ID				22	
#define COMP1_TXT_PO					14
#define COMP1_TXT_LINE					16
#define COMP1_TXT_PO_QTY				17
#define COMP1_TXT_SEQUENCE				15
#define COMP1_TXT_SEQUENCE_QTY			18
#define COMP1_TXT_CONFIRM_STATUS		19
#define COMP1_TXT_BEGIN_COMP			0
#define COMP1_NUM_CART					0

//page 2
#define COMP2_TXT_BEGIN_COMP			5

//page 3
#define SIZE_BUT_CONFIRM_ID			4	
#define  SIZE_TXT_BEGIN_SIZE		5

//page 4
#define MBX_BUT_YES_ID			3	
#define MBX_BUT_NO_ID			4	
#define MBX_TXT_MESSAGE			0

//page 5
#define SET_BUT_SAVE_ID			16	
#define SET_BUT_ADD_ID			17	
#define SET_BUT_RESET_ID		18	
#define SET_BUT_MAIN_ID			19	
#define SET_BUT_SAVE_SERVER_IP	21

#define SET_TXT_USER_ID			14	
#define SET_TXT_PASS_ID			15
#define SET_NUM_CART_ID			21	
#define SET_TXT_BEGIN_WIFI_ID	0	
//template<typename T>
//constexpr auto GET_CHARS(T x) { return const_cast<char*>(x); }

class BNextion
{
protected:
	int baud;
	int timeout;
	const char* propType[2] = { ".val", ".txt" };
#if USE_DEFAULT_SERIAL
	Nextion nex = Nextion(Serial);
#else
	HardwareSerial mySerial = HardwareSerial(2); // RX, TX
	Nextion nex = Nextion(mySerial);
#endif

public:

	// define Name of variables  
	const char* SELECT_PO = "SELECT_PO";
	const char* SELECT_SEQ = "SELECT_SEQ";
	const char* SELECT_SIZE = "SELECT_SIZE";
	const char* IS_CUTTING = "IS_CUTTING";
	const char* CLEAR_SIZE = "CLEAR_SIZE";
	const char* SELECT_COM = "SELECT_COM";
	const char* EthernetStatus = "Ethernet";


	const char* NexNumber[TOTAL_NEXTION_NUMBER];
	const char* NexText[TOTAL_NEXTION_TEXT];
	const char* NexButton[TOTAL_NEXTION_BUTTON];
	const char* NexPo[TOTAL_NEXTION_POTEXT];
	const char* NexVa[TOTAL_NEXTION_VARIABLE];

	const uint8_t PageNumber[TOTAL_NEXTION_PAGE]{ 0,1,2,3,4,5,6};
	const char* PageName[TOTAL_NEXTION_PAGE]{ "Comp1P","Comp2P","SizeP","MbxP", "SettingP", "Mbx2P","Boot" };

	NextionButton BUT_NEXT1 = NextionButton(nex, Comp1P, COMP1_BUT_NEXT_ID, "b1");

	NextionButton BUT_NEXT2 = NextionButton(nex, Comp2P, COMP1_BUT_NEXT_ID, "b1");

	NextionButton BUT_CONFIRM = NextionButton(nex, SizeP, SIZE_BUT_CONFIRM_ID, "b2");

	NextionButton BUT_MBX_YES = NextionButton(nex, MbxP, SIZE_BUT_CONFIRM_ID, "b0");

	NextionButton BUT_MBX_NO = NextionButton(nex, MbxP, SIZE_BUT_CONFIRM_ID, "b1");

	NextionButton BUT_SAVE_ID = NextionButton(nex, SettingP, SET_BUT_SAVE_ID, "b0");

	NextionButton BUT_ADD = NextionButton(nex, SettingP, SET_BUT_ADD_ID, "b1");

	NextionButton BUT_RESET = NextionButton(nex, SettingP, SET_BUT_RESET_ID, "b2");

	NextionButton BUT_MAIN = NextionButton(nex, SettingP, SET_BUT_MAIN_ID, "b3");

	NextionButton BUT_SAVE_IP = NextionButton(nex, SettingP, SET_BUT_SAVE_SERVER_IP, "b4");

	INextionTouchable PAGE_LOADING= INextionTouchable(nex);

	// Nextion Page vector
	// Define all Nextion Page
	std::vector<NextionPage> Page;

	BNextion(int _baud, int _timeout);

	/*---------------------------------------------------------------------------------------*/
	void init();
	//----------------------------------------------------------------------------------------------------------------
	void setNumberProperty(uint8_t _pageId, char* _property, uint32_t _number);
	void setStringProperty(uint8_t _pageId, char* _property, char* text);
	void getStringProperty(uint8_t _pageId, char* _property, char* _value, size_t _len);
	uint32_t getNumberProperty(uint8_t _pageId, char* _property);
	//----------------------------------------------------------------------------------------------------------------
	void setPage_stringProperty(uint8_t _pageId, char* _property, char* _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);
	void setPage_stringProperty(uint8_t _pageId, const char* _property, char* _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);

	void setPage_numberProperty(uint8_t _pageId, char* _property, uint32_t _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);
	void setPage_numberProperty(uint8_t _pageId, const char* _property, uint32_t _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);

	void setPage_stringAsNumberProperty(uint8_t _pageId, const char* _property, uint32_t _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);
	void setPage_stringAsNumberProperty(uint8_t _pageId, char* _property, uint32_t _value, size_t _propLen = PROPERTY_NAME_MAX_LEN);

	uint32_t getPage_numberProperty(uint8_t _pageId, char* _property, size_t _propLen = PROPERTY_NAME_MAX_LEN);
	uint32_t getPage_numberProperty(uint8_t _pageId, const char* _property, size_t _propLen = PROPERTY_NAME_MAX_LEN);

	void getPage_stringProperty(uint8_t _pageId, char* _property, char* _result, size_t _resultLen, size_t _propLen = PROPERTY_NAME_MAX_LEN);
	void getPage_stringProperty(uint8_t _pageId, const char* _property, char* _result, size_t _resultLen, size_t _propLen = PROPERTY_NAME_MAX_LEN);

	void setPage_PropertyForceColor(uint8_t _pageId, const char* _property, uint16_t _color);
	void setPage_PropertyBackgroundColor(uint8_t _pageId, const char* _property, uint16_t _color);

	void showMessage(char* _message)
	{
		GotoPage(MbxP);
		setPage_stringProperty(MbxP, NexText(MBX_TXT_MESSAGE), _message);
	}

	//----------------------------------------------------------------------------------------------------------------
	// Property Name converter
	void getPropertyName(char* _propertyName, char* _result, bool _isNumber = false);
	void getPropertyName(const char* _propertyName, char* _result, bool _isNumber = false);
	//----------------------------------------------------------------------------------------------------------------

	void GotoPage(uint8_t _page) { NexPage(_page).show(); }

	// Listen to Nextion device
	void Listening() { nex.poll(); }
};
