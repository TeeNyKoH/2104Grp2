#ifndef encoder_h
#define encoder_h

#define PI 3.14f

int wheelRotation1, wheelRotation2, wheelTime1, wheelTime2;
float wheelDistance1, wheelDistance2;
int done1, done2;
int startTime, stopTime1, stopTime2;
double totalSpeed, totalDistance, rotationDistance;

double getDistance();
double getSpeed();
// static void wheel_encoder(unsigned int,unsigned int);

#endif