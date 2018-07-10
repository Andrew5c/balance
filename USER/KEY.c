
/*
加入按键，实验阶段，按键按下时亮屏，点亮10秒后熄灭。这个板子上，得把J-LINK的线拔了才能用按键KEY2。
这个板子上key2 接在PA15，KEY4->PA14，KAY3->PA13
当中断发生时，处理器首先会完成中断现场的保护，然后跳转到中断向量表中查找中断处理函数的入口地址，
进而执行相应的中断处理函数.
*/

//暂时先用key2去皮，key3单价加，key4单价减

#include <stm32f10x.h>
#include "main.h"

#define KEY_ON		0
#define KEY_OFF	1


/*----------------------------------
**函数名称：key_init
**功能描述：按键引脚初始化及中断配置
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void Key_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;			//定义一个EXTI结构体变量

	//1、使能GPIO和外部中断必须是能APIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

	//2、GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //中断设置为浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//引脚拉高
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//3、清除中断标志，设置中断线路,将GPIO与中断映射一起
	EXTI_ClearITPendingBit(EXTI_Line14);
	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿出发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能中断线
	EXTI_Init(&EXTI_InitStructure);							//根据参数初始化中断寄存器
	
	//4、配置中断向量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组2
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//中断处理函数分配
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//中断占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//副优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);							   	//根据参数初始化中断寄存器

	//到此，中断所有的设置就配置完了，这是简单的按键的中断，其他的可能要复杂一点

}

/*
** 带延时消抖并检测按键松开的按键中断
*/
void EXTI15_10_IRQHandler(void)
{
	//KEY1
	if(EXTI_GetITStatus(EXTI_Line14) != RESET) //确保是否产生了EXTI Line中断
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == KEY_ON)
		{
			Delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == KEY_ON)
			{
				Restart_From_Low_Power();			 //停机唤醒后需要启动HSE	
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line14);    //清除中断标志位
	}
	/*
	//KEY2
	else if(EXTI_GetITStatus(EXTI_Line15) != RESET) //确保是否产生了EXTI Line中断
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == KEY_ON)
		{
			Delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == KEY_ON)
			{
				Beep_Warning(1,100);
				ESP8266_Link_WIFI();		//连接WiFi
				start_linking = !start_linking;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line15);    //清除中断标志位
	}
	*/
}

