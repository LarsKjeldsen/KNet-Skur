#include "HW.h"
#include <Ticker.h>
#include "Display.h"

Ticker Ticker_10ms;
long Timer_counter = 0;

int32_t LightCountDownSec = 0;
int32_t ReadingCountDownSec = 0;
int32_t SleepCountDownSec = 0;
int32_t Load4ChargeCountDownSec = 0;
uint16_t millivolt[SENCE_READINGS];

int Count_Sec = 0;

int hit = 0;

void HW_setup()
{
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
    pinMode(L3, OUTPUT);
    pinMode(RELAY, OUTPUT);

    pinMode(SENCE, INPUT);
    LIGHT1_OFF;
    LIGHT2_OFF;
    RELAY_OFF;
}



// When Touch interrupt is triggeret
void ICACHE_RAM_ATTR TouchCallback()
{
}

