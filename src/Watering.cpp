#include "hw.h"
#include "Watering.h"
#include <SoftwareSerial.h>

SoftwareSerial dist_serial;


#define NUM_WATER_READINGS 3
int WaterPointer = 0;
unsigned int WaterLevel[NUM_WATER_READINGS];
unsigned int PreAvarage = 0;
#define WATERVARIANCE 20  // Max diviasion in %

void Dist_setup()
{
	for (int i = 0; i < NUM_WATER_READINGS; i++)
		WaterLevel[i] = 0;

	Serial2.begin(9600, SERIAL_8N1, GPIO_NUM_16, GPIO_NUM_17, false);
}

unsigned int Dist_get_reading()
{

	unsigned int Reading = Dist_single_reading();

//	Serial.print(Reading); Serial.print('|'); // Serial.print(WaterPointer); Serial.print('|');
	if (Reading != 0)
	{
		WaterPointer++;
		if (WaterPointer > NUM_WATER_READINGS - 1)
			WaterPointer = 0;

		WaterLevel[WaterPointer] = Reading;
		if (WaterLevel[NUM_WATER_READINGS - 1] == 0)  // Last reading is numm, so first run.
			for (int i = 0; i < NUM_WATER_READINGS; i++) {
				WaterLevel[i] = Reading; // Fill all reading with firtst reading.
				PreAvarage = Reading;
			}
	}

	unsigned int ava = 0;

	for (int i = 0; i < NUM_WATER_READINGS; i++) {
		ava += WaterLevel[i];
//		Serial.print(WaterLevel[i]); Serial.print('-');
	}

//	Serial.println();

	PreAvarage = ava / NUM_WATER_READINGS;
	return  PreAvarage;
}

unsigned int Dist_single_reading()
{
	unsigned int distance;
	int RetryCount = 20;
	unsigned char Data_H = 0, Data_L = 0, checksum = 0; // , Data_S = 0;

	static int TIMEOUT = 200;

	while (Serial2.available() > 0) 
		ReadByte(TIMEOUT);

	unsigned char c;
	while ((c = ReadByte(TIMEOUT) != 0Xff) && (RetryCount-- > 0))
			; // Wait for start header

	if (RetryCount == -1) // We did not receive start byte
		return 0;

	Data_H = ReadByte(TIMEOUT);
	Data_L = ReadByte(TIMEOUT);
	checksum = ReadByte(TIMEOUT);

Serial.print(Data_H, HEX); Serial.print(":");
Serial.print(Data_L, HEX); Serial.print(":");
Serial.print(checksum, HEX); Serial.print(": ");

	unsigned char sum;
	sum = (Data_H + Data_L + 0xff) & 0xFF;
	if (sum == checksum)
	{
		distance = (int)Data_H * 256;
		distance += (int)Data_L;
		
//		Serial.print("Distance = "); Serial.print(distance); Serial.println(" cm");

		if (distance > 30)
			return distance;
		else
		{
			Serial.println("Below the lower limit");
		}
	}
	return 0; // Error
}


int ReadByte(int timeout)
{
	unsigned long t = millis() + timeout;

	while (millis() < t)
	{
		//		Serial.print('.');

		if (Serial2.available()) {
			int c = Serial2.read();
//			Serial.print(c, HEX); Serial.print(',');
			return c;
		}
//		delay(20);
	}
	return -1;
}
