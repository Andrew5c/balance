
#ifndef __USART1_H
#define __USART1_H

#include <stm32f10x.h>

//�����������ļ��м�鴮�ڷ��͹���������
extern unsigned char Tx_Buffer[256];
extern unsigned char Tx_Counter;
extern unsigned char CmdRx_Buffer[50];	


void USART_1_Init(uint32_t BaudRate);
void Send_Senser(void);
//void USART_RX_DMA_Config(void);
unsigned char USART_DMA_Receive(void);


#endif



