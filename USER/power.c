
#include "main.h"


/*
** �͹���ģʽ:
	��ֹͣģʽ�У���һ���ر����������е�ʱ�ӣ����е����趼ֹͣ���������������ں˵ļĴ������ڴ����Ϣ��
	���Դ�ֹͣģʽ���ѣ������¿���ʱ�Ӻ󣬻����Դ��ϴ�ֹͣ������ִ�д��롣
	ֹͣģʽ����������һ���ⲿ�ж�(EXTI)����.
*/
void Low_Power_Mode(void)
{
	//CS1237�͹���
	CS1237_Power_Down();
	
	//������Ϣ��
	HMI_Sleep_Mode();
	
	//OLEDϢ��
	OLED_Display_Off();
	
	//ʹ�ܵ�Դ����Ԫ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	
	//STM32�ĵ͹���,����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��жϻ���
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);	
}


/*
** ϵͳ��ֹͣģʽ������ʱ��ʹ��HSI��Ϊϵͳʱ�ӵ�
   ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL
   ����ѡ��PLL��Ϊϵͳʱ��.
*/
void Restart_From_Low_Power(void)
{
	//����������stm32
	ErrorStatus HSEStartUpStatus;
   //ʹ�� HSE
   RCC_HSEConfig(RCC_HSE_ON);

   //�ȴ� HSE ׼������
   HSEStartUpStatus = RCC_WaitForHSEStartUp();

   if(HSEStartUpStatus == SUCCESS)
   {
		 //ʹ�� PLL
		 RCC_PLLCmd(ENABLE);

		 //�ȴ� PLL ׼������
		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		 {
		 }

		 //ѡ��PLL��Ϊϵͳʱ��Դ 
		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		 //�ȴ�PLL��ѡ��Ϊϵͳʱ��Դ
		 while(RCC_GetSYSCLKSource() != 0x08)
		 {
		 }
   }
	
	//����CS1237
	CS1237_Restart();
	
	//����������
	HMI_Unsleep_Mode();
	
	//��ʾOLED
	OLED_Display_On();
	
}


