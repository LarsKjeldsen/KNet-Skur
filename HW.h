
#ifndef _HW_h
#define _HW_h

#include "Arduino.h"

// HW
#define NUM_SW	2

#define SW1_Pin 12 
#define SW2_Pin 12 

#define SDA 18
#define SCL 19

#define SDA_INA33221 22
#define SCL_INA33221 23


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

#endif

