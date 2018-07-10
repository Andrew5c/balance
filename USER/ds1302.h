
#ifndef __DS1302_H
#define __DS1302_H

#include <stm32f10x.h>
#include "hardware.h"

//DS1302工作电压：3.3-5V
//----------------------------------
#define DS1302_PORT         GPIOA

#define DS1302_SCK_PIN 		GPIO_Pin_4		//时钟
#define DS1302_IO_PIN 		GPIO_Pin_6    //双向IO口
#define DS1302_CE_PIN 		GPIO_Pin_5   //片选使能，当需要读写的时候，置高位

#define DS1302_SCK          PAout(4)  //位带操作，可直接给高低电平，但是切记不能给0/1之外的数。切记
#define DS1302_CE           PAout(5)
#define DS1302_DATIN        PAin(6)   //io设置为输入
#define DS1302_DATOUT       PAout(6)  //io设置为输出

//#define DS1302_DAT_INPUT()     {GPIOB->CRL&= 0XF0FFFFFF;GPIOB->CRL|= 8<<24;}  //设置成上拉或者下拉输入模式,需要外接上拉电阻
//#define DS1302_DAT_OUTPUT()    {GPIOB->CRL&= 0XF0FFFFFF;GPIOB->CRL|= 3<<24;}  //设置成最大50M的通用推挽输出


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
void Display_Real_Time(void);  //显示实时时间
void adjust_real_time(void);

#endif

