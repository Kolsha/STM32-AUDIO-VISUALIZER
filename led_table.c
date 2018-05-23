#include <stm32f0xx.h>
#include "led_table.h"

#include "spi_control.h"




static struct LedTable_t
{
    volatile uint16_t cycles;
		volatile uint8_t x_pos;
	
		volatile uint8_t table[TABLE_W * TABLE_H];
		
		volatile uint16_t data;
} LedTable;


static void setLatchState(int on){
	GPIOA->BSRR = (on > 0) ? GPIO_BSRR_BS_8 : GPIO_BSRR_BR_8;
}




void handler(uint16_t data){	
	
	setLatchState(0);

	writeData2SPI(LedTable.data);
	
	setLatchState(1);
	
}

void clearTable(void){
	for(int i = 0; i < TABLE_H * TABLE_W; i++){
		LedTable.table[i] = 0;
	}
}



void initLedTable(void){
	
	
	LedTable.cycles = 0;
	LedTable.x_pos = 0;
	
	LedTable.data = 0;
	
	clearTable();
	
	
	initSPI();
	setSPIRxHandler(handler);
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //RCC_AHBENR_GPIOBEN | ;
	//latch
	GPIOA->MODER &= ~(GPIO_MODER_MODER8);
	GPIOA->MODER |= GPIO_MODER_MODER8_0;
	//latch
	

	
	writeData2SPI(LedTable.data);
}



void setPixel(int x, int y, uint8_t val){

	LedTable.table[x * TABLE_W + y] = val;
	//LedTable.data = (1 << (8 + x)) | (1 << y);
}

uint8_t getPixel(int x, int y){
	return LedTable.table[x * TABLE_W + y];
}

void setData(uint16_t data){
	LedTable.data = data;
}



void loopLedTable(void){
	/*setLatchState(0);
	while(isSPIBusy()){
		setLatchState(0);
	}
	*/
	volatile uint16_t data = 0;
	
	//LedTable.data = 0;
	
	// data = 
	for(int i = 0; i < DELTA_X; i++){
	
		
		
		for(int y = 0; y < TABLE_H; y++){
			if(LedTable.table[LedTable.x_pos * TABLE_W + y]){
				data |= (1 << (8 + LedTable.x_pos)) | (1 << y);
				
			}
		}
		
		LedTable.x_pos = (LedTable.x_pos + 1) % TABLE_W;
	}
	LedTable.data = data;
	
	//LedTable.x_pos = (LedTable.x_pos + 1) % TABLE_W; 
	LedTable.cycles++;
	//writeData2SPI(LedTable.data);
	//setLatchState(1);
	
}

