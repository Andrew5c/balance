#ifndef __ADC_H
#define __ADC_H

#include <stm32f10x.h>

//ע��ADC�����ѹ�ķ�ΧΪ 0-3.3V��

//ADC1��ʼ��ַ���Ϲ������ݼĴ�����ƫ�Ƶ�ַ
//�����ַ����Ӳ����ַ�����ɸı䣬��ʱ����ʹ�� char *const p = ((u32)0x40012400 + 0x4C);�����Σ�
//��ʾָ��pֻ��ָ����Ƭ��ַ��p���ɱ��ı䣬����һ��ֻ��ָ��
#define ADC1_DR_Address    ((u32)0x40012400 + 0x4C)//ADC����ת��֮�����������ַ����


void ADC_All_Init(void);//����������
float ADC_cal_aveg(void);//������������



#endif



