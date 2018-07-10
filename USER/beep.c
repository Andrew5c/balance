
#include "main.h"


void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1��ʹ��GPIO���ⲿ�жϱ�������APIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//2��GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}


/*---------------------------------
** ���ƣ�Beep_Warning
** ���ܣ���������ʾ������
** ��ڣ�nbeep����Ҫ�켸��
		   delay������֮�����ʱ
** ���ڣ���
** ˵����
��ʱ�ĵ���ֵ��
100ms��ʱ�������Ƚϼ��٣�����������
300ms��ʱ��Ƚ�������������ʾ
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

