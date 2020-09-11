// Network.h

#ifndef _NETWORK_h
#define _NETWORK_h

#include "Reading.h"
#include "Arduino.h"
#include <PubSubClient.h>

void WiFi_Setup();
void MQTT_Setup();

void SendMQTT(char *Topic, char *payload);
void SendMQTT(char *Topic, int32_t payload);
void SendMQTT(char *Topic, float payload);

void Send_reading(Reading *);

#endif

