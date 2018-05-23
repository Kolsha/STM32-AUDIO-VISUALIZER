#include <stm32f0xx.h>                  // Device header

#include "sys_tick.h"



void SysTickEnable(uint32_t mic){
	
	SystemCoreClockUpdate();
	SysTick->LOAD = ((SystemCoreClock / 50000) * mic) - 1;
	SysTick->VAL = SysTick->LOAD;
	
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk
								| SysTick_CTRL_TICKINT_Msk;
	//SysTick_CTRL_TICKINT_Msk -- interrupt
	//
}



uint8_t isSysTicked(void){
	if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == SysTick_CTRL_COUNTFLAG_Msk){
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
		return 1;
	}	
	return 0;
}

void SysTickDisable(void){
	SysTick->LOAD = 0;
}

