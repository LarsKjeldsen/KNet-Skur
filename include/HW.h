
#ifndef _HW_h
#define _HW_h
#include "Arduino.h"
#include "driver/rtc_io.h"
#include <Ticker.h>
#include "Display.h"

extern int32_t LightCountDownSec;
extern int32_t ReadingCountDownSec;
extern int32_t SleepCountDownSec;
extern int32_t Load4ChargeCountDownSec;

extern bool Touch1;
extern bool Touch2;
extern bool Touch3;
extern bool Touch4;
extern bool Touch5;

extern int Count_Sec;

#define SENCE_READINGS 10

// HW
#define TOUCH1_Pin GPIO_NUM_14
#define TOUCH2_Pin GPIO_NUM_12
#define TOUCH3_Pin GPIO_NUM_13
#define TOUCH4_Pin GPIO_NUM_15
#define TOUCH5_Pin GPIO_NUM_4

#define L1 GPIO_NUM_32
#define L2 GPIO_NUM_33
#define L3 GPIO_NUM_25

#define RELAY GPIO_NUM_26
#define SENCE GPIO_NUM_34

// Wire1
#define SDA 18
#define SCL 19

// Wire
#define SDA_INA33221 21 // & Display
#define SCL_INA33221 22 // & Display

#define TOUCH_TRESHOLD 25


#define SW1     (touchRead(TOUCH1_Pin) < TOUCH_TRESHOLD)
#define SW2     (touchRead(TOUCH2_Pin) < TOUCH_TRESHOLD)
#define SW3     (touchRead(TOUCH3_Pin) < TOUCH_TRESHOLD)
#define SW4     (touchRead(TOUCH4_Pin) < TOUCH_TRESHOLD)
#define SW5     (touchRead(TOUCH5_Pin) < TOUCH_TRESHOLD)


#define LIGHT1_OFF digitalWrite(L1, HIGH)
#define LIGHT1_ON  digitalWrite(L1, LOW)
#define LIGHT2_OFF digitalWrite(L3, HIGH)
#define LIGHT2_ON  digitalWrite(L3, LOW)
#define RELAY_OFF  digitalWrite(RELAY, LOW)
#define RELAY_ON   digitalWrite(RELAY, HIGH)


void HW_setup();
void ICACHE_RAM_ATTR TouchCallback1();
void ICACHE_RAM_ATTR TouchCallback2();
void ICACHE_RAM_ATTR TouchCallback3();
void ICACHE_RAM_ATTR TouchCallback4();
void ICACHE_RAM_ATTR TouchCallback5();
int ReadTouch(uint8_t TouchPad);
#endif
