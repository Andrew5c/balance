
#include "main.h"


void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1、使能GPIO和外部中断必须是能APIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//2、GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}


/*---------------------------------
** 名称：Beep_Warning
** 功能：蜂鸣器提示音函数
** 入口：nbeep：需要响几声
		   delay：声音之间的延时
** 出口：无
** 说明：
延时的典型值：
100ms的时候响声比较急促，可用来报警
300ms的时候比较慢，可用来提示
----------------------------------*/
void Beep_Warning(unsigned char nbeep, unsigned int delay)
{
	int i;
	
	for(i=0; i < nbeep; i++)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		
		Delay_ms(delay);	
		
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		
		Delay_ms(delay);	
		
	}
}

