
/*
**systick是一个24位向下计数定时器，自动重装载
*/
#ifndef __SYSTICK_H
#define __SYSTICK_H


#include <stm32f10x.h>

void delay_10us(__IO u32 nTime);
void delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);  //中断调用函数
void Timing_1ms(__IO u32 nTime);//定时1ms函数


#endif

