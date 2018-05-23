#ifndef SPI_CONTROL
#define SPI_CONTROL

#include <stm32f0xx.h>



typedef void (*rxHandler_t) (uint16_t data);
typedef void (*txHandler_t) (void);

void initSPI(void);


void setSPIRxHandler(rxHandler_t handler);
void setSPITxHandler(txHandler_t handler);

inline void writeData2SPI(uint16_t value);

uint8_t isSPIBusy(void);

uint8_t isTxEmpty(void);







#endif
