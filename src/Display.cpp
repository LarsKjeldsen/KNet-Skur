#include "Display.h"
#include "HW.h"

#define BACKLIGHT_PIN 4

LiquidCrystal_I2C lcd(0x3F, 40, 4);

void Display_Setup()
{
	pinMode ( BACKLIGHT_PIN, OUTPUT );
	digitalWrite ( BACKLIGHT_PIN, HIGH );
  
  	lcd.begin(40,4);         
}

void Display_Text(const char * text, int line)
{
	lcd.setCursor(0, line);
	lcd.print(text);
}

void Display_Clear()
{
	lcd.clear();
}


void Display_Solar(Reading * r)
{
	#define DISPLAYLEN 20
	char d[DISPLAYLEN * 2];  //  Just to be sure
	lcd.setCursor(0, 0);

	sprintf(d, "Sol %dmA %d/%dV", r->Solar1_mA + r->Solar2_mA, (int)r->Solar1_V, (int)r->Solar2_V);
	for (int i=strlen(d); i<DISPLAYLEN; i++)
		d[i] = ' ';
	d[DISPLAYLEN] = '\0';
	lcd.print(d);
}

void Display_Battery(Reading * r)
{
	#define DISPLAYLEN 20
	char d[DISPLAYLEN * 2];  //  Just to be sure
	char s[20];
	lcd.setCursor(0, 1);
	int Charge = 0;

	if (r->Battery_V <= 11.31) Charge = 0;
	if (r->Battery_V > 11.31) Charge = 10;
	if (r->Battery_V > 11.58) Charge = 20;
	if (r->Battery_V > 11.75) Charge = 30;
	if (r->Battery_V > 11.90) Charge = 40;
	if (r->Battery_V > 12.06) Charge = 50;
	if (r->Battery_V > 12.20) Charge = 60;
	if (r->Battery_V > 12.32) Charge = 70;
	if (r->Battery_V > 12.42) Charge = 80;
	if (r->Battery_V > 12.50) Charge = 90;
	if (r->Battery_V > 12.70) Charge = 100;

	sprintf(d, "Bat %dmA %sV %d%%", r->Charger_mA, dtostrf(r->Battery_V, 3, 1, s), Charge);
	for (int i=strlen(d); i<DISPLAYLEN; i++)
		d[i] = ' ';
	d[DISPLAYLEN] = '\0';
	lcd.print(d);
}

void Display_Load(Reading * r)
{
	#define DISPLAYLEN 20
	char d[DISPLAYLEN * 2];  //  Just to be sure

	lcd.setCursor(0, 2);
	sprintf(d, "Use %d %d %d %d", r->Load1_mA, r->Load2_mA, r->Load3_mA, r->Load4_mA);
	for (int i=strlen(d); i<DISPLAYLEN; i++)
		d[i] = ' ';
	d[DISPLAYLEN] = '\0';
	lcd.print(d);
}

void Display_sleeping()
{
	lcd.setCursor(0, 3);
	lcd.print("Sleeping            ");
}


void Display_Status(Reading * r)
{
	#define DISPLAYLEN 20
	char d[DISPLAYLEN * 2];  //  Just to be sure

	lcd.setCursor(0, 3);
	switch (DisplayStatus)
	{
	case 0:
		sprintf(d, "Vejr : %dC/%dP/%d%%", (int)r->Temp, (int)r->Press, (int)r->Humid);
		break;

	case 1:
		sprintf(d, "%d %d %d %d", DisplayCountDownSec, ErrorCount, LightCountDownSec, Load4ChargeCountDownSec);
		break;
	
	case 2:
		sprintf(d, "Vejr : %dC/%dP/%d%%", DisplayCountDownSec, (int)r->Temp, (int)r->Press, (int)r->Humid);
		break;

	case 3:
		sprintf(d, "SC:%d EC:%d LC:%d L4:%d", DisplayCountDownSec, ErrorCount, LightCountDownSec, Load4ChargeCountDownSec);
		break;
	}

	for (int i=strlen(d); i<DISPLAYLEN; i++)
		d[i] = ' ';
	d[DISPLAYLEN] = '\0';
	lcd.print(d);
}


void ControlBacklight(bool backlight)
{
	if (backlight)
		lcd.backlight();
	else
		lcd.noBacklight();
}

