#include "Network.h"
#include "Reading.h"
#include "WIFI-Secret.h"
#include <ArduinoOTA.h>
#include <EEPROM.h>

char ssid[] = SSID_NAME;
char password[] = PASSWORD;

const char clientId[] = "Skur_V2";

IPAddress ip(192, 168, 1, 252);
// IPAddress ip(192, 168, 1, 120); //DEBUG
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);

WiFiClient ethClient;

IPAddress MQTTServer(192, 168, 1, 21);
PubSubClient MQTTclient(ethClient);

HTTPClient httpClient;



bool WiFi_Setup()
{
	Serial.print("WIFI Setup");

	WiFi.mode(WIFI_STA);

	WiFi.config(ip, gw, mask);
	WiFi.begin(ssid, password);
	return (WiFi.status() == wl_status_t::WL_CONNECTED);
}


void WIFI_disconnect()
{
	int i = 0;

//	MQTTclient.disconnect();

//	WiFi.mode(WIFI_OFF);
//	WiFi.disconnect(true, false);
}


bool Send_reading(Reading* r)
{
	if (WiFi.status() != WL_CONNECTED)
		if (! WiFi_Setup())
			return false;

	if (!MQTTclient.connected())
		if ( ! MQTT_Setup())
			return false;

	SendMQTT("KNet/Haven/Skur/Solar1_mA", r->Solar1_mA);   delay(10);
	SendMQTT("KNet/Haven/Skur/Solar1_V", r->Solar1_V);     delay(10);
	SendMQTT("KNet/Haven/Skur/Solar2_mA", r->Solar2_mA);   delay(10);
	SendMQTT("KNet/Haven/Skur/Solar2_V", r->Solar2_V);     delay(10);
	SendMQTT("KNet/Haven/Skur/Battery_mA", r->Load1_mA);   delay(10);
	SendMQTT("KNet/Haven/Skur/Battery_V", r->Battery_V);   delay(10);
	SendMQTT("KNet/Haven/Skur/Charger_mA", r->Charger_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Charger_V", r->Charger_V);   delay(10);
	SendMQTT("KNet/Haven/Skur/Load1_mA", r->Load1_mA);     delay(10);
	SendMQTT("KNet/Haven/Skur/Load2_mA", r->Load2_mA);     delay(10);
	SendMQTT("KNet/Haven/Skur/Load3_mA", r->Load3_mA);     delay(10);
	SendMQTT("KNet/Haven/Skur/Load4_mA", r->Load4_mA);
	
	SendMQTT("KNet/Haven/Vejr/Temperatur", r->Temp);      delay(10);
	SendMQTT("KNet/Haven/Vejr/Fugtighed", r->Humid);      delay(10);
	SendMQTT("KNet/Haven/Vejr/Lufttryk", r->Press);      delay(10);

	if (r->Vandstand_mm != 0) 
		SendMQTT("KNet/Haven/Regn/vandstand_mm", (int32_t)r->Vandstand_mm); delay(10);

	delay(1000);

	Maintanance_mode = GetStatusCode();
	if (!Maintanance_mode)
		WIFI_disconnect();

	return true;
}

void MQTT_Initial_setup()
{
	int i = 0;

	Serial.println("MQTT_Setup");

	if (WiFi.status() != WL_CONNECTED)
		WiFi_Setup();

//	String IP = WiFi.localIP().toString();

	MQTTclient.setServer(MQTTServer, 1883);
	MQTTclient.setSocketTimeout(120);
	MQTTclient.setKeepAlive(120);
	if (!MQTTclient.connected()) {
		Serial.println("Connect MQTT");
		MQTTclient.connect(clientId);
	}

	delay(250);

	MQTT_Setup();
}


bool MQTT_Setup()
{
	int i = 0;

	Serial.println("MQTT_Setup");

	if (!MQTTclient.connected()) 
	{
		Serial.println("Connect MQTT");
		MQTTclient.connect(clientId);

		return MQTTclient.connected();
	}
}


bool SendMQTT(const char* Topic, int32_t payload)
{
	bool ret;

	char s[20];
	itoa(payload, s, 10);
	ret = MQTTclient.publish(Topic, s, false);
	return (ret);
}

bool SendMQTT(const char* Topic, float payload)
{
	bool ret;

	char s[30];
	dtostrf(payload, 5, 2, s);
	ret = MQTTclient.publish(Topic, s, false);
	return (ret);
}

bool SendMQTT(const char* Topic, char *payload)
{
	bool ret;

	ret = MQTTclient.publish(Topic, payload, false);
	return (ret);
}


int GetStatusCode()
{
	httpClient.begin("http://192.168.1.21:8123/api/states/input_boolean.skur_debug");
	httpClient.setReuse(false);
	httpClient.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiIwMmEyNmYxZDViMDE0MWIxODhkNWMxZGM0NTk1ZjcxNCIsImlhdCI6MTYxNzM2NzY0MSwiZXhwIjoxOTMyNzI3NjQxfQ.iJ0YQy9E4U9Rwbs9EJMYl1-DIoBHCW6AAB0rL3mAsEw");
	httpClient.addHeader("Content-Type", "application/json");

	int httpCode = httpClient.GET();

	if (httpCode == -11)  // Try again.
	{
		httpCode = httpClient.GET();
	}


	if (httpCode == 200) { //Check for the returning code

		String payload = httpClient.getString();

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
	sleep(1000);
	ESP.restart();
}