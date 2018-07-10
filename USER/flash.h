#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"  


//����ʵ��оƬflash���ô�С
#define STM32_FLASH_SIZE 64 	 			 //��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)


//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 		  0x08000000 	//STM32 FLASH����ʼ��ַ

//ѡ�������ݵ���ʼ��ַ
#define FLASH_SAVE_ADDR  		  0x08018000   //�ֶ�ֵ����ַ
#define FLASH_SAVE_ADDR_PRICE   0x08019000 	//���۴洢��ַ


u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif


