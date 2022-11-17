#ifndef barcode_h
#define barcode_h

extern int result, totalResult, startTime, interruptCounter, avg, avgs, blackCounter, whiteCounter, interruptCounter, j, sum;
extern int currentTime;
extern int whiteLimit, blackLimit;
extern int isBlack, limitCheck;

int black[7];
int white[6];

char bars[5];
char spaces[4];

char* tempOutput;
char* barcodeOutput[3];

extern int blackOffset, whiteOffset;

int set_limits(int);
void get_barcode(int);
void barcode_interrupt();

#endif