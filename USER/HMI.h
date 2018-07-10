
#ifndef __HMI_H
#define __HMI_H

void HMI_Send_String(char *buf1);
void HMI_Send_String_1(char *buf1);
void HMI_Send_Start(void);
void HMI_Receive(void);

void HMI_Sleep_Mode(void);
void HMI_Unsleep_Mode(void);

#endif

