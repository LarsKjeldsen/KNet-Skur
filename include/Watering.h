// Watering.h

#ifndef _WATERING_h
#define _WATERING_h

#define NUM_WATER_READINGS 3
#define WATERVARIANCE 20  // Max diviasion in %
// NewPing sonar = sonar = NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

extern unsigned int WaterLevel[NUM_WATER_READINGS];

//void Dist_setup();
unsigned int Dist_get_reading();
unsigned int Dist_single_reading();
int ReadByte(int timeout);

#endif

