#include "BNextion.h"
#include <string.h>

BNextion::BNextion(int _baud, int _timeout)
{
	baud = _baud;
	timeout = _timeout;
	for (int i = 0; i < TOTAL_NEXTION_PAGE; i++)
	{
		Page.push_back(NextionPage(nex, i, 0, PageName[i]));
	}
}

void BNextion::init()
{

#if USE_DEFAULT_SERIAL
	Serial.begin(baud);
	while (!Serial);
	Serial.setTimeout(timeout);
#else
	mySerial.begin(baud);
	while (!mySerial);
	mySerial.setTimeout(timeout);
#endif

	char* prt;

	Serial.println("Initialize Nextion screen number name");
	for (int i = 0; i < TOTAL_NEXTION_NUMBER; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'n';
		prt[1] = i + 48;
		if (i >= 10)
		{
			prt[1] = i / 10 + 48;
			prt[2] = i % 10 + 48;
		}
		NexNumber[i] = prt;
		Serial.println(NexNumber[i]);
	}

	Serial.println("Initialize Nextion screen button name");
	for (int i = 0; i < TOTAL_NEXTION_BUTTON; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'b';
		prt[1] = i + 48;
		if (i >= 10)
		{
			prt[1] = i / 10 + 48;
			prt[2] = i % 10 + 48;
		}
		NexButton(i) = prt;
		Serial.println(NexButton(i));
	}

	Serial.println("Initialize Nextion screen text name");
	for (int i = 0; i < TOTAL_NEXTION_TEXT; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 't';
		prt[1] = i + 48;
		if (i >= 10)
		{
			prt[1] = i / 10 + 48;
			prt[2] = i % 10 + 48;
		}
		NexText(i) = prt;
		Serial.println(NexText(i));
	}

	Serial.println("Initialize Nextion screen text name");
	for (int i = 0; i < TOTAL_NEXTION_POTEXT; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'p';
		prt[1] = 'o';
		prt[2] = i + 48;
		if (i >= 10)
		{
			prt[2] = i / 10 + 48;
			prt[3] = i % 10 + 48;
		}
		NexPo(i) = prt;
		Serial.println(NexPo(i));
	}

	Serial.println("Initialize Nextion screen Variable name");
	for (int i = 0; i < TOTAL_NEXTION_VARIABLE; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'v';
		prt[1] = 'a';
		prt[2] = i + 48;
		if (i >= 10)
		{
			prt[2] = i / 10 + 48;
			prt[3] = i % 10 + 48;
		}
		NexVa(i) = prt;
		Serial.println(NexVa(i));
	}

	delete prt;
	//GotoPage(0);
}

//---------------------------------------------------------------------------------------------------
void BNextion::setNumberProperty(uint8_t _pageId, char* _property, uint32_t _number)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return;
	Page.at(_pageId).setNumberProperty(_property, _number);
}

void BNextion::setStringProperty(uint8_t _pageId, char* _property, char* _text)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return;
	Page.at(_pageId).setStringProperty(_property, _text);
}

void BNextion::getStringProperty(uint8_t _pageId, char* _property, char* _value, size_t _len)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return;
	Page.at(_pageId).getStringProperty(_property, _value, _len);
}

uint32_t BNextion::getNumberProperty(uint8_t _pageId, char* _property)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return 0;
	return Page.at(_pageId).getNumberProperty(_property);
}
//---------------------------------------------------------------------------------------------------

void BNextion::setPage_stringProperty(uint8_t _pageId, char* _property, char* _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	setStringProperty(_pageId, propName, _value);
}

uint32_t BNextion::getPage_numberProperty(uint8_t _pageId, char* _property, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, true);
	uint32_t result = getNumberProperty(_pageId, propName);
	return result;
}

void BNextion::getPage_stringProperty(uint8_t _pageId, char* _property, char* _result, size_t _resultLen, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	getStringProperty(_pageId, propName, _result, _resultLen);
}

void BNextion::setPage_numberProperty(uint8_t _pageId, char* _property, uint32_t _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, true);
	setNumberProperty(_pageId, propName, _value);
}

//-------------------------------------------------------------------------------------------------
void BNextion::setPage_stringProperty(uint8_t _pageId, const char* _property, char* _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	setStringProperty(_pageId, propName, _value);
}

void BNextion::setPage_numberProperty(uint8_t _pageId, const char* _property, uint32_t _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, true);
	setNumberProperty(_pageId, propName, _value);
}

void BNextion::setPage_stringAsNumberProperty(uint8_t _pageId, const char* _property, uint32_t _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	char value[10];

	sprintf(value, "%d", _value);
	setStringProperty(_pageId, propName, value);
}

void BNextion::setPage_stringAsNumberProperty(uint8_t _pageId, char* _property, uint32_t _value, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	char value[10];

	sprintf(value, "%d", _value);
	setStringProperty(_pageId, propName, value);
}

uint32_t BNextion::getPage_numberProperty(uint8_t _pageId, const char* _property, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, true);
	return getNumberProperty(_pageId, propName);
}

void BNextion::getPage_stringProperty(uint8_t _pageId, const char* _property, char* _result, size_t _resultLen, size_t _propLen)
{
	char propName[_propLen];
	getPropertyName(_property, propName, false);
	Serial.println(_resultLen);
	getStringProperty(_pageId, propName, _result, _resultLen);
}

void BNextion::setPage_PropertyForceColor(uint8_t _pageId, const char* _property, uint16_t _color)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return;

	char* propName = new char[50];
	sprintf(propName, "%s.pco", _property);
	NexPage(_pageId).setForceColor(propName, _color);
	delete propName;
}

void BNextion::setPage_PropertyBackgroundColor(uint8_t _pageId, const char* _property, uint16_t _color)
{
	if (_pageId >= TOTAL_NEXTION_PAGE)
		return;

	char* propName = new char[50];
	sprintf(propName, "%s.bco", _property);
	NexPage(_pageId).setForceColor(propName, _color);
	delete propName;
}

//------------------------------------------------------------------------
void BNextion::getPropertyName(char* _propertyName, char* _result, bool _isNumber)
{
	int type = 0;
	if (!_isNumber)
		type = 1;

	sprintf(_result, "%s%s", _propertyName, propType[type]);

	Serial.println(_result);
}

void BNextion::getPropertyName(const char* _propertyName, char* _result, bool _isNumber)
{
	int type = 0;
	if (!_isNumber)
		type = 1;

	sprintf(_result, "%s%s", _propertyName, propType[type]);

	Serial.println(_result);
}
