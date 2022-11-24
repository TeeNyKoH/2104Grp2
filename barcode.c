#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "decode.h"
#include "barcode.h"

int result, totalResult, startTime, interruptCounter, avg, avgs, blackCounter, whiteCounter, interruptCounter, j, sum, currentTime, limitCheck, isBlack = 0;
int whiteLimit = 400, blackLimit = 3300;
int blackOffset = 1, whiteOffset = 2;

int set_limits(int reading)
{
    if (whiteLimit == blackLimit || 7 * whiteLimit > blackLimit)
    {
        if (reading >= blackLimit)
        {
            blackLimit = reading;
            // init whiteLimit
            if (whiteLimit == 0)
                whiteLimit = blackLimit;
        }

        if (reading <= whiteLimit)
            whiteLimit = reading;

        if (7 * whiteLimit < blackLimit)
        {
            return 1;
        }
    }

    return 0;
}

void get_barcode(int reading)
{
    if (reading > blackLimit && !isBlack)
    {
        currentTime = time_us_32() - startTime;
        startTime = time_us_32();
        // printf("white %d\n", time);
        white[whiteCounter] = currentTime;
        whiteCounter++;
        isBlack = 1;
    }
    else if (reading < whiteLimit && isBlack)
    {
        currentTime = time_us_32() - startTime;
        // printf("black %d\n", time);
        startTime = time_us_32();
        black[blackCounter] = currentTime;
        blackCounter++;
        isBlack = 0;
    }

    if (blackCounter == 5 + blackOffset)
    {
        // printf("%d\n", whiteCounter);
        avgs = 0;
        sum = 0;

        for (j = blackOffset; j < blackCounter; j++)
        {
            // printf("%d = %d\n", j, black[j]);
            sum += black[j];
        }
        avgs = sum / 5;
        // printf("Average = %d\n\n", avgs);

        for (j = blackOffset; j < blackCounter; j++)
        {
            if (black[j] < avgs)
            {
                strcat(bars, "0");
            }
            else
            {
                strcat(bars, "1");
            }
        }
        // printf("bars %s\n", bars);
        avgs = 0;
        sum = 0;
        for (j = whiteOffset; j < whiteCounter; j++)
        {
            // printf("%d = %d\n", j, black[j]);
            sum += white[j];
        }
        avgs = sum / 4;

        for (j = whiteOffset; j < whiteCounter; j++)
        {
            if (white[j] < avgs)
            {
                strcat(spaces, "0");
            }
            else
            {
                strcat(spaces, "1");
            }
        }
        // printf("bars %s\n", bars);
        // printf("spaces %s\n", spaces);
        tempOutput = decode_barcode(bars, spaces);
        // printf("tempoutput %s\n", tempOutput);
        strcat(barcodeOutput, tempOutput);
        // printf("%s\n", barcodeOutput);

        memset(bars, 0, 5);
        memset(spaces, 0, 4);
        blackCounter = 0;
        whiteCounter = 0;

        blackOffset = 0;
        whiteOffset = 1;
    }
}

void resetAllVariables()
{
    blackOffset = 1;
    whiteOffset = 2;
    result, totalResult, startTime, interruptCounter, avg, avgs, blackCounter, whiteCounter, interruptCounter, j, sum, currentTime, limitCheck, isBlack = 0;
    memset(barcodeOutput, 0, 3);
}

void barcode_interrupt()
{
    if (!adc_fifo_is_empty())
    {
        result = adc_fifo_get();
        totalResult += result;
        interruptCounter++;

        // printf("%d\n",result);

        if (interruptCounter == 8333)
        {
            avg = totalResult / 8333;
            // printf("%d\n",avg);

            if (startTime == 0)
            {
                startTime = time_us_32();
            }
            get_barcode(avg);
            // printf("black %d  white %d\n",blackLimit,whiteLimit);

            if (strlen(barcodeOutput) == 3)
            {
                printf("%s\n", barcodeOutput);
                resetAllVariables();
            }

            // reset
            totalResult = 0;
            interruptCounter = 0;
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}