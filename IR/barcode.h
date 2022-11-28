#ifndef barcode_h
#define barcode_h

extern int result, totalResult, interruptCounter, resultAverage;
extern int j, thicknessSum, thicknessAverage, blackCounter, whiteCounter;
extern int startTime, currentTime;
extern int isBlack;
extern int blackWhiteLimit;
extern int blackOffset, whiteOffset;

extern float weight, EMWF, previousReading;

int black[7];
int white[6];

char bars[5];
char spaces[4];

char *tempOutput;
char *barcodeOutput[3];

void get_barcode(int);
void barcode_interrupt();
void barcode_interrupt_moving_average();
void barcode_interrupt_exponential_weighted_filter();
void barcode_interrupt_raw();

#endif