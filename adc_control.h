#ifndef ADC_CONTROL
#define ADC_CONTROL

#include "stm32f0xx.h"                  // Device header


typedef enum {
    DMAS_NONE = 0,
    DMAS_HALF_COMPLETE,
		DMAS_ALL_COMPLETE
} DMAState_t;


uint8_t initADC(uint16_t *write_buff, uint32_t buff_len, DMAState_t *dma_flag);

void startADC(void);

void stopADC(void);





#endif

