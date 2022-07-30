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
bool Touch1 = false;
bool Touch2 = false;
bool Touch3 = false;
bool Touch4 = false;
bool Touch5 = false;

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
void ICACHE_RAM_ATTR TouchCallback1()
{
    Serial.println("Touch1");
    Touch1 = true;
}
void ICACHE_RAM_ATTR TouchCallback2()
{
    Serial.println("Touch2");
    Touch2 = true;
}
void ICACHE_RAM_ATTR TouchCallback3()
{
    Touch3 = true;
    Serial.println("Touch3");
}
void ICACHE_RAM_ATTR TouchCallback4()
{
    Touch4 = true;
    Serial.println("Touch4");
}
void ICACHE_RAM_ATTR TouchCallback5()
{
    Touch5 = true;
    Serial.println("Touch5");
}

