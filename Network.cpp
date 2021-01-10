#include "Network.h"
#include "Reading.h"
#include "WIFI-Secret.h"
#include <ArduinoOTA.h>

char ssid[] = SSID_NAME;
char password[] = PASSWORD;

IPAddress ip(192, 168, 1, 216);
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);

WiFiClient ethClient;

IPAddress MQTTServer(192, 168, 1, 20);
PubSubClient MQTTclient(ethClient);



void WiFi_Setup()
{
	Serial.println("WIFI Setup");

	int i = 0;
	WiFi.mode(WIFI_STA);

	WiFi.config(ip, gw, mask);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
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
	Serial.println("WIFI disconnect");

	MQTTclient.disconnect();
	WiFi.mode(WIFI_OFF);
}

void Send_reading(Reading* r)
{
	SendMQTT("KNet/Haven/Skur_v2/Solar1_mA", r->Solar1_mA);
	SendMQTT("KNet/Haven/Skur_v2/Solar1_V", r->Solar1_V);
	SendMQTT("KNet/Haven/Skur_v2/Solar2_mA", r->Solar2_mA);
	SendMQTT("KNet/Haven/Skur_v2/Solar2_V", r->Solar2_V);
	SendMQTT("KNet/Haven/Skur_v2/Battery_mA", r->Battery_mA);
	SendMQTT("KNet/Haven/Skur_v2/Battery_V", r->Battery_V);
	SendMQTT("KNet/Haven/Skur_v2/Charger_mA", r->Charger_mA);
	SendMQTT("KNet/Haven/Skur_v2/Charger_V", r->Charger_V);
	SendMQTT("KNet/Haven/Skur_v2/Load1_mA", r->load1_mA);
	SendMQTT("KNet/Haven/Skur_v2/Load2_mA", r->load2_mA);
	SendMQTT("KNet/Haven/Skur_v2/Load3_mA", r->load3_mA);
	SendMQTT("KNet/Haven/Skur_v2/Load_V", r->Load_V);

	SendMQTT("KNet/Haven/Vejr_v2/Temperatur", r->Temp);
	SendMQTT("KNet/Haven/Vejr_v2/Fugtighed", r->Humid);
	SendMQTT("KNet/Haven/Vejr_v2/Lufttryk", r->Press);

	WIFI_disconnect();
}

void MQTT_Setup()
{
	int c = 0;

	Serial.println("MQTT_Setup");

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

		delay(100);
		Serial.println("ERROR");
		if (c++ >= 10) {
			Serial.println("Unable to connect to MQTT, ESP is restarting.");
			ESP.restart();
		}
	}
}


void SendMQTT(char* Topic, int32_t payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[20];
	itoa(payload, s, 10);

	MQTTclient.publish(Topic, s, false);
}

void SendMQTT(char* Topic, float payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[20];
	dtostrf(payload, 4, 1, s);

	MQTTclient.publish(Topic, s, false);
}

