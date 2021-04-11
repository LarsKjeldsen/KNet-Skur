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
#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include "INA3221.h"
#include <PubSubClient.h>
#include <Ticker.h>
#include <Wire.h>
#include "HW.h"
#include "Reading.h"
#include "Network.h"
#include "Display.h"

Reading* reading;

#define WAKE_TIME 600 // 10 minute
#define SEND_READING_INTERVAL 60
#define LIGHT_DELAY_SECOND 600  // 10 min.
#define MAX_LOAD4_CHARGE_TIME 3600  // One hour

int Next_reading = 0;
int millisec = 0;

RTC_NOINIT_ATTR bool Maintanance_mode;

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
	Display_Text("Starter MQTT", 3);
	MQTT_Setup();
	Display_Text("Starter BME280", 3);

	reading = new Reading();

	reading->Get_weather();
	reading->Get_power();
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
	touchAttachInterrupt(TOUCH1_Pin, TouchCallback, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH2_Pin, TouchCallback, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH3_Pin, TouchCallback, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH4_Pin, TouchCallback, TOUCH_TRESHOLD);
	touchAttachInterrupt(TOUCH5_Pin, TouchCallback, TOUCH_TRESHOLD);

	//Configure Touchpad as wakeup source
	esp_sleep_enable_touchpad_wakeup();
}


void loop()
{
	ArduinoOTA.handle();

	unsigned long m = millis();
	Get_reading();
	Update_display();
	Update_timers();

	if (millisec < m)
	{
		do	{  // Make sure we don't skip a sec.
			millisec += 1000;
			Sec_Tick();
		}
		while (millisec < m);
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
		esp_light_sleep_start();
		gpio_hold_dis(L1); gpio_hold_dis(L2); gpio_hold_dis(L3); gpio_hold_dis(RELAY);
		wakeup_reason();
	}
	
	Check_buttoms();
}


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



void Update_timers()
{
	unsigned long m = millis();
	Count_Sec = m / 1000ul;
}

void Get_reading()
{
	reading->Get_power();
	reading->Get_weather();
}

void Update_display()
{
	Display_Solar(reading);
	Display_Battery(reading);
	Display_Load(reading);
	if (SW1)
		Display_Weather(reading);
	else
		Display_Status();
	

}

void Check_buttoms()
{
	if (SW1)
	{
		SleepCountDownSec = WAKE_TIME;
		ControlBacklight(true);
	}

	if (SW1 && SW5)
	{
		ESP.restart();
	}

	if (SW2)
	{
		LightCountDownSec = LIGHT_DELAY_SECOND;
		ControlBacklight(true);
		LIGHT1_ON;
	}
	if (SW3)
	{
		Load4ChargeCountDownSec = MAX_LOAD4_CHARGE_TIME;
		SleepCountDownSec = MAX_LOAD4_CHARGE_TIME;
		ControlBacklight(true);
		RELAY_ON;
	}


	if (SW5)
	{
		SleepCountDownSec = 0;
		Load4ChargeCountDownSec = 0;
		LightCountDownSec = 3; // Delay 10 to turn off light
		ControlBacklight(false);
	}
}



esp_sleep_wakeup_cause_t wakeup_reason()
{
	esp_sleep_wakeup_cause_t wakeup_reason;
	wakeup_reason = esp_sleep_get_wakeup_cause();

#ifdef _DEBUG
	switch (wakeup_reason)
	{
	case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
	case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
	case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
	case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad : ") + esp_sleep_get_touchpad_wakeup_status(); break;
	case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
	default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
	}
#endif
	return wakeup_reason;
}

