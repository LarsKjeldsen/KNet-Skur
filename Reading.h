// Reading.h

#ifndef _READING_h
#define _READING_h

#include "Arduino.h"
#include <BME280I2C.h>
#include <BME280.h>
#include <Adafruit_INA219.h>


class Reading
{
public:
	int16_t Solar1_mA;
	int16_t Solar2_mA;
	int16_t Battery_mA;
	int16_t load_mA;
	float Solar1_V;
	float Solar2_V;
	float Battery_V;
	float load_V;

	void Get_weather();
	void Get_power();

	float Temp;
	float Humid;
	float Press;

private:
	BME280I2C bme;
	Adafruit_INA219 Solar1;
	Adafruit_INA219 Solar2;
	Adafruit_INA219 Batt;
	Adafruit_INA219 Load;

public: 
	Reading();
};
#endif

