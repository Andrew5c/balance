
#include "main.h"

/*
** �������治ͬ�İ�������
A		B		C		D		*		#
10		11		12		13		14		15
*/

static void mode_Out(void);
static void mode_IPU(void);


/*
** 4*4���󰴼���Ҫ8���˿ڽ���ѭ��ɨ��
** �У�PA0-PA3 �������
** �У�PA4-PA7 ��������
*/
void Key_Board_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1��ʹ��GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//2��GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��������
	GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	
}

static void mode_Out(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


static void mode_IPU(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//�������� ������ʱ���ڸߵ�ƽ
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*
** �����ڶ�ʱ�����涨ʱѭ�����������ѭ��ɨ�谴����û�а���
** ���ݰ��²�ͬ�İ��������ز�ͬ��ֵ
** ����16˵��û�а�������
*/
unsigned char Key_Board_Scan(void)
{
	//������������Ĳ�ֵͬ��ȷ������İ���
	unsigned char key_value = 0;
	
	if((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F)
	{
		//����
		Delay_ms(20);
		if((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F)
		{
			//ѭ�����еĶ˿��øߵ�ƽ������еĵ�ƽ״̬
			//��������ȫ���ߵ�����Ҫ��д
			
			//��һ��
			GPIO_SetBits(GPIOA, GPIO_Pin_0);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x81: key_value = 1; break;
				case 0x41: key_value = 4; break;
				case 0x21: key_value = 7; break;
				case 0x11: key_value = 14; break;
				default:break;
			}
			
			//�ڶ���
			GPIO_SetBits(GPIOA, GPIO_Pin_1);
			GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x82: key_value = 2; break;
				case 0x42: key_value = 5; break;
				case 0x22: key_value = 8; break;
				case 0x12: key_value = 0; break;
				default:break;
			}
			
			//������
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x84: key_value = 3; break;
				case 0x44: key_value = 6; break;
				case 0x24: key_value = 9; break;
				case 0x04: key_value = 15; break;
				default:break;
			}
			
			//������
			GPIO_SetBits(GPIOA, GPIO_Pin_3);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x88: key_value = 10; break;
				case 0x48: key_value = 11; break;
				case 0x28: key_value = 12; break;
				case 0x08: key_value = 13; break;
				default:break;
			}
							
			//�����ϣ����ö˿�״̬
			GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
			GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
			
			//�ȴ�����̧�𣬳���������
			//���Զ���OXFFFF
			while((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F) ;
			
			//���ذ���ֵ
			return key_value;
		}
		//û�дﵽ������ʱ�䣬����Ϊ�Ƕ�������
		else
			return 16;

	}
	//û�а�������
	else
		return 16;

}


