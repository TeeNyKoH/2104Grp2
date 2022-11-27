#ifndef encoder_h
#define encoder_h
#include "hardware/timer.h"

#define PI 3.14f

int wheelRotation1, wheelRotation2, wheelTime1, wheelTime2;
float wheelDistance1, wheelDistance2;
int done1, done2;
int startTime, stopTime1, stopTime2, currentTime;
double avgSpeed, totalDistance, tempDistance, currentDistance;

double get_total_distance();
double get_average_speed();
static void wheel_encoder(unsigned int,long unsigned int);
void init_encoder();
#endif