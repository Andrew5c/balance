
#ifndef __CS1237_H
#define __CS1237_H

#define CS1237_GPIO_PORT		GPIOB

#define SCLK_PIN					GPIO_Pin_9
#define DOUT_PIN					GPIO_Pin_8

//之前使用这种方式可以把IO口双向使用，但是在这里就不能用了，不知道为什么
//#define SCLK           PAout(2)
//#define DOUT_IN        PAin(4)   //io设置为输入
//#define DOUT_OUT       PAout(4)  //io设置为输出

//使用输入输出切换的方式进行读写
#define SCLK_1		GPIO_SetBits(CS1237_GPIO_PORT,SCLK_PIN)
#define SCLK_0		GPIO_ResetBits(CS1237_GPIO_PORT,SCLK_PIN)

#define DOUT_1		GPIO_SetBits(CS1237_GPIO_PORT,DOUT_PIN)
#define DOUT_0		GPIO_ResetBits(CS1237_GPIO_PORT,DOUT_PIN)

//读取端口电平
#define DOUT_IN()		GPIO_ReadInputDataBit(CS1237_GPIO_PORT,DOUT_PIN)

#define CS_CONFIG   0X0C		//芯片配置寄存器地址
#define ADC_BIT     20			//ADC有效位数，带符号位，最高24位
#define FILTER		  0.00001	//滤波系数，小于1

extern long int ADC_LAST ;	//上一轮的ADC数值保存
extern long int ADC_NOW ;

void CS1237_Init(void);
void CS1237_Power_Down(void);
void CS1237_Restart(void);
long int CS1237_Read_18bit_ADC(void);
long int CS1237_Read_ADC(void);
void CS1237_Write_Config(unsigned char config);
unsigned char CS1237_Read_Config(void);

#endif
