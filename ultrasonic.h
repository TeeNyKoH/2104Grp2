#ifndef ultrasonic_h
#define ultrasonic_h
extern float ultrasonic_Front, ultrasonic_Avg_Front, ultrasonic_Avg_Left, ultrasonic_Avg_Right, ultrasonic_Left, ultrasonic_Right; 
int left_block, front_block, right_block, centerCar;
void setupUltrasonicPins(int trigPin, int echoPin);
int getPulse(int trigPin, int echoPin);
float getCm(int trigPin, int echoPin);
float getUSDectection(int trigPin, int echoPin1, int echoPin2, int echoPin3);
#endif