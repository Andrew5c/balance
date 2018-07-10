
#include "systick.h"

//u8 volatile timing_flag = 0;//定时完成标志置1，告诉主函数显示时间到

static void SysTick_Init(uint16_t a);   //启动系统滴答定时器，配置中断时间
static __IO u32 TimingDelay;  //systick计数全局变量，volatile好处是告诉编译器这个变量很容易改变，不要优化

//启动系统滴答定时器，配置中断时间
//定时计算    》》》因为SystemFrequency = 72M,
//所以也就是  （72M / 100000）* （1 / 72M）s = 10us
//a = 1 is 1us,,,,a = 10 is 10us,,,,a = 1000 is 1ms,,
static void SysTick_Init(uint16_t a)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0库版本
	if (SysTick_Config(SystemCoreClock / 1000000 * a))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while (1);  //异常，进入死循环
	}
	// 关闭滴答定时器  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}	

//us延时 ，10us为一个单位
void delay_10us(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(10);
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //使能滴答定时器
   while(TimingDelay != 0);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}


//我觉得这个函数完全就是把定时器当做延时器用了。开启了1ms定时器之后就死循环这了，等到1ms定时结束之后才能继续下面的
//程序，没有发挥到定时器的功能，这只是一个精确的延时函数。不是定时闹钟。注意区分定时器和延时器的区别。
void delay_ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(1000);  //设置1ms一个中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //使能滴答定时器
   while(TimingDelay != 0);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk; //关闭定时器
}

/*----------------------------------
**函数名称：Timing_1ms
**功能描述：单位定时1ms，时间到产生一个中断，其实基本和上面一样，就是少了while等待
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void Timing_1ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(1000);  //设置1ms一个中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //使能滴答定时器
}

/*----------------------------------
**函数名称：TimingDelay_Decrement
**功能描述：获取节拍程序，在 SysTick 中断函数 SysTick_Handler()调用
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }

}

