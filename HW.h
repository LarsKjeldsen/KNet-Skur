
#ifndef _HW_h
#define _HW_h

#include "Arduino.h"

// HW
#define NUM_SW	2

#define SW1_Pin D4 
#define SW2_Pin D3 

#define SCL D1
#define SDA D2

// Rotary switch
#define encoder0PinA   D6
#define encoder0PinB   D7

// Relays
#define RELAY_LOAD D0
#define RELAY_LIGHT D8

#define RELAY_LOAD_ON (digitalWrite(RELAY_LOAD, HIGH))
#define RELAY_LOAD_OFF (digitalWrite(RELAY_LOAD, LOW))
#define RELAY_LIGHT_ON (digitalWrite(RELAY_LIGHT, HIGH))
#define RELAY_LIGHT_OFF (digitalWrite(RELAY_LIGHT, LOW))


#define SW1     (digitalRead(SW1_Pin) == 1)
#define SW2     (digitalRead(SW2_Pin) == 1)
#define SW_CNTMAX 80


enum Event {
	EV_NONE,
	EV_S1_SHORT,
	EV_S1_LONG,
	EV_S2_SHORT,
	EV_S2_LONG,
	EV_S1S2_LONG,
	EV_S3_SHORT,
	EV_S3_LONG,
	EV_TIMEOUT,
};

void HW_setup();
void ICACHE_RAM_ATTR TimerRoutine_10ms();
void ICACHE_RAM_ATTR doEncoder();

#endif

