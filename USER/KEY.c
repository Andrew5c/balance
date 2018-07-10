
/*
���밴����ʵ��׶Σ���������ʱ����������10���Ϩ����������ϣ��ð�J-LINK���߰��˲����ð���KEY2��
���������key2 ����PA15��KEY4->PA14��KAY3->PA13
���жϷ���ʱ�����������Ȼ�����ж��ֳ��ı�����Ȼ����ת���ж��������в����жϴ���������ڵ�ַ��
����ִ����Ӧ���жϴ�����.
*/

//��ʱ����key2ȥƤ��key3���ۼӣ�key4���ۼ�

#include <stm32f10x.h>
#include "main.h"

#define KEY_ON		0
#define KEY_OFF	1


/*----------------------------------
**�������ƣ�key_init
**�����������������ų�ʼ�����ж�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void Key_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;			//����һ��EXTI�ṹ�����

	//1��ʹ��GPIO���ⲿ�жϱ�������APIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

	//2��GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //�ж�����Ϊ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//��������
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//3������жϱ�־�������ж���·,��GPIO���ж�ӳ��һ��
	EXTI_ClearITPendingBit(EXTI_Line14);
	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽΪ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//�½��س���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//ʹ���ж���
	EXTI_Init(&EXTI_InitStructure);							//���ݲ�����ʼ���жϼĴ���
	
	//4�������ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���2
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//�жϴ���������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�ж�ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);							   	//���ݲ�����ʼ���жϼĴ���

	//���ˣ��ж����е����þ��������ˣ����Ǽ򵥵İ������жϣ������Ŀ���Ҫ����һ��

}

/*
** ����ʱ��������ⰴ���ɿ��İ����ж�
*/
void EXTI15_10_IRQHandler(void)
{
	//KEY1
	if(EXTI_GetITStatus(EXTI_Line14) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == KEY_ON)
		{
			Delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == KEY_ON)
			{
				Restart_From_Low_Power();			 //ͣ�����Ѻ���Ҫ����HSE	
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line14);    //����жϱ�־λ
	}
	/*
	//KEY2
	else if(EXTI_GetITStatus(EXTI_Line15) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == KEY_ON)
		{
			Delay_ms(10);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == KEY_ON)
			{
				Beep_Warning(1,100);
				ESP8266_Link_WIFI();		//����WiFi
				start_linking = !start_linking;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line15);    //����жϱ�־λ
	}
	*/
}

