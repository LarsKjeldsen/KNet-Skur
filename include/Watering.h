// Watering.h

#ifndef _WATERING_h
#define _WATERING_h

// NewPing sonar = sonar = NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void Dist_setup();
unsigned int Dist_get_reading();
unsigned int Dist_single_reading();
int ReadByte(int timeout);

#endif

