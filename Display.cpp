#include "Display.h"
#include "HW.h"

LiquidCrystal_I2C lcd(0x3F, 40, 4);


void Display_Setup()
{
	lcd.init();
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
	lcd.setCursor(0, 0);
	lcd.printf("Solar %4dmA  %4dmA", r->Solar1_mA, r->Solar2_mA);
}

void Display_Battery(Reading * r)
{
	char s[20];
	lcd.setCursor(0, 1);
	lcd.printf("Bat  %5dmA  %sV", r->Charger_mA, dtostrf(r->Battery_V, 5, 2, s));
}

void Display_Load(Reading * r)
{
	lcd.setCursor(0, 2);
	lcd.printf("L %3d %3d %3d %3d", r->Load1_mA, r->Load2_mA, r->Load3_mA, r->Load4_mA);
}


void Display_sleeping()
{
	lcd.setCursor(0, 3);
	lcd.print("Sleeping            ");
}


void Display_Status()
{
	lcd.setCursor(0, 3);
	lcd.printf("%3lu, %3lu            ", SleepCountDownSec, LightCountDownSec);
}



void ControlBacklight(bool backlight)
{
	if (backlight)
		lcd.backlight();
	else
		lcd.noBacklight();
}

