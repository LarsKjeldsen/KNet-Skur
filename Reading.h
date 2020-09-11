// Reading.h

#ifndef _READING_h
#define _READING_h

#include "Arduino.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include <INA3221.h>
#include <Wire.h>


class Reading
{
public:
	int16_t Solar1_mA;
	int16_t Solar2_mA;
	int16_t Solar3_mA;
	int16_t Battery_mA;
	int16_t load1_mA;
	int16_t load2_mA;
	int16_t load3_mA;
	float Solar1_V;
	float Solar2_V;
	float Solar3_V;
	float Battery_V;
	float Load_V;

	void Get_weather();
	void Get_power();

	float Temp;
	float Humid;
	float Press;

private:
	Adafruit_BME280 bme;
	
	Adafruit_INA219 Solar1 {0x40};
	Adafruit_INA219 Solar2 {0x41};
	Adafruit_INA219 Solar3 {0x44};
	Adafruit_INA219 Batt   {0x45};

	INA3221 ina3221;

public: 
	Reading();
};
#endif

