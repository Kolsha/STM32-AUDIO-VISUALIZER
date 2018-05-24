//#define TEST_AUDIO

#include <math.h>
#include "stm32f0xx.h" // Device header



#include "led_table.h"
#include "btn_utils.h"
#include "sys_tick.h"
#include "adc_control.h"
#include "fix_fft.h"



#ifdef TEST_AUDIO

		#include "audio_samples.h"
		#define AUDIO_BUFF_LEN 128
		#define Log2BuffLen 7

#else

		#define AUDIO_BUFF_LEN 256//1024
		#define Log2BuffLen 8//9

#endif


#define SAMPLING_RATE (14000000 / 256) // Hz
#define DELTA_FREQ (SAMPLING_RATE / AUDIO_BUFF_LEN) // Hz


uint32_t SquareRoot(uint32_t a_nInput)
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}






static const uint32_t freq_filter[TABLE_W] = {
    // Hz
    //1000, 2000, 4000, 6000, 8000, 10000, 15000, 21000
	1000, 2000, 3000, 6000, 10000, 12000, 14000, 16000
};

volatile DMAState_t dma_flag = DMAS_NONE;

volatile int16_t audio_buff[AUDIO_BUFF_LEN];
volatile int16_t imag_buff[AUDIO_BUFF_LEN];

uint32_t mag_at(uint32_t idx){
			uint32_t R = (uint32_t)(audio_buff[idx] * audio_buff[idx]);//
      uint32_t I = (uint32_t)(imag_buff[idx] * imag_buff[idx]);

      uint32_t magnitude = (R + I); // sqrt
			magnitude = SquareRoot(magnitude);
			return magnitude;
}



volatile uint8_t xs[TABLE_W] = {0};
volatile uint8_t draw_pos = 0;
volatile uint8_t timer_tick = 0;
void SysTick_Handler(void) {
	
  uint16_t data = (1 << draw_pos);
	for(int i = 0; i <= xs[draw_pos]; i++){
		data |= (1 << (8 + i));
	}
  setData(data);
	
	if(timer_tick > 20 && xs[draw_pos] > 0){
				xs[draw_pos]--;
				timer_tick = 0;
	}
	
  draw_pos = (draw_pos + 1) % TABLE_H;
	timer_tick++;
	
}




int main(void) {

  initLedTable();
  SysTickEnable(40);
  initBtn();
	
#ifndef TEST_AUDIO
  initADC((uint16_t*)audio_buff, AUDIO_BUFF_LEN, (DMAState_t*)&dma_flag);
	startADC();
#else
	dma_flag = DMAS_ALL_COMPLETE;
	
	uint16_t audio_sel = 0;
#endif
	

  

  uint64_t buff_average = 0;
	uint32_t x_val[TABLE_W] = {0};

  for (;;) {
		
    if(readBtn()){
			continue;
		}

#ifdef TEST_AUDIO
		int16_t *ptr = 0;
		if(audio_sel < 100){
			ptr = audio_1_khz;
		}
		else if(audio_sel < 200){
			ptr = audio_10_khz;
		}
		else if(audio_sel < 300){
			ptr = audio_15_khz;
		}else if(audio_sel < 400){
			ptr = audio_21_khz;
		}
		else if(audio_sel < 500){
			ptr = audio_1_10_khz;
		}
		else if(audio_sel < 1000){
			ptr = audio_diff;
		}
		
			
    for (int i = 0; i < AUDIO_BUFF_LEN; i++) {
      audio_buff[i] = ptr[i]; 
    }
		
		audio_sel = (audio_sel + 1) % 1000;
#else	
		
		if(dma_flag != DMAS_ALL_COMPLETE){
			continue;
		}

		stopADC();
    dma_flag = DMAS_NONE;
#endif

		
    for (int i = 0; i < AUDIO_BUFF_LEN; i++) {
      imag_buff[i] = 0;
      buff_average += audio_buff[i];
    }

    buff_average = (buff_average / (AUDIO_BUFF_LEN) );

    for (int i = 0; i < AUDIO_BUFF_LEN; i++) {
      audio_buff[i] -= buff_average;
    }

    fix_fft((int16_t *)audio_buff, (int16_t *)imag_buff, Log2BuffLen, 0);

    uint8_t x_pos = 0;
    uint32_t max_mgn = 0;
		uint8_t count = 0;
		uint32_t cur_freq = DELTA_FREQ;
		
    for (uint32_t idx = 1; idx < (AUDIO_BUFF_LEN / 2); idx++) {
			
      cur_freq += DELTA_FREQ;
 
			if (cur_freq < freq_filter[x_pos]) {
        continue;
      }
			
			uint32_t magnitude = mag_at(idx);
			magnitude = 3 * log10(magnitude);
			
      if (magnitude > max_mgn)
        max_mgn = magnitude;

      x_val[x_pos] = magnitude;

      x_pos = (x_pos + 1) % TABLE_W;
			count++;
			
			if(count >= TABLE_H){
				break;
			}
    }
		
    for (x_pos = 0; x_pos < TABLE_W; x_pos++) {
			
			if(x_val[x_pos] == 0){
				continue;
			}
			
			int32_t tmp = (x_val[x_pos] * TABLE_H) / max_mgn;

			if(tmp >= TABLE_H){
				xs[x_pos] = TABLE_H - 1;
			}
			else {
				xs[x_pos] = tmp;
			}
				
    }
		
#ifndef TEST_AUDIO
		startADC();
#endif

		
  }
	
	//not reachable
}
