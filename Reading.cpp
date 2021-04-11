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

	ina3221.begin(SDA_INA33221, SCL_INA33221);

	Solar1.setCalibration_32V_2A();
	Solar2.setCalibration_32V_2A();
	Charger.setCalibration_32V_2A();

	Serial.println("Starting BME");
	while ((!bme.begin(0x76, &Wire1)) && (c < 10)) {
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
//Adafruit_BME280::MODE_FORCED
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::SAMPLING_X1,
		Adafruit_BME280::FILTER_X4,
//		Adafruit_BME280::FILTER_OFF,
		Adafruit_BME280::STANDBY_MS_62_5);

	Serial.println("Completed Startup");
}

void Reading::Get_power()
{
	Solar1_V = Solar1.getBusVoltage_V();
	Solar1_mA = Solar1.getCurrent_mA();
	Solar2_V = Solar2.getBusVoltage_V();
	Solar2_mA = Solar2.getCurrent_mA();
	Charger_V = Charger.getBusVoltage_V();
	Charger_mA = Charger.getCurrent_mA();

	Load1_mA = ina3221.getCurrent_mA(1);
	Load2_mA = ina3221.getCurrent_mA(2);
	Load3_mA = ina3221.getCurrent_mA(3);

	if (Load4ChargeCountDownSec <= 0)  // Remove noise when relay is off.
		Load4_mA = 0;
	else
		Load4_mA = Get_Load4_mA();

	Battery_V = ina3221.getBusVoltage_V(1);
/*
	Serial.print("Solar1      = "); Serial.print(Solar1_mA);  Serial.print(" / "); Serial.println(Solar1_V);
	Serial.print("Solar2      = "); Serial.print(Solar2_mA);  Serial.print(" / "); Serial.println(Solar2_V);
	Serial.print("Battery     = "); Serial.print(Battery_mA); Serial.print(" / "); Serial.println(Battery_V);
	Serial.print("Charger     = "); Serial.print(Charger_mA); Serial.print(" / "); Serial.println(Charger_V);
	Serial.print("L1_mA1   = "); Serial.print(load1_mA);  Serial.print(" / "); Serial.println(Load_V);
	Serial.print("L2_mA2   = "); Serial.print(load2_mA);  Serial.print(" / "); Serial.println(Load_V);
	Serial.print("L3_mA2   = "); Serial.println(load3_mA);
	Serial.print("L4_mA2   = "); Serial.println(load4_mA);
*/
}


void Reading::Get_weather()
{
//	bme.read(Press, Temp, Humid, BME280::TempUnit_Celsius, BME280::PresUnit_hPa);
	Press = bme.readPressure() / 100.0F;
	Temp = bme.readTemperature();
	Humid = bme.readHumidity();
}

extern uint16_t millivolt[];
static int p = 0;

uint16_t Reading::Get_Load4_mA()
{
	int32_t r = 0;

	millivolt[p] = analogRead(SENCE);
	if (p++ >= SENCE_READINGS - 1)
		p = 0;

	for (int i = 0; i < SENCE_READINGS; i++)
		r += millivolt[i];

	r = (r / SENCE_READINGS) - 260;

	if (r < 0)
		r = 0;

	return r * 15;
}