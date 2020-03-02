#ifndef __LOCAL_BACKUP_H
#define __LOCAL_BACKUP_H

#define DATE_TIME_START_ADDRESS 0
#define PO_ID_ADDRESS 10
#define SEQ_ID_ADDRESS 14
#define COMPONENT_ID_ADDRESS 18

#define CART_NUMBER_ADDRESS 0
#define SERVER_IP_ADDRESS 10
#define SERVER_PORT_ADDRESS 30
#define ELEMENT_INDEX_ADDRESS 50
#define START_WIFI_ADDRESS 100

#include "EEPROM.h"
#include "BTypedef.h"

namespace lcal {

	void SaveTime(DateTime_t* _datetime, bool isReading);

	template <typename T>
	void SaveCuttingInfo(T _poId, T _seqId, T _comId);

	template <typename T>
	void ReadCuttingInfo(T& _poId, T& _seqId, T& _comId);

	template<typename T >
	void write(T _value, int _address);

	template<typename T>
	T read(int _address);

	uint8_t ReadIndex();

	void WriteIndex(uint8_t value);

	//------------------------------------------------------------------------------
	void SaveTime(DateTime_t* _datetime, bool isReading)
	{
		EEPROM.begin(1024);
		for (uint8_t i = 0; i < 6; i++)
		{
			if (!isReading)
				EEPROM.write(DATE_TIME_START_ADDRESS + i, _datetime->arr[i]);
			else
				_datetime->arr[i] = EEPROM.read(DATE_TIME_START_ADDRESS + i);
		}

		EEPROM.end();
	}

	template <typename T>
	void SaveCuttingInfo(T _poId, T _seqId, T _comId)
	{
		EEPROM.begin(1024);
		Serial.println(_poId);
		Serial.println(_seqId);
		Serial.println(_comId);

		write<T>(_poId, PO_ID_ADDRESS);
		write<T>(_seqId, SEQ_ID_ADDRESS);
		write<T >(_comId, COMPONENT_ID_ADDRESS);
		EEPROM.end();
	}

	template <typename T>
	void ReadCuttingInfo(T& _poId, T& _seqId, T& _comId)
	{
		EEPROM.begin(1024);

		Serial.println(_poId);
		Serial.println(_seqId);
		Serial.println(_comId);

		_poId = read<T>(PO_ID_ADDRESS);
		_seqId = read<T>(SEQ_ID_ADDRESS);
		_comId = read<T>(COMPONENT_ID_ADDRESS);

		EEPROM.end();
	}


	void ReadWifiAccount(char* ssid, char* password, int index, size_t len)
	{
		EEPROM.begin(1024);

		int address = index * len * 2 + START_WIFI_ADDRESS;

		for (size_t i = address; i < len + address; i++)
		{
			*ssid = EEPROM.read(i);
			++ssid;
		}
		address += len;

		for (size_t i = address; i < len + address; i++)
		{
			*password = EEPROM.read(i);
			++password;
		}

		EEPROM.end();
	}


	uint8_t SaveWifiAccount(char* ssid, char* password, uint8_t totalAccount, size_t len)
	{
		Serial.println("SaveWifiAccount");

		uint8_t ElementIndex = ReadIndex();

		if (ElementIndex >= totalAccount)
			ElementIndex = 0;

		EEPROM.begin(1024);

		int address = ElementIndex * len * 2 + START_WIFI_ADDRESS;

		for (size_t i = address; i < len + address; i++)
		{
			EEPROM.write(i, *ssid);
			++ssid;
		}

		address += len;
		for (size_t i = address; i < len + address; i++)
		{
			EEPROM.write(i, *password);
			++password;
		}

		EEPROM.end();

		ElementIndex++;
		WriteIndex(ElementIndex);

		return ElementIndex;
	}

	template<typename T>
	void WriteServerPort(T port)
	{
		EEPROM.begin(1024);
		write(port, SERVER_PORT_ADDRESS);
		EEPROM.end();
	}

	template<typename T>
	void ReadServerPort(T& port)
	{
		EEPROM.begin(1024);
		port = read<T>(SERVER_PORT_ADDRESS);
		EEPROM.end();
	}

	void ReadServerIp(char* serverIp, size_t len)
	{
		EEPROM.begin(1024);

		for (size_t i = SERVER_IP_ADDRESS; i < SERVER_IP_ADDRESS + len; i++)
		{
			*serverIp = EEPROM.read(i);
			++serverIp;
		}
		EEPROM.end();
	}


	void WriteServerIp(char* serverIp, size_t len)
	{
		EEPROM.begin(1024);

		for (size_t i = SERVER_IP_ADDRESS; i < SERVER_IP_ADDRESS + len; i++)
		{
			EEPROM.write(i, *serverIp);
			++serverIp;
		}
		EEPROM.end();
	}

	template <typename T>
	void SaveServerInformation(char* serverIp, uint8_t len, T& port)
	{
		WriteServerIp(serverIp, len);
		WriteServerPort(port);
	}

	template <typename T>
	void ReadServerInformation(char* serverIp, uint8_t len, T& port)
	{
		ReadServerIp(serverIp, len);
		ReadServerPort(port);
	}


	uint8_t ReadIndex()
	{
		uint8_t value;
		Serial.println("ReadIndex:");
		EEPROM.begin(1024);
		value = EEPROM.read(ELEMENT_INDEX_ADDRESS);
		if (value >= 255)
			value = 0;

		Serial.print("ElementIndex:");
		Serial.println(value);
		EEPROM.end();
		return value;
	}

	void WriteIndex(uint8_t value)
	{
		EEPROM.begin(1024);
		Serial.printf("WriteIndex: %d\r\n", value);
		EEPROM.write(ELEMENT_INDEX_ADDRESS, value);
		EEPROM.end();
	}

	template<typename T>
	void write(T _value, int _address)
	{
		Serial.println("Eeprom write");
		uint8_t size = sizeof(T);
		Serial.println(size);

		for (uint8_t i = 0; i < size; i++)
		{
			uint8_t value = _value >> ((size - 1 - i) * 8);
			EEPROM.write(_address + i, value);
			Serial.println(value, HEX);
		}
	}

	template<typename T>
	T read(int _address)
	{
		Serial.println("Eeprom read");
		uint8_t size = sizeof(T);
		Serial.println(size);
		T result;

		for (uint8_t i = 0; i < size; i++)
		{
			uint8_t sLeft = (size - 1 - i) * 8;
			result |= (T)(EEPROM.read(_address + i) << sLeft);
			Serial.println(result, HEX);
		}

		return result;
	}
}
#endif