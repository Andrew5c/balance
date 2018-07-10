
#ifndef __TIME2_H
#define __TIME2_H


#include <stm32f10x.h>


extern volatile unsigned char time2_num;
extern volatile unsigned char start_linking;

void Time_2_Init(int16_t ntime);


#endif

