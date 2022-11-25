#ifndef motor_h
#define motor_h

extern int trigPin;
extern int gpin0, gpin1, gpin2;

extern int leftNotch;
extern int rightNotch;
extern int leftCount;
extern int rightCount;

#define LEFT 'L'
#define RIGHT 'R'
#define FORWARD 'F'
#define REVERSE 'R'

extern int isCarForward;

void initializeMotor();
void forwardCar();
void stopCar();
void stopLeft();
void stopRight();
void turnLeft90();
void turnLeft180();
void turnRight90();
void reverseCar();
void slightTurnLeft();
void slightTurnRight();
void motorPID();
void turnLeft45();
void turnRihht45();
void adjustSpeed();
void slowLeft();
void slowRight();
#endif