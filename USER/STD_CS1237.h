
#ifndef __STD_CS1237_H
#define __STD_CS1237_H


extern long int STD_ADC_LAST ;	//上一轮的ADC数值保存
extern long int STD_ADC_NOW ;


void SCS1237_Init(void);
void SCS1237_Power_Down(void);
void SCS1237_Restart(void);
long int SCS1237_Read_18bit_ADC(void);
long int SCS1237_Read_ADC(void);
void SCS1237_Write_Config(unsigned char config);
unsigned char SCS1237_Read_Config(void);

#endif
