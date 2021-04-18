#include "Network.h"
#include "Reading.h"
#include "WIFI-Secret.h"
#include <ArduinoOTA.h>

char ssid[] = SSID_NAME;
char password[] = PASSWORD;

IPAddress ip(192, 168, 1, 218);
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);

WiFiClient ethClient;

IPAddress MQTTServer(192, 168, 1, 21);
PubSubClient MQTTclient(ethClient);

HTTPClient httpClient;


void WiFi_Setup()
{
	Serial.println("WIFI Setup");

	int i = 0;
	WiFi.mode(WIFI_STA);

	WiFi.config(ip, gw, mask);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		Serial.print(WiFi.status());
		Serial.print(" ");
		if (i++ >= 50)
			ESP.restart();
	}

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




void WIFI_disconnect()
{
	int i = 0;
	Serial.println("WIFI disconnect");

	MQTTclient.disconnect();


	while (MQTTclient.state() != -1) {
		delay(100);
		Serial.print('D');
		MQTTclient.loop();
		if (i++ >= 20)
			ESP.restart();
	}
	
	delay(10);

//	WiFi.disconnect(true,  false);
	WiFi.mode(WIFI_OFF);
}

void Send_reading(Reading* r)
{
	SendMQTT("KNet/Haven/Skur/Solar1_mA", r->Solar1_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Solar1_V", r->Solar1_V); delay(10);
	SendMQTT("KNet/Haven/Skur/Solar2_mA", r->Solar2_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Solar2_V", r->Solar2_V); delay(10);
	SendMQTT("KNet/Haven/Skur/Battery_mA", r->Load1_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Battery_V", r->Battery_V); delay(10);
	SendMQTT("KNet/Haven/Skur/Charger_mA", r->Charger_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Charger_V", r->Charger_V); delay(10);
	SendMQTT("KNet/Haven/Skur/Load1_mA", r->Load1_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Load2_mA", r->Load2_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Load3_mA", r->Load3_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Load4_mA", r->Load4_mA); delay(10);
	SendMQTT("KNet/Haven/Skur/Load_V", r->Load_V); delay(10);

	SendMQTT("KNet/Haven/Vejr/Temperatur", r->Temp); delay(10);
	SendMQTT("KNet/Haven/Vejr/Fugtighed", r->Humid); delay(10);
	SendMQTT("KNet/Haven/Vejr/Lufttryk", r->Press);

	// What for all publish to finalyse
	int i = 0;
	while (i++ < 100) {
		delay(10);
		MQTTclient.loop();
	}

	MQTTclient.disconnect();

	Maintanance_mode = GetStatusCode();
	if (!Maintanance_mode)
		WIFI_disconnect();
}

void MQTT_Setup()
{
	int c = 0;

	Serial.print("MQTT_Setup");

	if (WiFi.status() != WL_CONNECTED)
		WiFi_Setup();

	String IP = WiFi.localIP().toString();

	MQTTclient.setServer(MQTTServer, 1883);
	MQTTclient.setSocketTimeout(120);
	MQTTclient.setKeepAlive(120);
	String clientId = "Skur_V2" + IP;

	MQTTclient.connect(clientId.c_str());

	while (!MQTTclient.connected())
	{
		Serial.print("Attempting MQTT connection... : ");
		// Attempt to connect
		MQTTclient.connect(clientId.c_str());

		delay(250);
		Serial.println("ERROR");
		if (c++ >= 10) {
			Serial.println("Unable to connect to MQTT, ESP is restarting.");
			ESP.restart();
		}
	}
	Serial.println("  Done");
}


void SendMQTT(const char* Topic, int32_t payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[20];
	itoa(payload, s, 10);
//	Serial.print("Sending : "); Serial.println(s);
	MQTTclient.publish(Topic, s, false);
}

void SendMQTT(const char* Topic, float payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[30];
	dtostrf(payload, 5, 2, s);
//	Serial.print("Sending : "); Serial.println(s);
	MQTTclient.publish(Topic, s, false);
}


int GetStatusCode()
{

	httpClient.begin("http://192.168.1.21:8123/api/states/input_boolean.skur_debug");
	httpClient.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiIwMmEyNmYxZDViMDE0MWIxODhkNWMxZGM0NTk1ZjcxNCIsImlhdCI6MTYxNzM2NzY0MSwiZXhwIjoxOTMyNzI3NjQxfQ.iJ0YQy9E4U9Rwbs9EJMYl1-DIoBHCW6AAB0rL3mAsEw");
	httpClient.addHeader("Content-Type", "application/json");

	int httpCode = httpClient.GET();

	if (httpCode == -11)  // Try again.
	{
		httpCode = httpClient.GET();
	}

	if (httpCode > 0) { //Check for the returning code

		String payload = httpClient.getString();
		cJSON* root = cJSON_Parse(payload.c_str());
		String besked_raw = cJSON_GetObjectItem(root, "state")->valuestring;
		Serial.println("Besked : " + besked_raw);
		if (besked_raw == NULL)
			return false;
		if (besked_raw == "on")
		{
//			Serial.println(("debug = " + besked_raw).c_str());
			return true;
		}
	}
	return false;
}