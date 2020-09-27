// 
// 
// 

#include "HW.h"
#include <Ticker.h>

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;

volatile uint8_t debounce[NUM_SW];      // switch debounce buffer
volatile uint8_t switchcount[NUM_SW];

Ticker Ticker_10ms;
long Timer_counter = 0;


void HW_setup()
{

	Ticker_10ms.attach_ms(10, TimerRoutine_10ms);
}


int ReadTouch(uint8_t TouchPad)
{
    return touchRead(TouchPad);
}

void ICACHE_RAM_ATTR  TimerRoutine_10ms()
{
	enum Event ev = EV_NONE;

	if (Count_10ms++ == 100)
	{
		Count_10ms = 0;
		Count_Sec++;
		SecTick = true;
		if (Second_CountDown > 0)
			Second_CountDown--;

		if (Second_LightCountdown > 0)
			Second_LightCountdown--;
	}


#define MONITOR_S(n) \
        { \
            uint8_t s = n - 1; \
            /* read switch positions into sliding 8-bit window */ \
            debounce[s] = (debounce[s] << 1) | SW ## n ;  \
            if (debounce[s] == 0) { \
                /* down for at least 8 ticks */ \
                S ## n ## _PRESSED = 1; \
                if (!S ## n ## _LONG) { \
                    switchcount[s]++; \
                } \
            } else { \
                /* released or bounced */ \
                if (S ## n ## _PRESSED) { \
                    if (!S ## n ## _LONG) { \
                        ev = EV_S ## n ## _SHORT; \
                    } \
                    S ## n ## _PRESSED = 0; \
                    S ## n ## _LONG = 0; \
                    switchcount[s] = 0; \
                } \
            } \
            if (switchcount[s] > SW_CNTMAX) { \
                S ## n ## _LONG = 1; \
                switchcount[s] = 0; \
                ev = EV_S ## n ## _LONG; \
            } \
        }

	MONITOR_S(1);
    MONITOR_S(2);
    MONITOR_S(3);
    MONITOR_S(4);
    MONITOR_S(5);

	if (event == EV_NONE) {
		event = ev;
	}
}

