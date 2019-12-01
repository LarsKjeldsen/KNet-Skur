#include "Display.h"
#include "HW.h"

extern volatile bool S1_LONG;
extern volatile bool S1_PRESSED;
extern volatile bool S2_LONG;
extern volatile bool S2_PRESSED;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


void Display_Setup()
{
	lcd.begin(20, 4);
	lcd.clear();
	lcd.on();
	lcd.write("Starter Skur...");
}

void Display_Text(char * text, int line)
{
	lcd.setCursor(0, line);
	lcd.write(text);
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
	lcd.printf("Bat  %5dmA  %sV", r->Battery_mA, dtostrf(r->Battery_V, 5, 2, s));
}

void Display_Load(Reading * r)
{
	lcd.setCursor(0, 2);
	lcd.printf("Load  %4dmA", r->load_mA);
}


void Display_Countdown(int32_t Second_CountDown)
{
	lcd.setCursor(0, 3);
	lcd.printf("%02d:%02d:%02d    Inv: ", (Second_CountDown / 3600) % 24, (Second_CountDown / 60) % 60, Second_CountDown % 60);
	if (Second_CountDown == 0) {
		lcd.print("OFF");
	}
	else {
		lcd.print("ON ");
	}

}

void Display_Weather(Reading * r)
{
	lcd.setCursor(0, 3);
	//	lcd.printf("%2d.%01dC %2d.%01d%% %4d.%01dPa", r->Temp / 100, abs(r->Temp % 10), r->Humid / 100, r->Humid % 10, r->Press / 100, r->Press % 10);
	//	lcd.printf("%6fC %6f %7fPa", r->Temp, r->Humid, r->Press);
}

void Display_buttoms(Event e, bool s1, bool s2)
{
	lcd.setCursor(16, 2);
	if (S1_LONG)    lcd.print('L'); else lcd.print(' ');
	if (S1_PRESSED) lcd.print('S'); else lcd.print(' ');
	if (S2_LONG)    lcd.print('L'); else lcd.print(' ');
	if (S2_PRESSED) lcd.print('S'); else lcd.print(' ');
}

void ControlBacklight(int32_t count)
{
	if (S2_PRESSED) {
		lcd.backlight();
		return;
	}
	if (count == 0)
		lcd.noBacklight();
	else
	{
		lcd.backlight();
		lcd.setCursor(16, 2);
		lcd.printf("%d:%02d", count / 60, count % 60);
	}
}