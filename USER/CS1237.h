
#ifndef __CS1237_H
#define __CS1237_H

#define CS1237_GPIO_PORT		GPIOB

#define SCLK_PIN					GPIO_Pin_9
#define DOUT_PIN					GPIO_Pin_8

//֮ǰʹ�����ַ�ʽ���԰�IO��˫��ʹ�ã�����������Ͳ������ˣ���֪��Ϊʲô
//#define SCLK           PAout(2)
//#define DOUT_IN        PAin(4)   //io����Ϊ����
//#define DOUT_OUT       PAout(4)  //io����Ϊ���

//ʹ����������л��ķ�ʽ���ж�д
#define SCLK_1		GPIO_SetBits(CS1237_GPIO_PORT,SCLK_PIN)
#define SCLK_0		GPIO_ResetBits(CS1237_GPIO_PORT,SCLK_PIN)

#define DOUT_1		GPIO_SetBits(CS1237_GPIO_PORT,DOUT_PIN)
#define DOUT_0		GPIO_ResetBits(CS1237_GPIO_PORT,DOUT_PIN)

//��ȡ�˿ڵ�ƽ
#define DOUT_IN()		GPIO_ReadInputDataBit(CS1237_GPIO_PORT,DOUT_PIN)

#define CS_CONFIG   0X0C		//оƬ���üĴ�����ַ
#define ADC_BIT     20			//ADC��Чλ����������λ�����24λ
#define FILTER		  0.00001	//�˲�ϵ����С��1

extern long int ADC_LAST ;	//��һ�ֵ�ADC��ֵ����
extern long int ADC_NOW ;

void CS1237_Init(void);
void CS1237_Power_Down(void);
void CS1237_Restart(void);
long int CS1237_Read_18bit_ADC(void);
long int CS1237_Read_ADC(void);
void CS1237_Write_Config(unsigned char config);
unsigned char CS1237_Read_Config(void);

#endif
