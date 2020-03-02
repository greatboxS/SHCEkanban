
#include "WiFiClient.h"
#include <cstdint>

// Initialize the ethernet SPI interface 
// CS_PIN 


class LocalClient
{
private:
	const char* GetUrl = "GET /%s HTTP/1.1";
	const char* PostUrl = "POST /%s HTTP/1.1";
	const char* PutUrl = "PUT /%s HTTP/1.1";
	const char* DeletetUrl = "DELETE /%s HTTP/1.1";

	String Url = "";
	//const char* Host = "Host: 10.4.2.23:32768";
	char Host[50] = "Host: 192.168.1.5:32765";
	const char* Connection = "Connection: Keep-Alive";
	const char* KeepAlive = "Keep-Alive: timeout=10, max=1000";

	byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	char server[20] = "192.168.1.5";
	int ServerPort = 32765;
	int InitializeConnect_Timeout = 20;
	int tick = 0;
	bool lanConntected = false;
	bool serverConnected = false;

	void(*received_data_Callback)(WiFiClient&);
	void making_url(const String& _ext, uint8_t _method = 1);

	

public:

	WiFiClient client;
	void init(void(*_received_data_Callback)(WiFiClient& _stream));
	void Run();

	bool begin_connection();

	void begin_Request(const String& _url, uint8_t _method = 1);

	void DisconnectFromServer();

	void SetupHost(String& serverName, uint16_t port)
	{
		Serial.println("SetupHost");
		ServerPort = port;
		memccpy(server, serverName.c_str(), 0, sizeof(server));
		ApplyServer();
		Serial.println("Setup done!");
		
	}

	void SetupHost(char* serverName, uint16_t port)
	{
		Serial.println("SetupHost");
		ServerPort = port;
		memccpy(server, serverName, 0, sizeof(server));
		ApplyServer();
		Serial.println("Setup done!");

	}

	void SetServer(String& serverName)
	{
		Serial.println("Set server");
		memccpy(server, serverName.c_str(), 0, sizeof(server));
		ApplyServer();
	}

	void SetPort(uint16_t  port)
	{
		Serial.println("Set Port");
		ServerPort = port;
		ApplyServer();
	}

	void ApplyServer()
	{
		Serial.println("ApplyServer");
		snprintf(Host, sizeof(Host), "Host: %s:%d", server, ServerPort);
		Serial.printf("Server: %s\r\nHost: %s\r\n", server, Host);
	}
};