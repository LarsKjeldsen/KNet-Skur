#include <Ticker.h>
#include <Wire.h>
#include <BME280I2C.h>
#include <BME280.h>
#include "Reading.h"
#include "Network.h"
#include "HW.h"
#include "Display.h"
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

extern volatile int32_t Second_CountDown;
extern volatile int32_t Second_LightCountdown;
extern volatile uint16_t Count_Sec;

extern volatile enum Event event;
extern volatile bool SecTick;
extern volatile bool S1_LONG;
extern volatile bool S1_PRESSED;
extern volatile bool S2_LONG;
extern volatile bool S2_PRESSED;

int32_t Second_CountDown_Last;
Reading *reading;


#define LIGHT_DELAY_SECOND  600  // 10 min.

void setup()
{

	Serial.begin(115200);

	Wire.begin(SDA, SCL);
	Display_Setup();
	Display_Text("   Starter BME", 1);
	HW_setup();
	Display_Text("   Starter WiFi", 2);
	WiFi_Setup();
	Display_Text("   Starter MQTT", 3);
	MQTT_Setup();

	delay(1000);
	Display_Clear();

	reading = new Reading();

	reading->Get_weather();
	reading->Get_power();

	Send_reading(reading);
	Second_LightCountdown = 120;
}

void loop()
{
	ArduinoOTA.handle();

	enum  Event ev;
	bool Tick;

	ev = event;
	Tick = SecTick;
	event = EV_NONE;
	SecTick = false;

	if (Second_CountDown == 0)
		RELAY_LOAD_OFF;
	else
		RELAY_LOAD_ON;

	if (ev == EV_S1_SHORT)
		Second_LightCountdown = LIGHT_DELAY_SECOND;

	if (ev == EV_S1_LONG)
		Second_LightCountdown = 0;

	if (Second_LightCountdown)
		RELAY_LIGHT_ON;
	else
		RELAY_LIGHT_OFF;

	ControlBacklight(Second_LightCountdown);

	if (Tick)
	{
//		Display_buttoms(ev, SW1_Pin, SW2_Pin);

		reading->Get_power();
		reading->Get_weather();

		if (Count_Sec % 60 == 0) // Every min
			Send_reading(reading);

		Display_Solar(reading);
		Display_Battery(reading);
		Display_Load(reading);
		if (! Second_CountDown)
			Display_Weather(reading);
	}
	if (Second_CountDown_Last != Second_CountDown)
	{
		Display_Countdown(Second_CountDown);
		Second_CountDown_Last = Second_CountDown;
	}

	delay(100);
}
