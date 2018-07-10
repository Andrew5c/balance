
#ifndef __DELAY_H
#define __DELAY_H

#include <stm32f10x.h>

void Delay_us(u32 time);
void Delay_ms(u32 time);
void Delay_us_nop(u32 nTimer);
void Delay_ms_nop(u32 nTimer);
	

#endif

