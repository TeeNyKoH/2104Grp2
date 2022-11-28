#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"

char decodeCharacters[NUMBER_OF_STRING][MAX_STRING_SIZE] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "A", "B", "C", "D", "E",
    "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z", "-", ".", "_", "*", "$", "/", "+", "%"};

char decodeSpaces[NUMBER_OF_STRING][MAX_STRING_SIZE] = {"0100", "0010", "0001",
                                                      "1000"};

char decodeBars[NUMBER_OF_STRING][MAX_STRING_SIZE] = {
    "10001", "01001", "11000", "00101", "10100", "01100",
    "00011", "10010", "01010", "00110", "00000"};

char output[5];

int counter, barCounter, spaceCounter, value1, value2, value3, finalValue = 0;

// Comparing both the strings.
int compare(char a[], char b[])
{
    int flag = 0, i = 0;                 // integer variables declaration
    while (a[i] != '\0' && b[i] != '\0') // while loop
    {
        if (a[i] != b[i])
        {
            flag = 1;
            break;
        }
        i++;
    }
    if (flag == 0)
        return 0;
    else
        return 1;
}

// Function to concatenate two integers into one
int concat(int a, int b)
{

    char s1[20];
    char s2[20];

    // Convert both the integers to string
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);

    // Concatenate both strings
    strcat(s1, s2);

    // Convert the concatenated string to integer
    int c = atoi(s1);

    // return the formed integer
    return c;
}

char* decode_barcode(char bars[], char spaces[])
{
    // for (int i = 0; i < strlen(decodeBars); i++)
    for (int i = 0; i < 11; i++)
    {

        /* count number */
        counter++;

        /* compare string value, calling compare() function  */
        int c = compare(decodeBars[i], bars);
        if (c == 0)
        {
            // if string is same
            barCounter = counter;
        }
    }

    /* reset counter*/
    counter = 0;

    if (barCounter != 11)
    {
        // for (int i = 0; i < strlen(decodeSpaces); i++)
        for (int i = 0; i < 4; i++)
        {

            counter++;

            int c = compare(decodeSpaces[i],  spaces);
            if (c == 0)
            {
                spaceCounter = counter;
            }
        }

        /* reset counter*/
        counter = 0;

        // -------------

        for (int i = 0; i < 11; i++)
        {

            counter++;

            int c = compare(decodeBars[i], bars);
            if (c == 0)
            {
                barCounter = counter;
            }
        }

        /* reset counter*/
        counter = 0;

        // ----- working for calculations -----------------

        // math
        if (barCounter == 10)
        {
            value1 = barCounter * spaceCounter;
            finalValue = value1 - 1;
        }
        else
        {
            value1 = spaceCounter - 1;
            value2 = barCounter;
            value3 = concat(value1, value2);
            finalValue = value3 - 1;
        }
    }

    // get array item
    printf("Character Value = %s\n", decodeCharacters[finalValue]);
    return decodeCharacters[finalValue];
}
