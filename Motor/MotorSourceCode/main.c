/* DriverLib Includes */
#include <driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>



//Variables for counting of notches
uint32_t leftNotch;
uint32_t rightNotch;
int16_t leftCounter;
int16_t rightCounter;

int rightBaseSpeed = 5000;
int leftBaseSpeed = 5000;

//value to multiply for duty cycle
volatile static float pidLeft = 1;
volatile static float pidRight = 1;

//variables needed to calculate PID
volatile static int16_t prevErrorLeft = 0;
volatile static int16_t prevErrorRight = 0;
volatile static int16_t sumErrorLeft = 0;
volatile static int16_t sumErrorRight = 0;



#define TIMER_PERIOD 256

//Timer A to trigger 0.25s interrupt for PID
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,              // SMCLK Clock Source = 3Mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_32,         // TACLK = 3MHz / 64
        TIMER_PERIOD,                           // 46875 ticks (CCR0)
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};


//PWM configuration for both DC motor to be at 50% duty cycle
Timer_A_PWMConfig pwmConfigRight =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_3,
        10000,  // 10000/1000000 = 100hz
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        5000 // 5000/10000 = 50% Duty Cycle
};

Timer_A_PWMConfig pwmConfigLeft =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_3,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        5000
};



//Function for PID

void setPID(void)
{
    //Constant values for PID
    //Proportional - how much to adjust based on the error.
    //small error - small adjustments, large error - large adjustments
    static const float KP = 0.03;

    //Integral - how much to adjust based on sum of error.
    static const float KI = 0.006;

    //Derivative - how much to adjust based on previous error
    static const float KD = 0.015;

    //setting max and min PID values
    static const int pidMAX = 1;
    static const int pidMIN = 0;

    //target distance for every PID call.
    volatile static int16_t target = 8;

    //errors based on target distance and actual distance
    int16_t errorLeft = target - leftCounter;
    int16_t errorRight = target - rightCounter;

    //pid calculations based on current error, previous error and sum of error
    pidLeft += (errorLeft * KP) + (prevErrorLeft * KD) + (sumErrorLeft * KI);
    pidRight += (errorRight * KP) + (prevErrorRight * KD) + (sumErrorRight * KI);

    // Boundaries for PID left and right
    if(pidLeft >= pidMAX)
    {
        pidLeft = pidMAX;
    }
    else if(pidLeft <= pidMIN)
    {
        pidLeft = pidMIN;
    }
    else
    {
        pidLeft = pidLeft;
    }

    if(pidRight >= pidMAX)
    {
        pidRight = pidMAX;
    }
    else if(pidRight <= pidMIN)
    {
        pidRight = pidMIN;
    }
    else
    {
        pidRight = pidRight;
    }

    //duty cycle to set based on calculated PID values
    uint16_t leftDutyCyle = leftBaseSpeed * pidLeft;
    uint16_t rightDutyCycle = rightBaseSpeed * pidRight;

    //configuring the duty cycle
    pwmConfigLeft.dutyCycle = leftDutyCyle;
    pwmConfigRight.dutyCycle = rightDutyCycle;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigLeft);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigRight);

    //reset distance traveled
    leftCounter = 0;
    rightCounter = 0;

    //setting previous error values
    prevErrorLeft = errorLeft;
    prevErrorRight = errorRight;

    //storing sum of errors
    sumErrorLeft += errorLeft;
    sumErrorRight += errorRight;
}


void initialiseMotor(void)
{
    WDT_A_holdTimer();

    /* Configuring P4.1 and P4.2 as Output. P2.4 as peripheral output for PWM
    */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);


    /* Configuring P4.3 and P4.4 as Output. P2.5 as peripheral output for PWM
    */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);


    /* Configuring P5.4 and P5.5 as input to receive output from wheel encoder to increment the notches count
     */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN4);

    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);

    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN4);

    /* Enabling interrupts and starting the timer */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Interrupt_enableInterrupt(INT_TA1_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    /* Generate PWM for left and right motor*/
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigRight);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigLeft);

    //enable interrupt on port 5
    Interrupt_enableInterrupt(INT_PORT5);

    //enable all interrupts
    Interrupt_enableMaster();
}

//function to move car forward
void forwardCar(void)
{
    //left motor front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //right motor front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
}

//function to stop car
void stopCar(void)
{

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

}

//function to stop right wheel
void stopRight(void)
{

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

}

//function to stop left wheel
void stopLeft(void)
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
}

//function to turn car left by 45˚
void stationaryTurnLeft45(void)
{

    //right motor go infront
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor stop
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 4)
        {
            stopRight();
            rightNotch = 0;
            //notchesdetected2 = 0;
        }
        if(leftNotch == 5)
        {
            stopLeft();
            leftNotch = 0;
            //notchesdetected2 = 0;
        }
    }

}

