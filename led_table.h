#ifndef LED_TABLE
#define LED_TABLE

#include <stm32f0xx.h>

#define TABLE_W 8
#define TABLE_H 8

#define DELTA_X 1

void initLedTable(void);

void setPixel(int x, int y, uint8_t val);

uint8_t getPixel(int x, int y);
void loopLedTable(void);

void clearTable(void);

void setData(uint16_t data);

#endif


