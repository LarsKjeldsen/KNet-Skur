// 
// 
// 

#include "Reading.h"
#include "Display.h"




Reading::Reading()
{
	int c = 0;

	Load.begin(0x40);
	Solar1.begin(0x44);
	Solar2.begin(0x45);
	Batt.begin(0x41);

	Load.setCalibration_32V_2A();
	Solar1.setCalibration_32V_1A();
	Solar2.setCalibration_32V_1A();
	Batt.setCalibration_32V_2A();

	Serial.println("Starting BME");
	while (!bme.begin() & c < 10) {
		Serial.println("Could not find BME280 sensor!");
		delay(1000);
		c++;
	}
	if (c < 10)
		Serial.println("BMP280 sensor OK !");
	else
		Serial.println("BMP280 ERROR starting device");

	BME280I2C::Settings settings(
		BME280::OSR_X1,
		BME280::OSR_X1,
		BME280::OSR_X1,
		BME280::Mode_Normal,
		BME280::StandbyTime_50ms,
		BME280::Filter_4,
		BME280::SpiEnable_False,
		0x76 // I2C address. I2C specific.
	);

	bme.setSettings(settings);
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
	bme.read(Press, Temp, Humid, BME280::TempUnit_Celsius, BME280::PresUnit_hPa);
//	bme.read(Press, Temp, Humid);
}

