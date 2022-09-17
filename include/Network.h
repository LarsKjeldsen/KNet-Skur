// Network.h

#ifndef _NETWORK_h
#define _NETWORK_h

#include "Reading.h"
#include "Arduino.h"
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <cJSON.h>

#define EEPROM_SIZE 32


extern RTC_NOINIT_ATTR bool Maintanance_mode;

bool WiFi_Setup();
bool MQTT_Setup();
void OTA_Setup();
void MQTT_Initial_setup();

bool SendMQTT(const char *Topic, char *payload);
bool SendMQTT(const char *Topic, int32_t payload);
bool SendMQTT(const char *Topic, float payload);
void My_Esp_Restart(String ErrorText);

bool Send_reading(Reading *);
int GetStatusCode();

#endif

