// Network.h

#ifndef _NETWORK_h
#define _NETWORK_h

#include "Reading.h"
//#include "Arduino.h"
#include <MQTT.h>
#include <HTTPClient.h>
#include <cJSON.h>

#define EEPROM_SIZE 32


extern bool Maintanance_mode;

bool WiFi_Setup(int ErrorCount = 200);
void WiFi_disconnect();
bool MQTT_Setup();
void OTA_Setup();
void MQTT_Loop();
bool SendMQTT(const char *Topic, char *payload);
bool SendMQTT(const char *Topic, int32_t payload);
bool SendMQTT(const char *Topic, float payload);
void My_Esp_Restart(String ErrorText);

bool Send_reading(Reading *, int ErrorCount = 200);
int GetStatusCode();

#endif

