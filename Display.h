// Display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#include "Arduino.h"
#include "HW.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Reading.h"

extern volatile bool S1_LONG;
extern volatile bool S1_PRESSED;
extern volatile bool S2_LONG;
extern volatile bool S2_PRESSED;
extern volatile bool S3_LONG;
extern volatile bool S3_PRESSED;
extern volatile bool S4_LONG;
extern volatile bool S4_PRESSED;
extern volatile bool S5_LONG;
extern volatile bool S5_PRESSED;


void Display_Setup();

void Display_Solar(Reading *);
void Display_Battery(Reading *);
void Display_Load(Reading *);
void Display_Countdown(int32_t);
void Display_Weather(Reading *);
void Display_buttoms(Event, bool, bool);
void ControlBacklight(int32_t);
void Display_Text(char *, int);
void Display_Clear();

#endif

