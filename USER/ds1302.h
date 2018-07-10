
#ifndef __DS1302_H
#define __DS1302_H

#include <stm32f10x.h>
#include "hardware.h"

//DS1302������ѹ��3.3-5V
//----------------------------------
#define DS1302_PORT         GPIOA

#define DS1302_SCK_PIN 		GPIO_Pin_4		//ʱ��
#define DS1302_IO_PIN 		GPIO_Pin_6    //˫��IO��
#define DS1302_CE_PIN 		GPIO_Pin_5   //Ƭѡʹ�ܣ�����Ҫ��д��ʱ���ø�λ

#define DS1302_SCK          PAout(4)  //λ����������ֱ�Ӹ��ߵ͵�ƽ�������мǲ��ܸ�0/1֮��������м�
#define DS1302_CE           PAout(5)
#define DS1302_DATIN        PAin(6)   //io����Ϊ����
#define DS1302_DATOUT       PAout(6)  //io����Ϊ���

//#define DS1302_DAT_INPUT()     {GPIOB->CRL&= 0XF0FFFFFF;GPIOB->CRL|= 8<<24;}  //���ó�����������������ģʽ,��Ҫ�����������
//#define DS1302_DAT_OUTPUT()    {GPIOB->CRL&= 0XF0FFFFFF;GPIOB->CRL|= 3<<24;}  //���ó����50M��ͨ���������


typedef struct _time{ 

	u8 second;
	u8 minute;
	u8 hour;
	u8 date;
	u8 month;
	u8 week;
	u8 year;

}my_time;


void DS1302_Init(void);
void DS1302_ReadTime(void);
void Display_Real_Time(void);  //��ʾʵʱʱ��
void adjust_real_time(void);

#endif

