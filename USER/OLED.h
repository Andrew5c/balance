/*
OLED显示屏对应引脚链接
CS（片选）———— GPIOD3；
RST（复位）————GPIOD4；
DC（数据命令选择） GPIOD5；
D0(SCL)——————GPIOD6；
D1(SDA)——————GPIOD7;
*/
#ifndef __SPI_H
#define __SPI_H

#include <stm32f10x.h>
#include "systick.h"


#define OLED_PORT   GPIOB

#define OLED_CS_PIN			GPIO_Pin_0
#define OLED_RST_PIN			GPIO_Pin_1
#define OLED_DC_PIN			GPIO_Pin_2
#define OLED_D0_PIN			GPIO_Pin_10			//测试其他端口怎么不行？只能是这一个端口？
#define OLED_D1_PIN			GPIO_Pin_11

//X为1时对应GPIO端口输出高电平，X为0时对应GPIO端口输出低电平 
#define OLED_CS(X)   X?GPIO_SetBits(OLED_PORT, OLED_CS_PIN):GPIO_ResetBits(OLED_PORT, OLED_CS_PIN)  
  
#define OLED_RST(X)  X?GPIO_SetBits(OLED_PORT, OLED_RST_PIN):GPIO_ResetBits(OLED_PORT, OLED_RST_PIN)      
  
#define OLED_DC(X)   X?GPIO_SetBits(OLED_PORT, OLED_DC_PIN):GPIO_ResetBits(OLED_PORT, OLED_DC_PIN)  

#define OLED_D0(X)   X?GPIO_SetBits(OLED_PORT, OLED_D0_PIN):GPIO_ResetBits(OLED_PORT, OLED_D0_PIN)      
  
#define OLED_D1(X)   X?GPIO_SetBits(OLED_PORT, OLED_D1_PIN):GPIO_ResetBits(OLED_PORT, OLED_D1_PIN)      
  
//OLED模式设置
#define SIZE 16
//#define SIZE 8    //SIZE选择英文字体的大小
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define OLED_Order 0       //定义写命令
#define OLED_Data  1       //定义写数据  

//基础操作函数
void SPI_Write(u8 data, u8 Mode);
void OLED_Coord(u8 x, u8 y);  
void OLED_Init(void);
void OLED_Clear(void); 
void OLED_Display_Off(void); 
void OLED_Display_On(void);
void OLED_ShowChinese(u8 x, u8 y, u8 chinese);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_Show_String(u8 x, u8 y, u8 *chr);  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void Clear_Left_Num(unsigned char *num_string);
void Clear_Left_Num2(unsigned char *num_string);

//电子秤使用显示函数
void OLED_Show_Tips(void);

//手环使用显示时间函数
void OLED_Show_16X32_Num(u8 x, u8 y, u8 num); //显示时间和中间的冒号
void OLED_Show_Big_Time(u8 x,u8 y,u32 num,u8 len);  //调用上面的函数直接显示两位数的时间

//倒立摆使用显示函数
//void OLED_Show_encoder_address(void);

#endif

