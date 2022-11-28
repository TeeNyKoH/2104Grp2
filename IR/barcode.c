#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "decode.h"
#include "barcode.h"

int result, totalResult, interruptCounter, resultAverage = 0;
int j, thicknessSum, thicknessAverage, blackCounter, whiteCounter = 0;
int startTime, currentTime = 0;
int isBlack = 0;
int blackWhiteLimit = 1600;
int blackOffset = 1, whiteOffset = 2;

float weight = 0.2, EMWF, previousReading = 0.0,test = 0.0;

void resetAllVariables()
{
    result, totalResult, interruptCounter, resultAverage = 0;
    j, thicknessSum, thicknessAverage, blackCounter, whiteCounter = 0;
    startTime, currentTime = 0;
    isBlack = 0;
    blackOffset = 1, whiteOffset = 2;
    memset(barcodeOutput, 0, 3);
}

void get_barcode(int reading)
{
    if (reading > blackWhiteLimit && !isBlack)
    {
        currentTime = time_us_32() - startTime;
        startTime = time_us_32();
        white[whiteCounter] = currentTime;
        whiteCounter++;
        isBlack = 1;
    }
    else if (reading < blackWhiteLimit && isBlack)
    {
        currentTime = time_us_32() - startTime;
        startTime = time_us_32();
        black[blackCounter] = currentTime;
        blackCounter++;
        isBlack = 0;
    }

    // ready to decode once there is 6 blackbars counted
    if (blackCounter == 5 + blackOffset) // ignore first black bar for first 6 blackbars
    {
        thicknessAverage = 0;
        thicknessSum = 0;
        // calculate bars thickness to calculate average
        for (j = blackOffset; j < blackCounter; j++)
        {
            thicknessSum += black[j];
        }
        thicknessAverage = thicknessSum / 5;
        // append value accordingly
        // 0 = thin
        // 1 = thick
        for (j = blackOffset; j < blackCounter; j++)
        {
            if (black[j] < thicknessAverage)
            {
                strcat(bars, "0");
            }
            else
            {
                strcat(bars, "1");
            }
        }
        // reset value for white/spaces
        thicknessAverage = 0;
        thicknessSum = 0;
        for (j = whiteOffset; j < whiteCounter; j++)
        {
            thicknessSum += white[j];
        }
        thicknessAverage = thicknessSum / 4;

        for (j = whiteOffset; j < whiteCounter; j++)
        {
            if (white[j] < thicknessAverage)
            {
                strcat(spaces, "0");
            }
            else
            {
                strcat(spaces, "1");
            }
        }
        // send bars & spaces for decode
        tempOutput = decode_barcode(bars, spaces);
        strcat(barcodeOutput, tempOutput);

        if (strlen(barcodeOutput) == 3)
        {
            printf("barcode value = %s\n", barcodeOutput);
            resetAllVariables();
        }

        memset(bars, 0, 5);
        memset(spaces, 0, 4);
        blackCounter = 0;
        whiteCounter = 0;

        blackOffset = 0;
        whiteOffset = 1;
    }
}

void barcode_interrupt_moving_average()
{
    if (!adc_fifo_is_empty())
    {
        result = adc_fifo_get();
        totalResult += result;
        interruptCounter++;

        // printf("%d\n",result);

        if (interruptCounter == 100)
        {
            // start timer once first value is read
            if (startTime == 0)
            {
                startTime = time_us_32();
            }

            resultAverage = totalResult / 100;
            // printf("%d\n",resultAverage);
            get_barcode(resultAverage);

            // reset
            totalResult = 0;
            interruptCounter = 0;
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}

void barcode_interrupt_exponential_weighted_filter()
{
    if (!adc_fifo_is_empty())
    {
        // start timer once first value is read
        if (startTime == 0)
        {
            startTime = time_us_32();
        }

        result = adc_fifo_get();

        totalResult += result;
        interruptCounter++;

        if (interruptCounter >= 100)
        {
            resultAverage = totalResult / 100;
            EMWF = (1 - weight) * previousReading + weight * resultAverage;
            get_barcode(EMWF);
            previousReading = EMWF;

            // printf("%.2f\n", EMWF);
            totalResult = 0;
            interruptCounter = 0;
        }

        if (strlen(barcodeOutput) == 3)
        {
            printf("%s\n", barcodeOutput);
            resetAllVariables();
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}

void barcode_interrupt_raw()
{
    if (!adc_fifo_is_empty())
    {
        // start timer once first value is read
        if (startTime == 0)
        {
            startTime = time_us_32();
        }

        result = adc_fifo_get();

        totalResult += result;
        interruptCounter++;

        if (interruptCounter >= 100)
        {
            resultAverage = totalResult / 100;
            get_barcode(resultAverage);

            printf("%d\n", resultAverage);
            totalResult = 0;
            interruptCounter = 0;
        }

        if (strlen(barcodeOutput) == 3)
        {
            printf("%s\n", barcodeOutput);
            resetAllVariables();
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}