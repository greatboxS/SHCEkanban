/*
 Name:		EkanbanV3.ino
 Created:	11/11/2019 8:33:28 AM
 Author:	Dell-PC
*/

#define SCREEN_LIGHT_CONTROL_PIN 23

#define log(...) Serial.print(__VA_ARGS__)
#define log_ln(...) Serial.println(__VA_ARGS__)
#define f_log(...) Serial.println(#__VA_ARGS__)
// the setup function runs once when you press reset or power the board
#include "EConfig.h"
#include "OTAConfig.h"


long ticks = 0;
volatile bool requestNow = false;
volatile int requestTime = 0;
void Task_Main(void* parameter)
{
	for (;;)
	{
		EClient.Run();
		vTaskDelay(10);
	}
}

void Task_Nextion_Display(void* parameter)
{
	for (;;)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			WIFI_Connect2();
		}

		if (requestNow)
		{
			requestNow = false;
			EKanban.GetData();
			Serial.printf("Free heap size %d\r\n", esp_get_free_heap_size());
		}

		if (Serial.available() > 0)
		{
			String s = Serial.readString();
			Serial.println(s);

			if (s.indexOf("name") > -1)
			{
				s.remove(0, s.indexOf(":") + 1);

				memccpy(ssid, s.c_str(), 0, sizeof(ssid));
				Serial.printf("Name: %s\r\n", ssid);
			}

			if (s.indexOf("password") > -1)
			{
				s.remove(0, s.indexOf(":") + 1);
				s.replace(" ", "");

				memccpy(password, s.c_str(), 0, sizeof(password));
				Serial.printf("Password: %s\r\n", password);
			}

			if (s.indexOf("save") > -1)
			{
				size_t size = WIFI_Source.AddWiFi(ssid, password);
				Serial.printf("Size: %d\r\n", size);
				Setting_AddWiFiAccount(ssid, password);

				Setting_ReadWiFiSource();
				//BUT_ADD_CALLBACK((NextionEventType)1, NULL);
			}

			if (s.indexOf("connect") > -1)
			{
				WIFI_Connect2();
			}

			if (s.indexOf("scan") > -1)
			{
				WIFI_Scan();
			}

			if (s.indexOf("get-data") > -1)
			{
				EKanban.GetData();
			}

			if (s.indexOf("confirm") > -1)
			{
				EKanban.Confirm();
			}

			if (s.indexOf("server") > -1)
			{
				while (true)
				{
					if (Serial.available() > 0)
					{
						Serial.println("Server Name: ");
						String Servername = Serial.readString();
						EClient.SetServer(Servername);
						char server[20];
						Servername.toCharArray(server, sizeof(server));
						Setting_SaveServerInformation(server, sizeof(server));
						break;
					}
					delay(5);
				}
			}

			if (s.indexOf("port") > -1)
			{
				while (true)
				{
					if (Serial.available() > 0)
					{
						Serial.println("Server Port: ");
						String number = Serial.readString();
						uint16_t port = number.toInt();
						EClient.SetPort(port);
						Setting_SaveServerInformation(port);
						break;
					}
					delay(5);
				}
			}

			if (s.indexOf("reset") > -1)
			{
				ESP.restart();
			}
		}

		if (WINDOWS_STATE >= 50)
		{
			touch_pad_intr_disable();
			WINDOWS_STATE = 0;
			while (!WiFi.isConnected()) {
				delay(10);
			}
			Bnextion.GotoPage(6);
			OTA_Cofiguration();
		}

		Bnextion.Listening();
		vTaskDelay(2);
		
	}

}

void RootRTCTimer(TimerHandle_t pxTimer)
{
	f_log(RootRTCTimer);
	NextionUpdateWIFI();
	DATETIME.localSec++;
	if (DATETIME.localSec >= 60)
	{
		DATETIME.localSec = 0;
		DATETIME.localMin++;
		if (DATETIME.localMin >= 60)
		{
			DATETIME.localHour++;
			if (DATETIME.localHour >= 24)
			{
				DATETIME.localHour = 0;
			}
		}
	}
}

void RequestTimer(TimerHandle_t pxTimer)
{
	f_log(RequestTimer);
	if (WiFi.isConnected())
	{
		if (EKanban.ScreenOff)
		{
			requestTime++;
			if (requestTime >= 6)
			{
				requestTime = 0;
				requestNow = true;
			}
		}
		else
		{
			requestNow = true;
		}
	}
}

void setup()
{
	Serial.begin(115200);
	while (!Serial);
	System_startup();

	BaseType_t error;

	error = xTaskCreatePinnedToCore(Task_Main, "Task_Nextion_Display", 1024 * 100, NULL, 1, NULL, 1);

	Serial.println(error);

	error = xTaskCreatePinnedToCore(Task_Nextion_Display, "Task_Main", 1024 * 50, NULL, 2, NULL, 0);

	Serial.println(error);

	TimerHandle_t rootTimer = xTimerCreate("RootRTCTimer",       // Just a text name, not used by the kernel.
		(1000),   // The timer period in ticks.
		pdTRUE,        // The timers will auto-reload themselves when they expire.
		NULL,  // Assign each timer a unique id equal to its array index.
		RootRTCTimer // Each timer calls the same callback when it expires.
	);

	TimerHandle_t requestTimer = xTimerCreate("RequestTimer", (5000), pdTRUE, NULL, RequestTimer);

	if (rootTimer == NULL)
	{
	}
	else
	{
		// Start the timer.  No block time is specified, and even if one was
			// it would be ignored because the scheduler has not yet been
			// started.
		if (xTimerStart(rootTimer, 0) != pdPASS)
		{
			log_ln("Can not start root timer");
		}
		else
			log_ln("start root timer successfully");

	}

	if (requestTimer == NULL)
	{
	}
	else
	{
		if (xTimerStart(requestTimer, 0) != pdPASS)
		{
			log_ln("Can not start request timer");
		}
		else
			log_ln("start request timer successfully");

	}
}


// the loop function runs over and over again until power down or reset
void loop() {


}
