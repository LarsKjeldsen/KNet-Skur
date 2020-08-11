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

volatile bool S1_LONG;
volatile bool S1_PRESSED;
volatile bool S2_LONG;
volatile bool S2_PRESSED;

volatile enum Event event;
volatile bool SecTick;

volatile uint16_t Count_10ms;
volatile uint16_t Count_Sec = 59;
volatile int32_t Second_CountDown;
volatile int32_t Second_LightCountdown;

Ticker Ticker_10ms;
long Timer_counter = 0;


void HW_setup()
{
	pinMode(RELAY_LOAD, OUTPUT);
	pinMode(RELAY_LIGHT, OUTPUT);

	RELAY_LIGHT_OFF;
	RELAY_LOAD_OFF;

	pinMode(encoder0PinA, INPUT_PULLUP);
	pinMode(encoder0PinB, INPUT_PULLUP);

	attachInterrupt(encoder0PinA, doEncoder, CHANGE);
	attachInterrupt(encoder0PinB, doEncoder, CHANGE);

	pinMode(SW1_Pin, INPUT_PULLUP);
	pinMode(SW2_Pin, INPUT_PULLUP);

	Ticker_10ms.attach_ms(10, TimerRoutine_10ms);
}


void ICACHE_RAM_ATTR  doEncoder()
{
	int MSB = digitalRead(encoder0PinA); //MSB = most significant bit
	int LSB = digitalRead(encoder0PinB); //LSB = least significant bit

	int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
	int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

	if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
	{
		if (Second_CountDown < 600)
		{
			Second_CountDown += 30; Second_CountDown -= Second_CountDown % 30;
		}
		else
		{
			Second_CountDown += 120; Second_CountDown -= Second_CountDown % 120;
		}
	}
	if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
	{
		if (Second_CountDown < 600)
		{
			Second_CountDown -= 30; Second_CountDown -= Second_CountDown % 30;
		}
		else
		{
			Second_CountDown -= 120; Second_CountDown -= Second_CountDown % 120;
		}
	}

	if (Second_CountDown < 0)
		Second_CountDown = 0;
	if (Second_CountDown > 24 * 60 * 60)
		Second_CountDown = 24 * 60 * 60 - 1;

	lastEncoded = encoded; //store this value for next time
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

	if (event == EV_NONE) {
		event = ev;
	}
}

