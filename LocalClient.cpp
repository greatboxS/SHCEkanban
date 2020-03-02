#include "LocalClient.h"

void LocalClient::Run()
{
	if (client.available() > 0)
	{
		while (client.available() > 0)
		{
			received_data_Callback(client);
		}
	}
}

void LocalClient::making_url(const String& _ext, uint8_t _method)
{
	Url.remove(0, Url.length());
	switch (_method)
	{
	case 1:
		Url = GetUrl;
		break;
	case 2:
		Url = PostUrl;
		break;
	case 3:
		Url = PutUrl;
		break;
	case 4:
		Url = DeletetUrl;
		break;
	default:
		Url = GetUrl;
		break;
	}

	Url.replace("%s", _ext);
}

void LocalClient::init(void(*_received_data_Callback)(WiFiClient& _stream))
{
	Url.reserve(128);
	received_data_Callback = _received_data_Callback;
}

bool LocalClient::begin_connection()
{
	int serverPort = 0;

	serverPort = ServerPort;

	if (strlen(server) == 0)
		return false;

	client.connect(server, serverPort);
	int i = 0;
	Serial.printf("Connect to server: %s:%d\r\n", server, ServerPort);
	Serial.printf("Timeout: %d\r\n", InitializeConnect_Timeout);
	while (!client.connected() && i < InitializeConnect_Timeout)
	{
		delay(100);
		Serial.print(".");
		i++;
	}
	return client.connected();
}

void LocalClient::begin_Request(const String& _url, uint8_t _method)
{
	if (!client.connected())
	{
		if (!begin_connection())
			return;
	}

	making_url(_url, _method);
	//

	client.println(Url);
	Serial.println(Url);
	Serial.println(Host);
	client.println(Host);

	client.println(Connection);
	client.println(KeepAlive);
	client.println();
}


void LocalClient::DisconnectFromServer()
{
	client.stop();
}
