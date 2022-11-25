#ifndef motor_h
#define motor_h

extern int trigPin;
extern int gpin0, gpin1, gpin2;

extern int leftNotch;
extern int rightNotch;
extern int leftCount;
extern int rightCount;

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

#endif