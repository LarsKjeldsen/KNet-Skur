// 
// 
// 

#include "Reading.h"
#include "Display.h"




Reading::Reading()
{
	int c = 0;
	Load.begin();
	Solar1.begin();
	Solar2.begin();
	Batt.begin();

	Load.setCalibration_32V_2A();
	Solar1.setCalibration_32V_1A();
	Solar2.setCalibration_32V_1A();
	Batt.setCalibration_32V_2A();

	Serial.println("Starting BME");
	while (!bme.begin(0x76) & c < 10) {
		Serial.println("Could not find BME280 sensor!");
		delay(1000);
		c++;
	}
	if (c < 10)
		Serial.println("BME280 sensor OK !");
	else
		Serial.println("BME280 ERROR starting device");

	bme.setSampling(
		Adafruit_BME280::MODE_NORMAL,
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::FILTER_X4,
		Adafruit_BME280::STANDBY_MS_62_5);

//		Adafruit_BME280::StandbyTime_50ms,
//		Adafruit_BME280::SpiEnable_False,
//		0x76 // I2C address. I2C specific.

	Serial.println("Completed Startup");
}

void Reading::Get_power()
{
	load_V = Load.getBusVoltage_V();
	load_mA = Load.getCurrent_mA();
	Solar1_V = Solar1.getBusVoltage_V();
	Solar1_mA = Solar1.getCurrent_mA();
	Solar2_V = Solar2.getBusVoltage_V();
	Solar2_mA = Solar2.getCurrent_mA();
	Battery_V = Batt.getBusVoltage_V();
	Battery_mA = Batt.getCurrent_mA();
}


void Reading::Get_weather()
{
//	bme.read(Press, Temp, Humid, BME280::TempUnit_Celsius, BME280::PresUnit_hPa);
	Press = bme.readPressure() / 100.0F;
	Temp = bme.readTemperature();
	Humid = bme.readHumidity();
}

