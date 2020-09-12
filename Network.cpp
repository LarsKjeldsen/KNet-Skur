#include "Network.h"
#include "Reading.h"
#include "WIFI-Secret.h"

char ssid[] = SSID_NAME;
char password[] = PASSWORD;

IPAddress ip(192, 168, 1, 212);
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);

WiFiClient ethClient;

IPAddress MQTTServer(192, 168, 1, 20);
PubSubClient MQTTclient(ethClient);



void WiFi_Setup()
{
	int i = 0;
	WiFi.mode(WIFI_STA);
	
	WiFi.config(ip, gw, mask);
	WiFi.begin(ssid, password);

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}

	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
		if (i++ >= 10)
			ESP.restart();
	}


	 ArduinoOTA.setHostname("KNetSkur");

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

void Send_reading(Reading *r)
{
	SendMQTT("KNet/Haven/Skur/Solar1_mA",  r->Solar1_mA);
	SendMQTT("KNet/Haven/Skur/Solar1_V",   r->Solar1_V);
	SendMQTT("KNet/Haven/Skur/Solar2_mA",  r->Solar2_mA);
	SendMQTT("KNet/Haven/Skur/Solar2_V",   r->Solar2_V);
	SendMQTT("KNet/Haven/Skur/Battery_mA", r->Battery_mA);
	SendMQTT("KNet/Haven/Skur/Battery_V",  r->Battery_V);
	SendMQTT("KNet/Haven/Skur/Load_mA",    r->load_mA);
	SendMQTT("KNet/Haven/Skur/Load_V",     r->load_V);

	SendMQTT("KNet/Haven/Vejr/Temperatur", r->Temp);
	SendMQTT("KNet/Haven/Vejr/Fugtighed",  r->Humid);
	SendMQTT("KNet/Haven/Vejr/Lufttryk",   r->Press);
}

void MQTT_Setup()
{
	int c = 0;

	if (WiFi.status() != WL_CONNECTED)
		WiFi_Setup();

	String IP = WiFi.localIP().toString();
	MQTTclient.setServer(MQTTServer, 1883);
	String clientId = "Skur-"+IP+"-";
	clientId += String(random(0xffff), HEX);

	while (!MQTTclient.connected())
	{
		Serial.print("Attempting MQTT connection... : ");
		// Attempt to connect
		MQTTclient.connect(clientId.c_str());

		delay(1000);
		Serial.println("ERROR");
		if (c++ >= 10) {
			Serial.println("Unable to connect to MQTT, ESP is restarting.");
			ESP.restart();
		}
	}
}


void SendMQTT(char *Topic, int32_t payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[20];
	itoa(payload, s, 10);

	MQTTclient.publish(Topic, s, true);
}

void SendMQTT(char *Topic, float payload)
{
	if (!MQTTclient.connected())
		MQTT_Setup();

	char s[20];
	dtostrf(payload, 4, 1, s);

	MQTTclient.publish(Topic, s, true);
}

