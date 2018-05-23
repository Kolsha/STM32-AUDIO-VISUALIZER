#include "spi_control.h"


static rxHandler_t rxHandler = 0; //void (*rxHandler)(uint16_t data) = 0;
static txHandler_t txHandler = 0; //void (*txHandler)() = 0;


// SPI2 Interrupt Handler 
void SPI2_IRQHandler(void)
{
	
	if ((SPI2->SR & SPI_SR_TXE) && txHandler != 0) //
	{
		//SPI2->SR &= ~SPI_SR_TXE;
		txHandler();
	}
	
	if (SPI2->SR & SPI_SR_RXNE)
	{
		//SPI2->SR &= ~SPI_SR_RXNE;
		volatile uint16_t data = SPI2->DR; // Reset RXNE flag
		if (rxHandler != 0){
			rxHandler(data);
		}
        
	}
    
	
}
// SPI2 Interrupt Handler 




void initSPI(void){
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // enable clock for GPIOB
	
	//clock
	GPIOB->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER15);	
	GPIOB->MODER |=  GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;
	
	GPIOB->AFR[1] |= (0 << 4 *(13 - 8)) | (0 << 4 *(15 - 8));
	//clock
	

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // enable SPI2 
	
	SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA |
							SPI_CR1_BR  |
							SPI_CR1_SSM |
							SPI_CR1_SSI ; /* | SPI_CR1_LSBFIRST */  
							
	
	
	SPI2->CR2 = SPI_CR2_DS; // 16 bit data
	
	SPI2->CR1 |= SPI_CR1_SPE;
}



void writeData2SPI(uint16_t value)
{
    SPI2->DR = value;
}

uint8_t isSPIBusy(void)
{
	return ((SPI2->SR  & SPI_SR_BSY) == 0) ? 0 : 1;
}

uint8_t isTxEmpty(void){
	return ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE);
}






void setSPIRxHandler(rxHandler_t handler)
{
	rxHandler = handler;
	if(handler != 0){
		NVIC_EnableIRQ (SPI2_IRQn);
    SPI2->CR2 |= SPI_CR2_RXNEIE;
	}
	else{
		SPI2->CR2 &= ~SPI_CR2_RXNEIE;
	}
}

void setSPITxHandler(txHandler_t handler)
{
	txHandler = handler;
	if(handler != 0){
		NVIC_EnableIRQ(SPI2_IRQn);
		SPI2->CR2 |= SPI_CR2_TXEIE;
	}
	else{
		SPI2->CR2 &= ~SPI_CR2_TXEIE;
	}
}
