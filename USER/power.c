
#include "main.h"


/*
** 低功耗模式:
	在停止模式中，进一步关闭了其他所有的时钟，所有的外设都停止工作，但保留了内核的寄存器、内存的信息，
	所以从停止模式唤醒，并重新开启时钟后，还可以从上次停止处继续执行代码。
	停止模式可以由任意一个外部中断(EXTI)唤醒.
*/
void Low_Power_Mode(void)
{
	//CS1237低功耗
	CS1237_Power_Down();
	
	//触摸屏息屏
	HMI_Sleep_Mode();
	
	//OLED息屏
	OLED_Display_Off();
	
	//使能电源管理单元的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	
	//STM32的低功耗,进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);	
}


/*
** 系统从停止模式被唤醒时是使用HSI作为系统时钟的
   停机唤醒后配置系统时钟: 使能 HSE, PLL
   并且选择PLL作为系统时钟.
*/
void Restart_From_Low_Power(void)
{
	//启动并配置stm32
	ErrorStatus HSEStartUpStatus;
   //使能 HSE
   RCC_HSEConfig(RCC_HSE_ON);

   //等待 HSE 准备就绪
   HSEStartUpStatus = RCC_WaitForHSEStartUp();

   if(HSEStartUpStatus == SUCCESS)
   {
		 //使能 PLL
		 RCC_PLLCmd(ENABLE);

		 //等待 PLL 准备就绪
		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		 {
		 }

		 //选择PLL作为系统时钟源 
		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		 //等待PLL被选择为系统时钟源
		 while(RCC_GetSYSCLKSource() != 0x08)
		 {
		 }
   }
	
	//启动CS1237
	CS1237_Restart();
	
	//启动触摸屏
	HMI_Unsleep_Mode();
	
	//显示OLED
	OLED_Display_On();
	
}


