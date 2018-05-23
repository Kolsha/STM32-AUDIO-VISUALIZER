#include <stm32f0xx.h>
#include "adc_control.h"

static volatile DMAState_t *private_dma_flag = 0;
/*
void ADC1_COMP_IRQHandler(void){
	
	if((ADC1->ISR & ADC_ISR_EOC) != 0)
	{
     lastValue = ADC1->DR;
	}
	ADC1->ISR = 0;
	
}
*/


void DMA1_Channel1_IRQHandler(void){

	if(!private_dma_flag)
		return;
	
	if((DMA1->ISR & DMA_ISR_HTIF1)){
		*private_dma_flag = DMAS_HALF_COMPLETE;
	}
	else if((DMA1->ISR & DMA_ISR_TCIF1)){
		*private_dma_flag = DMAS_ALL_COMPLETE;
	} else{
		*private_dma_flag = DMAS_NONE;
	}
	
	DMA1->IFCR |= DMA_IFCR_CHTIF1 | DMA_IFCR_CTCIF1;
}


uint8_t initADC(uint16_t *write_buff, uint32_t buff_len, DMAState_t *dma_flag){
	
	if(!write_buff || buff_len < 1 || !dma_flag){
		return 0;
	}
	private_dma_flag = dma_flag;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER |= GPIO_MODER_MODER1_1;
	
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	
	
	

	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		ADC1->CR |= ADC_CR_ADDIS; 
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
	}
	
	ADC1->CHSELR = ADC_CHSELR_CHSEL1;
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; 
	
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
	

	ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
	
	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR)); 
	DMA1_Channel1->CMAR = (uint32_t)write_buff; 
	DMA1_Channel1->CNDTR = buff_len; 
	DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0
										 | DMA_CCR_TEIE | DMA_CCR_CIRC
										 | DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_HTIE; 
	DMA1_Channel1->CCR |= DMA_CCR_EN; 
	
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	
	//*/
	ADC1->CFGR1 |= ADC_CFGR1_CONT;
	
	ADC1->CR |= ADC_CR_ADCAL; 
	while ((ADC1->CR & ADC_CR_ADCAL) != 0)
	{
		/* time-out */
	}
	
	/*
	NVIC_EnableIRQ(ADC1_COMP_IRQn); 	
	ADC1->IER = ADC_IER_EOCIE; // Enable interrupts on EOC
	*/
	
	

	
	ADC1->CR |= ADC_CR_ADEN; 
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
	{
		/* time-out  */
	}
	
	return 1;
}


void startADC(void){
	ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversions */
}

void stopADC(void){
	ADC1->CR |= ADC_CR_ADSTP; /* (1) */
}

