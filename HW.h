
#ifndef _HW_h
#define _HW_h
#include "Arduino.h"

// HW
#define NUM_SW	5

#define TOUCH1_Pin 4
#define TOUCH2_Pin 15
#define TOUCH3_Pin 13
#define TOUCH4_Pin 12
#define TOUCH5_Pin 14


enum Event {
	EV_NONE,
	EV_S1_SHORT,
	EV_S1_LONG,
	EV_S2_SHORT,
	EV_S2_LONG,
	EV_S3_SHORT,
	EV_S3_LONG,
	EV_S4_SHORT,
	EV_S4_LONG,
	EV_S5_SHORT,
	EV_S5_LONG,
	EV_TIMEOUT,
};

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


extern volatile enum Event event;
extern volatile bool SecTick;

extern volatile uint16_t Count_10ms;
extern volatile uint16_t Count_Sec;
extern volatile int32_t Second_CountDown;
extern volatile int32_t Second_LightCountdown;


// Wire1
#define SDA 18
#define SCL 19

// Wire
#define SDA_INA33221 21
#define SCL_INA33221 22

#define TOUCH_TRESHOLD 25

#define SW1     (ReadTouch(TOUCH1_Pin) > TOUCH_TRESHOLD)
#define SW2     (ReadTouch(TOUCH2_Pin) > TOUCH_TRESHOLD)
#define SW3     (ReadTouch(TOUCH3_Pin) > TOUCH_TRESHOLD)
#define SW4     (ReadTouch(TOUCH4_Pin) > TOUCH_TRESHOLD)
#define SW5     (ReadTouch(TOUCH5_Pin) > TOUCH_TRESHOLD)
#define SW_CNTMAX 80


void HW_setup();
void ICACHE_RAM_ATTR TimerRoutine_10ms();
int ReadTouch(uint8_t TouchPad);
#endif
