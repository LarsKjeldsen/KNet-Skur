// Network.h

#ifndef _NETWORK_h
#define _NETWORK_h

#include "Reading.h"
#include "Arduino.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>


void WiFi_Setup();
void MQTT_Setup();

void SendMQTT(char *Topic, char *payload);
void SendMQTT(char *Topic, int32_t payload);
void SendMQTT(char *Topic, float payload);

void Send_reading(Reading *);

#endif