//function to turn car left by 90˚
void stationaryTurnLeft90(void)
{

    //right motor go front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor go back
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 8)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 9)
        {
            stopLeft();
            leftNotch = 0;
        }
    }

}

//function to reverse turn car by 180˚
void reverseTurn180(void)
{

    //right motor go front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor back
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 18)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 20)
        {
            stopLeft();
            leftNotch = 0;
        }
    }

}

//function to turn car right by 45˚
void stationaryTurnRight45(void)
{

    //right motor back
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor front
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 4)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 5)
        {
            stopLeft();
            leftNotch = 0;
        }
    }

}

//function to turn car right by 90˚
void stationaryTurnRight90(void)
{

    //right motor back
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor front
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 8)
        {
            stopRight();
            rightNotch = 0;
        }
        if(leftNotch == 9)
        {
            stopLeft();
            leftNotch = 0;
        }
    }

}



//function to slow left wheel by 5%
void slowLeftWheel()
{
    pwmConfigLeft.dutyCycle = leftBaseSpeed * 0.90; //drop 5% duty cycle (500/10000)
    pwmConfigRight.dutyCycle = rightBaseSpeed;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigRight); // Generate a PWM with timer running in up mode for right motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigLeft); // Generate a PWM with timer running in up mode for left motor

}

//function to slow right wheel by 10%
void slowRightWheel()
{
    pwmConfigRight.dutyCycle = rightBaseSpeed * 0.90; //drop 5% duty cycle (500/10000)
    pwmConfigLeft.dutyCycle = leftBaseSpeed;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigRight); // Generate a PWM with timer running in up mode for right motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigLeft); // Generate a PWM with timer running in up mode for left motor

}

//function to adjust wheel speed
void adjustWheel(void)
{
    //Left wheel faster
    if (leftNotch > rightNotch)
    {
        slowLeftWheel();
    }
    //Right wheel faster
    else if (rightNotch > leftNotch)
    {
        slowRightWheel();
    }

}

//function to reverse car and rotate by 180˚
void reverseCar(void)
{
    //left motor back
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //right motor back
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    while(1)
    {
        if(rightNotch >=8 && leftNotch >=8)
         {
             stopRight();
             stopLeft();
             rightNotch = 0;
             leftNotch = 0;
             reverseTurn180();
         }
    }
}

//function to move right wheel by 2 notches
void slightTurnRight(void)
{
    //right motor stop
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);

    while(1)
    {
        if(leftNotch == 2)
        {
            stopLeft();
            leftNotch = 0;
        }
    }

}

//function to move left wheel by 2 notches
void slightTurnLeft(void)
{
    //right motor stop
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor front
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);

    while(1)
    {
        if(rightNotch == 2)
        {
            stopRight();
            rightNotch = 0;
        }
    }

}

//function to pivot turn left by 90˚
void pivotTurnLeft90(void)
{

    //right motor go infront
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor stop
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 16)
        {
            stopCar();
            rightNotch = 0;
        }
    }

}

//function to pivot turn right by 90˚
void pivotTurnRight90(void)
{

    //right motor stop
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor go forward
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(leftNotch == 17)
        {
            stopCar();
            leftNotch = 0;
        }
    }
}

//function to pivot turn left by 45˚
void pivotTurnLeft45(void)
{

    //right motor go front
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor stop
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(rightNotch == 8)
        {
            stopCar();
            rightNotch = 0;
        }
    }

}

//function to pivot turn right by 45˚
void pivotTurnRight45(void)
{

    //right motor stop
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    //left motor go backward
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

    while(1)
    {
        if(leftNotch == 8)
        {
            stopCar();
            leftNotch = 0;
        }
    }
}

int main(void)
{
    initialiseMotor();
    forwardCar();

    while (1)
    {
        PCM_gotoLPM3();
    }
}

//timer interrupt handler
void TA1_0_IRQHandler(void)
{
    setPID();
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
}


//port 5 interrupt handler
void PORT5_IRQHandler(void)
{

    uint32_t leftStatus = GPIO_getInterruptStatus(GPIO_PORT_P5, GPIO_PIN5); //Left Wheel
    uint32_t rightStatus = GPIO_getInterruptStatus(GPIO_PORT_P5, GPIO_PIN4); //Right Wheel

    if(leftStatus & GPIO_PIN5)
    {
        leftNotch++;
        leftCounter++;

    }
    if(rightStatus & GPIO_PIN4)
    {
        rightNotch++;
        rightCounter++;

    }

    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN5);

}



