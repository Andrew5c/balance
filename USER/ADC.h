#ifndef __ADC_H
#define __ADC_H

#include <stm32f10x.h>

//注意ADC输入电压的范围为 0-3.3V。

//ADC1起始地址加上规则数据寄存器的偏移地址
//这个地址就是硬件地址，不可改变，这时可以使用 char *const p = ((u32)0x40012400 + 0x4C);来修饰，
//表示指针p只能指向这片地址，p不可被改变，这是一个只读指针
#define ADC1_DR_Address    ((u32)0x40012400 + 0x4C)//ADC数据转换之后存放在这个地址里面


void ADC_All_Init(void);//主函数调用
float ADC_cal_aveg(void);//其他函数调用



#endif



