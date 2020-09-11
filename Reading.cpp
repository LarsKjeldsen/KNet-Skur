// 
// 
// 

#include "Reading.h"
#include "Display.h"




Reading::Reading()
{
	int c = 0;

	Solar1.begin(&Wire1);
	Solar2.begin(&Wire1);
	Solar3.begin(&Wire1);
	Batt.begin(&Wire1);

	ina3221.begin(SDA_INA33221, SCL_INA33221);

	Solar1.setCalibration_32V_2A();
	Solar2.setCalibration_32V_2A();
	Solar3.setCalibration_32V_2A();
	Batt.setCalibration_32V_2A();

	Serial.println("Starting BME");
	while ((!bme.begin(0x76)) && (c < 1)) {
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

	Serial.println("Completed Startup");
}

void Reading::Get_power()
{
	Solar1_V = Solar1.getBusVoltage_V();
	Solar1_mA = Solar1.getCurrent_mA();
	Solar2_V = Solar2.getBusVoltage_V();
	Solar2_mA = Solar2.getCurrent_mA();
	Solar3_V = Solar3.getBusVoltage_V();
	Solar3_mA = Solar3.getCurrent_mA();
	Battery_V = Batt.getBusVoltage_V();
	Battery_mA = Batt.getCurrent_mA();

	load1_mA = ina3221.getCurrent_mA(1);
	load2_mA = ina3221.getCurrent_mA(2);
	load3_mA = ina3221.getCurrent_mA(3);
	Load_V = ina3221.getBusVoltage_V(1);

	Serial.print("Solar1_V      = "); Serial.println(Solar1_V);
	Serial.print("current_mA1   = "); Serial.println(load1_mA);
	Serial.print("current_mA2   = "); Serial.println(load2_mA);
	Serial.print("current_mA3   = "); Serial.println(load3_mA); Serial.println();
}


void Reading::Get_weather()
{
//	bme.read(Press, Temp, Humid, BME280::TempUnit_Celsius, BME280::PresUnit_hPa);
	Press = bme.readPressure() / 100.0F;
	Temp = bme.readTemperature();
	Humid = bme.readHumidity();
}

