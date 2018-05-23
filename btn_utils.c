#include <stm32f0xx.h>
#include "btn_utils.h"



void initBtn(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
}

uint8_t readBtn(void){
	
	return ((GPIOA->IDR & GPIO_IDR_0) == 0) ? 0 : 1;
}


