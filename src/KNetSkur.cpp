#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ssl_client.h>
#include <HTTPClient.h>
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
#include <Adafruit_INA219.h>
#include "INA3221.h"
#include <PubSubClient.h>
#include <Ticker.h>
#include <Wire.h>
#include "HW.h"
#include "Reading.h"
#include "Network.h"
#include "Display.h"
#include "Watering.h"
#include <EEPROM.h>

Reading* reading;

#define WAKE_TIME 600 // 10 minute
#define SEND_READING_INTERVAL 45
#define LIGHT_DELAY_SECOND 600  // 10 min.
#define MAX_LOAD4_CHARGE_TIME 3600  // One hour

int Next_reading = 0;
unsigned long millisec = 0;

RTC_NOINIT_ATTR bool Maintanance_mode = false;


void Sec_Tick()
{
	if (LightCountDownSec > 0)
		LightCountDownSec--;

	if (SleepCountDownSec > 0)
		SleepCountDownSec--;

	if (ReadingCountDownSec > 0)
		ReadingCountDownSec--;

	if (Load4ChargeCountDownSec > 0)
		Load4ChargeCountDownSec--;
}


void CheckForError()
{
	String st;
	char s[EEPROM_SIZE+1];

	EEPROM.begin(EEPROM_SIZE);

	uint8_t Error = EEPROM.read(0);

	Serial.print("CheckForError # char : "); Serial.println(Error);
	
	if (Error != 0)   // We have a message in Flash
		if (Error < EEPROM_SIZE)  // Check for garbage in Flash
		{
			int i;
			for (i = 0; i<Error; i++)
				st += (char)EEPROM.read(i+1);
		}

Serial.print("Len = "); Serial.println(Error);
Serial.print("Txt = "); Serial.println(st);

	SendMQTT("KNet/Debug/KNet_Skur", (char *)st.c_str());
	EEPROM.write(0,0);  // Clear error;
	EEPROM.commit();
}


void Update_timers()
{
	unsigned long m = millis();
	Count_Sec = m / 1000ul;
}

void Update_display()
{
	Display_Solar(reading);
	Display_Battery(reading);
	Display_Load(reading);
	Display_Status(reading);
}

void Check_buttoms()
{
	if (Touch1)
	{
		SleepCountDownSec = WAKE_TIME;
		ControlBacklight(true);
		Touch1 = false;
	}

	if (SW1 && SW5)
	{
		ESP.restart();
	}

	if (Touch2)
	{
		LightCountDownSec = LIGHT_DELAY_SECOND;
		ControlBacklight(true);
		LIGHT1_ON;
		Touch2 = false;
	}
	if (Touch3)
	{
		Load4ChargeCountDownSec = MAX_LOAD4_CHARGE_TIME;
		SleepCountDownSec = MAX_LOAD4_CHARGE_TIME;
		ControlBacklight(true);
		RELAY_ON;
		Touch3 = false;
	}


	if (Touch5)
	{
		SleepCountDownSec = 0;
		Load4ChargeCountDownSec = 0;
		LightCountDownSec = 3; // Delay 10 to turn off light
		ControlBacklight(false);
		Touch5 = false;
	}
}


void setup()
{
	Serial.begin(115200);

	Wire1.begin(SDA, SCL);
	Wire.begin(SDA_INA33221, SCL_INA33221);

	Display_Setup();
	Display_Text("Starter BME ", 3);
	HW_setup();
	Display_Text("Starter WiFi", 3);
	WiFi_Setup();
	OTA_Setup();
	Display_Text("Starter MQTT", 3);
	MQTT_Initial_setup();

	CheckForError();

	reading = new Reading();

	reading->Get_WaterReading();
	reading->Get_power();
	reading->Get_weather();

	Send_reading(reading);

	//	Second_LightCountdown = 120;
	Display_Solar(reading);
	Display_Battery(reading);
	Display_Load(reading);

	ControlBacklight(0); // turn off backlight

	SleepCountDownSec =60;  // Start with 1 min
	LightCountDownSec = 5;  // Start with 1 min

	ControlBacklight(true);

	// Setup interrupt on Touch Pad 3 (GPIO15)
	touchAttachInterrupt(TOUCH1_Pin, TouchCallback1, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH2_Pin, TouchCallback2, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH3_Pin, TouchCallback3, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH4_Pin, TouchCallback4, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH5_Pin, TouchCallback5, TOUCH_TRESHOLD);

	//Configure Touchpad as wakeup source
	esp_sleep_enable_touchpad_wakeup();
}


void loop()
{
	unsigned long m = millis();

	Update_timers();

	if (Maintanance_mode)
		ArduinoOTA.handle();

	if (millisec < m)
	{
		do	{  // Make sure we don't skip a sec.
			millisec += 1000;
			Sec_Tick();
		}
		while (millisec < m);
		reading->Get_WaterReading();
		reading->Get_power();
		reading->Get_weather();

// Serial.printf("T: %.2f -P: %.2f -H: %.2f -L:%d\n", reading->Temp, reading->Press, reading->Humid, reading->Vandstand_mm);
		Update_display();
	}

	if (ReadingCountDownSec <= 0)
	{
		ReadingCountDownSec = SEND_READING_INTERVAL;
		Send_reading(reading);
	}

	if (LightCountDownSec <= 0)
	{
		LIGHT1_OFF;
	}

	if (Load4ChargeCountDownSec <= 0)
		RELAY_OFF;

	if (!Maintanance_mode && SleepCountDownSec <= 0 && LightCountDownSec <= 0 && Load4ChargeCountDownSec <= 0)   // Only sleep if lights off
	{
		ControlBacklight(false);
		Display_sleeping();
		rtc_gpio_hold_en(L1); rtc_gpio_hold_en(L2);	rtc_gpio_hold_en(L3); rtc_gpio_hold_en(RELAY);
		gpio_deep_sleep_hold_en();
		esp_sleep_enable_timer_wakeup(10UL * 1000000UL);
//		Serial.println("Going to sleep");
		esp_light_sleep_start();
		gpio_hold_dis(L1); gpio_hold_dis(L2); gpio_hold_dis(L3); gpio_hold_dis(RELAY);
	}
	
	Check_buttoms();
}

