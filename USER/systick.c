
#include "systick.h"

//u8 volatile timing_flag = 0;//��ʱ��ɱ�־��1��������������ʾʱ�䵽

static void SysTick_Init(uint16_t a);   //����ϵͳ�δ�ʱ���������ж�ʱ��
static __IO u32 TimingDelay;  //systick����ȫ�ֱ�����volatile�ô��Ǹ��߱�����������������׸ı䣬��Ҫ�Ż�

//����ϵͳ�δ�ʱ���������ж�ʱ��
//��ʱ����    ��������ΪSystemFrequency = 72M,
//����Ҳ����  ��72M / 100000��* ��1 / 72M��s = 10us
//a = 1 is 1us,,,,a = 10 is 10us,,,,a = 1000 is 1ms,,
static void SysTick_Init(uint16_t a)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock / 1000000 * a))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);  //�쳣��������ѭ��
	}
	// �رյδ�ʱ��  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}	

//us��ʱ ��10usΪһ����λ
void delay_10us(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(10);
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //ʹ�ܵδ�ʱ��
   while(TimingDelay != 0);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}


//�Ҿ������������ȫ���ǰѶ�ʱ��������ʱ�����ˡ�������1ms��ʱ��֮�����ѭ�����ˣ��ȵ�1ms��ʱ����֮����ܼ��������
//����û�з��ӵ���ʱ���Ĺ��ܣ���ֻ��һ����ȷ����ʱ���������Ƕ�ʱ���ӡ�ע�����ֶ�ʱ������ʱ��������
void delay_ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(1000);  //����1msһ���ж�
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //ʹ�ܵδ�ʱ��
   while(TimingDelay != 0);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk; //�رն�ʱ��
}

/*----------------------------------
**�������ƣ�Timing_1ms
**������������λ��ʱ1ms��ʱ�䵽����һ���жϣ���ʵ����������һ������������while�ȴ�
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void Timing_1ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	SysTick_Init(1000);  //����1msһ���ж�
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //ʹ�ܵδ�ʱ��
}

/*----------------------------------
**�������ƣ�TimingDelay_Decrement
**������������ȡ���ĳ����� SysTick �жϺ��� SysTick_Handler()����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }

}

