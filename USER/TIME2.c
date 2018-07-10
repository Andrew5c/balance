
#include "main.h"

volatile unsigned char time2_num = 0;
volatile unsigned char start_linking = 0;


static void TIM2_NVIC_Configuration(void);
static void TIM2_Configuration(int16_t tim);


/*定时器2初始化，定时 ntime  ms*/
void Time_2_Init(int16_t ntime)
{
	TIM2_NVIC_Configuration();
	TIM2_Configuration(ntime);
	
}

/*
中断周期为 tim  ms
注意TIM_Period是一个16位无符号数，最大65535，也就是tim*1000最大不能超过65535
也就是tim最大不能超过65
*/
static void TIM2_Configuration(int16_t tim)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
	
    TIM_TimeBaseStructure.TIM_Period = tim*1000;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);	//时钟预分频数 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);			// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);		//使能定时器中断
    TIM_Cmd(TIM2, ENABLE);	// 开启时钟    
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//先关闭等待使用  
}


/* TIM2中断优先级配置 */
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval : None
  */
void TIM2_IRQHandler(void)
{
	if(start_linking == 1)
	{
		time2_num++;		
	}
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

