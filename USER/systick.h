
/*
**systick��һ��24λ���¼�����ʱ�����Զ���װ��
*/
#ifndef __SYSTICK_H
#define __SYSTICK_H


#include <stm32f10x.h>

void delay_10us(__IO u32 nTime);
void delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);  //�жϵ��ú���
void Timing_1ms(__IO u32 nTime);//��ʱ1ms����


#endif

