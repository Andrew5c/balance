/*
OLED��ʾ����Ӧ��������
CS��Ƭѡ���������� GPIOD3��
RST����λ����������GPIOD4��
DC����������ѡ�� GPIOD5��
D0(SCL)������������GPIOD6��
D1(SDA)������������GPIOD7;
*/
#ifndef __SPI_H
#define __SPI_H

#include <stm32f10x.h>
#include "systick.h"


#define OLED_PORT   GPIOB

#define OLED_CS_PIN			GPIO_Pin_0
#define OLED_RST_PIN			GPIO_Pin_1
#define OLED_DC_PIN			GPIO_Pin_2
#define OLED_D0_PIN			GPIO_Pin_10			//���������˿���ô���У�ֻ������һ���˿ڣ�
#define OLED_D1_PIN			GPIO_Pin_11

//XΪ1ʱ��ӦGPIO�˿�����ߵ�ƽ��XΪ0ʱ��ӦGPIO�˿�����͵�ƽ 
#define OLED_CS(X)   X?GPIO_SetBits(OLED_PORT, OLED_CS_PIN):GPIO_ResetBits(OLED_PORT, OLED_CS_PIN)  
  
#define OLED_RST(X)  X?GPIO_SetBits(OLED_PORT, OLED_RST_PIN):GPIO_ResetBits(OLED_PORT, OLED_RST_PIN)      
  
#define OLED_DC(X)   X?GPIO_SetBits(OLED_PORT, OLED_DC_PIN):GPIO_ResetBits(OLED_PORT, OLED_DC_PIN)  

#define OLED_D0(X)   X?GPIO_SetBits(OLED_PORT, OLED_D0_PIN):GPIO_ResetBits(OLED_PORT, OLED_D0_PIN)      
  
#define OLED_D1(X)   X?GPIO_SetBits(OLED_PORT, OLED_D1_PIN):GPIO_ResetBits(OLED_PORT, OLED_D1_PIN)      
  
//OLEDģʽ����
#define SIZE 16
//#define SIZE 8    //SIZEѡ��Ӣ������Ĵ�С
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define OLED_Order 0       //����д����
#define OLED_Data  1       //����д����  

//������������
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

//���ӳ�ʹ����ʾ����
void OLED_Show_Tips(void);

//�ֻ�ʹ����ʾʱ�亯��
void OLED_Show_16X32_Num(u8 x, u8 y, u8 num); //��ʾʱ����м��ð��
void OLED_Show_Big_Time(u8 x,u8 y,u32 num,u8 len);  //��������ĺ���ֱ����ʾ��λ����ʱ��

//������ʹ����ʾ����
//void OLED_Show_encoder_address(void);

#endif

