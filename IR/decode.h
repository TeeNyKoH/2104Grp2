#ifndef decode_h
#define decode_h

#define NUMBER_OF_STRING 45
#define MAX_STRING_SIZE 5

extern char output[5];

extern char Arr_Characters[NUMBER_OF_STRING][MAX_STRING_SIZE];
extern char Arr_Spaces[NUMBER_OF_STRING][MAX_STRING_SIZE];
extern char Arr_Bars[NUMBER_OF_STRING][MAX_STRING_SIZE];

extern int counter;
extern int barcounter;
extern int spacecounter;

extern int value1;
extern int value2;
extern int value3;
extern int finalvalue;

int compare(char[], char[]);
int concat(int, int);
char* decode_barcode(char[], char[]);

#endif