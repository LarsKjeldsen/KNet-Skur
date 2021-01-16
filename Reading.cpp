// 
// 
// 

#include "Reading.h"
#include "Display.h"
#include "HW.h"




Reading::Reading()
{
	int c = 0;

	Solar1.begin(&Wire1);
	Solar2.begin(&Wire1);
	Charger.begin(&Wire1);
//	Batt.begin(&Wire1);

	ina3221.begin(SDA_INA33221, SCL_INA33221);

	Solar1.setCalibration_32V_2A();
	Solar2.setCalibration_32V_2A();
	Charger.setCalibration_32V_2A();
//	Batt.setCalibration_32V_2A();

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
//	Battery_V = Batt.getBusVoltage_V();
//	Battery_mA = -Batt.getCurrent_mA();
	Charger_V = Charger.getBusVoltage_V();
	Charger_mA = Charger.getCurrent_mA();

	Battery_mA =  ina3221.getCurrent_mA(3);
	load2_mA =    ina3221.getCurrent_mA(2);
	load1_mA =    ina3221.getCurrent_mA(1);
	load1_mA = 0;
	Load_V =      ina3221.getBusVoltage_V(1);
	Battery_V = Load_V;
/*
	Serial.print("Solar1      = "); Serial.print(Solar1_mA);  Serial.print(" / "); Serial.println(Solar1_V);
	Serial.print("Solar2      = "); Serial.print(Solar2_mA);  Serial.print(" / "); Serial.println(Solar2_V);
	Serial.print("Battery     = "); Serial.print(Battery_mA); Serial.print(" / "); Serial.println(Battery_V);
	Serial.print("Charger     = "); Serial.print(Charger_mA); Serial.print(" / "); Serial.println(Charger_V);
	Serial.print("L1_mA1   = "); Serial.print(load1_mA);  Serial.print(" / "); Serial.println(Load_V);
	Serial.print("L2_mA2   = "); Serial.print(load2_mA);  Serial.print(" / "); Serial.println(Load_V);
	Serial.print("L3_mA3   = "); Serial.print(load3_mA);  Serial.print(" / "); Serial.println(Load_V);
*/
}


void Reading::Get_weather()
{
//	bme.read(Press, Temp, Humid, BME280::TempUnit_Celsius, BME280::PresUnit_hPa);
	Press = bme.readPressure() / 100.0F;
	Temp = bme.readTemperature();
	Humid = bme.readHumidity();
}

