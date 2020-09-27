#include <ESPmDNS.h>
#include <Update.h>
#include <HttpsOTAUpdate.h>
#include <ArduinoOTA.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_Sensor.h>
#include <AsyncUDP.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <WiFiSTA.h>
#include <WiFiServer.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ETH.h>
#include <SimpleBLE.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include "INA3221.h"
#include <PubSubClient.h>
#include <Ticker.h>
#include <Wire.h>
#include "Reading.h"
#include "Network.h"
#include "HW.h"
#include "Display.h"


int32_t Second_CountDown_Last;
Reading *reading;


#define LIGHT_DELAY_SECOND  600  // 10 min.

void setup()
{

	Serial.begin(115200);

	Wire1.begin(SDA, SCL);
	Wire.begin(SDA_INA33221, SCL_INA33221);

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

	pinMode(16, OUTPUT);
}

void loop()
{
	enum  Event ev;
	bool Tick;

	ev = event;
	Tick = SecTick;
	event = EV_NONE;
	SecTick = false;

	ArduinoOTA.handle();

	reading->Get_power();

	if (Second_CountDown == 0)
		;

	if (ev == EV_S1_SHORT)
		Second_LightCountdown = LIGHT_DELAY_SECOND;

	if (ev == EV_S1_LONG)
		Second_LightCountdown = 0;

	/*
	Serial.print(SW1); Serial.print(":"); Serial.print(SW2); Serial.print(":"); Serial.print(SW3); Serial.print(":"); Serial.print(SW4); Serial.print(":"); Serial.print(SW5); Serial.print("  -  ");
	Serial.print(S1_PRESSED); Serial.print(":"); Serial.print(S2_PRESSED); Serial.print(":"); Serial.print(S3_PRESSED); Serial.print(":"); Serial.print(S4_PRESSED); Serial.print(":"); Serial.print(S5_PRESSED); Serial.print("  -  ");
	Serial.print(S1_LONG); Serial.print(":"); Serial.print(S2_LONG); Serial.print(":"); Serial.print(S3_LONG); Serial.print(":"); Serial.print(S4_LONG); Serial.print(":"); Serial.println(S5_LONG);
	*/

	
	if (Tick)
	{
		Display_buttoms(ev, TOUCH1_Pin, TOUCH2_Pin);

		ControlBacklight(Second_LightCountdown);

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
