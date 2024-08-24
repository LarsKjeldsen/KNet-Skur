#include "Network.h"
#include "Reading.h"
#include "WIFI-Secret.h"
#include <ArduinoOTA.h>
#include <EEPROM.h>

char ssid[] = SSID_NAME;
char password[] = PASSWORD;


IPAddress ip(192, 168, 1, 252);
//IPAddress ip(192, 168, 1, 201); //DEBUG
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);
IPAddress MQTTServerIP(192, 168, 1, 22);

WiFiClient ethClient;
MQTTClient MQTTclient;
HTTPClient httpClient;



bool WiFi_Setup(int ErrorCount)
{ 
	if (WiFi.status() == wl_status_t::WL_CONNECTED)
		return true;

	Serial.printf("W1 %d %d \n", ErrorCount, WiFi.status());

	WiFi.disconnect(false);
	WiFi.mode(WIFI_STA);
	WiFi.config(ip, gw, mask);

	WiFi.begin(ssid, password);
	delay(500);
	return (WiFi.status() == wl_status_t::WL_CONNECTED);
}


bool MQTT_Setup()
{
	if (not MQTTclient.connected()) 
	{
		if (WiFi.status() != wl_status_t::WL_CONNECTED)
			WiFi_Setup(0);
		Serial.print("M1 ");
		MQTTclient.begin(MQTTServerIP, ethClient);
		MQTTclient.setKeepAlive(120);
		MQTTclient.connect("KNet-Skur-V3");
		delay(250);
	}

	return MQTTclient.connected();
}


void MQTT_Loop()
{
	MQTTclient.loop();
}


bool Send_reading(Reading* r, int ErrorCount)
{
	if (not WiFi_Setup(ErrorCount))
	{
		Serial.println("WiFi not connected");
		return false;
	}

	if (not MQTT_Setup())
	{
		Serial.println("MQTT not connected");
		return false;
	}

	SendMQTT("KNet/Haven/Vejr/Temperatur", r->Temp);      delay(100);
	SendMQTT("KNet/Haven/Vejr/Fugtighed", r->Humid);      delay(100);
	SendMQTT("KNet/Haven/Vejr/Lufttryk", r->Press);       delay(100);

	SendMQTT("KNet/Haven/Skur/Solar1_mA", r->Solar1_mA);   delay(100);
	SendMQTT("KNet/Haven/Skur/Solar1_V", r->Solar1_V);     delay(100);
	SendMQTT("KNet/Haven/Skur/Solar2_mA", r->Solar2_mA);   delay(100);
	SendMQTT("KNet/Haven/Skur/Solar2_V", r->Solar2_V);     delay(100);
	SendMQTT("KNet/Haven/Skur/Battery_V", r->Battery_V);   delay(100);
	SendMQTT("KNet/Haven/Skur/Charger_mA", r->Charger_mA); delay(100);
	SendMQTT("KNet/Haven/Skur/Charger_V", r->Charger_V);   delay(100);
	SendMQTT("KNet/Haven/Skur/Load1_mA", r->Load1_mA);     delay(100);
	SendMQTT("KNet/Haven/Skur/Load2_mA", r->Load2_mA);     delay(100);
	SendMQTT("KNet/Haven/Skur/Load3_mA", r->Load3_mA);     delay(100);
	SendMQTT("KNet/Haven/Skur/Load4_mA", r->Load4_mA);     delay(100);
	
	if (r->Vandstand_mm != 0) 
		SendMQTT("KNet/Haven/Regn/vandstand_mm", (int32_t)r->Vandstand_mm);

	delay(1000);

	Maintanance_mode = GetStatusCode();

	return true;
}

bool SendMQTT(const char* topic, int32_t payload)
{
	return (MQTTclient.publish(topic, String(payload)));
}

bool SendMQTT(const char* topic, float payload)
{
	return (MQTTclient.publish(topic, String(payload)));
}

bool SendMQTT(const char* topic, char *payload)
{
	return(MQTTclient.publish(topic, payload));
}


int GetStatusCode()
{
	httpClient.begin("http://192.168.1.20:8123/api/states/input_boolean.skur_debug");
	httpClient.setReuse(false);
	httpClient.addHeader("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI2YWJiZDM4Yzc2YTk0ZTQ4YTg3YTlkM2FkOGFkNDVkZiIsImlhdCI6MTY2NzY0NTY4MywiZXhwIjoxOTgzMDA1NjgzfQ.-QBAQvT2DCn8UPiw7oTD_iGXMnExdsBvFh1-X8DBmJo");
	httpClient.addHeader("Content-Type", "application/json");

	int httpCode = httpClient.GET();

	if (httpCode == -11)  // Try again.
	{
		httpCode = httpClient.GET();
	}


	if (httpCode == 200) { //Check for the returning code

		String payload = httpClient.getString();
//		Serial.println(payload);

		cJSON* root = cJSON_Parse(payload.c_str());
		if (root == NULL) {
			cJSON_Delete(root);
			return false;
		}

		String besked_raw = cJSON_GetObjectItem(root, "state")->valuestring;
		cJSON_Delete(root);

		if (besked_raw == NULL)
			return false;
		if (besked_raw == "on")
			return true;
	}
	return false;
}


void OTA_Setup()
{
	ArduinoOTA.setHostname("KNetSkurV2");

	ArduinoOTA.onStart([]() {
		Serial.println("OTA Start ");
		});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
		});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		unsigned int procent = (progress * 100) / total;
		if ((procent % 5) == 0)
			Serial.print('.');
		});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
		});
	ArduinoOTA.begin();


	Serial.println("");
	Serial.print("WiFi connected IP address: ");
	Serial.println(WiFi.localIP());
}

void My_Esp_Restart(String ErrorText)
{
	EEPROM.write(0, max((int)ErrorText.length(), EEPROM_SIZE-2)); // need room for size and trailing zero
	EEPROM.writeString(1, ErrorText);
	EEPROM.commit();
	sleep(1);
	ESP.restart();
}