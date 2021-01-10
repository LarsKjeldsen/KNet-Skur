#include "HW.h"
#include <Ticker.h>
#include "Display.h"

Ticker Ticker_10ms;
long Timer_counter = 0;

int32_t Second_CountDown;
int32_t Second_LightCountdown;
unsigned long ESP_Sleep_Timeout;

int hit = 0;

int Count_Sec = 0;


void HW_setup()
{
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
    pinMode(L3, OUTPUT);
}



// When Touch interrupt is triggeret
void ICACHE_RAM_ATTR TouchCallback()
{
}

