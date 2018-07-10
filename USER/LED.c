
#include "main.h"


//LED高电平点亮
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1、使能GPIO和外部中断必须是能APIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//2、GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13);
}

//LED闪烁3下表示程序开始运行
void LED_Start(void)
{
	unsigned char i = 0;
	
	for(i=0; i < 3; i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13);		
		Delay_ms(100);			
		GPIO_ResetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13);			
		Delay_ms(100);			
	}
}


void LED1_Twinkling(unsigned char num)
{
	unsigned char i = 0;
	
	for(i = 0; i<num; i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_12);	
		Delay_ms(100);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);	
		Delay_ms(100);	

	}
}


void LED2_Twinkling(unsigned char num)
{
	unsigned char i = 0;
	
	for(i = 0; i<num; i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_13);	
		Delay_ms(100);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);	
		Delay_ms(100);	

	}
}



