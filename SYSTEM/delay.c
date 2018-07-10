
#include "delay.h"

//stm32普通延时函数

void Delay_us(u32 time)  
{  
  u32 i=8*time;  
  while(i--);  
}  


void Delay_ms(u32 time)  
{  
  u32 i=8000*time;  
  while(i--);  
}  

//NOP延时
void Delay_us_nop(u32 nTimer)  
{  
    u32 i=0;  
    for(i=0;i<nTimer;i++)
	{  
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();  
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();  
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();  
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();  
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();  
    }  
} 

void Delay_ms_nop(u32 nTimer)  
{  
    u32 i=1000*nTimer;  
    Delay_us_nop(i);  
}  


