

#ifndef __USART2_H
#define __USART2_H

#include <stm32f10x.h>

//º¯ÊıÉùÃ÷
void Usart2_GPIO_Init(void);
void Usart2_Configuration(uint32_t BaudRate); 

void ESP8266_Init(uint32_t baud);
#endif


