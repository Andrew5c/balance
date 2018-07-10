/*
** ����ģ�������������
*/


#include "main.h"


/*
** ���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
** stm32��С��ģʽ�����ֽڴ���ڵ͵�ַ�ռ䣬���ֽڴ���ڸߵ�ַ�ռ�
*/
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
#define BYTE4(dwTemp)       (*((char *)(&dwTemp) + 4))

//DMA���������յ�������ݳ���
#define  DMA_Max_Receive_Len  20

unsigned char USART_RX_Buf[DMA_Max_Receive_Len] = {0};
unsigned char DMA_Receive_LEN = 0;


//���ڷ��͵���Ͱڸ˱����������ݣ�int�ͣ������ɸ�
unsigned char Tx_Buffer[256];
unsigned char Tx_Counter = 0;
unsigned char Tx_Counter_Temp = 0;
unsigned char Rx_Buffer[50];				//���ܻ���
unsigned char Rx_Counter = 0;          //���ܼ�������
unsigned char data_to_send[50] = {0};  //�������ݴ��
unsigned char CmdRx_Buffer[50] = {0};	//���ϸ�ʽ�Ľ����ַ�����


static void DMA_Start(void);
static void Send_Data(unsigned char *DataToSend ,unsigned char data_num);
static void USART_RX_DMA_Config(void);

/*----------------------------------
**�������ƣ�USART_1_Init
**�������������ڲ������ã������ж�����������
**����˵����BaudRate����Ҫ���õĲ�����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void USART_1_Init(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��
	NVIC_InitTypeDef  NVIC_InitStructure;									//�ж������ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	//�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//TX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   	//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
	
	//����ģʽ����
	USART_DeInit(USART1); 														//��λ����
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//�������Լ����ã�����������9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ

	USART_Init(USART1, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);							//ʹ�ܴ����жϽ���
//	USART_Cmd(USART1, ENABLE);     											//ʹ�ܴ�������
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); 						//ʹ�ܴ���1��DMA����
	
	//�����������1���ֽ��޷���ȷ���ͳ�ȥ������ 
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	
	
	//���ô����ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//�趨�ж�ԴΪ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�ж�ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);							   	//���ݲ�����ʼ���жϼĴ���
	
	USART_RX_DMA_Config();
}

/*---------------------------------
** ���ƣ�USART_RX_DMA_Config
** ���ܣ����ô���DMA����
** ��ڣ���
** ���ڣ���
** ˵��������1��RX����DMA��ͨ��5����
----------------------------------*/
static void USART_RX_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    //ʹ��DMA����
	
	DMA_DeInit(DMA1_Channel5); //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR; 	//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART_RX_Buf; 		//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//���ݴ��䷽�������ȡ���ڴ�
	DMA_InitStructure.DMA_BufferSize = DMA_Max_Receive_Len;					//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 				//����������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Rx_DMA ����ʶ�ļĴ���
	
	//��ʹ��DMA���ڿ������ڣ���ֹ�ϵ�ʱ��һ���޷��������ݡ�
	DMA_Cmd(DMA1_Channel5, ENABLE); 				//ʹ��USART1 TX DMA1 ��ָʾ��ͨ��
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); 		//ʹ�ܴ���1��DMA����
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���1
	
	DMA_Start();//��ʼһ��DMA����
}

/*
** ����DMA����ǰ�ȹرղ��������û���������
*/
static void DMA_Start(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE ); //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
	DMA_SetCurrDataCounter(DMA1_Channel5,DMA_Max_Receive_Len);//�������û����С,ָ������0
	DMA_Cmd(DMA1_Channel5, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}


/*----------------------------------
** ���ƣ�USART_DMA_Receive
** ���ܣ����DMA�������Ƿ���յ���Ч����
** ��ڣ���
** ���ڣ���
** ˵��������������ѭ�����DMA�������Ƿ�������
----------------------------------*/
unsigned char USART_DMA_Receive(void)
{		
	if(USART_RX_Buf[0])
	{
		//��ʱ�ȴ���������ݽ������
		//Delay_ms(10);
		
		//������յ����ݵĳ���
		DMA_Receive_LEN = DMA_Max_Receive_Len - DMA_GetCurrDataCounter(DMA1_Channel5);
		
		//���յ����ݿ�����������
		memcpy(CmdRx_Buffer,USART_RX_Buf,DMA_Receive_LEN);
		CmdRx_Buffer[DMA_Receive_LEN] = '\0';
		
		//��ջ��壬����ʼ��һ�εĽ���
		memset(USART_RX_Buf,0,DMA_Max_Receive_Len);
		DMA_Start();
		return 1;
	}
	else
		return 0;
}


/*----------------------------------
**�������ƣ�USART1_IRQHandler
**���������������жϷ�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/ 
void USART1_IRQHandler(void)
{
	/*�쳣�жϴ���*/
//	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//    {
//       USART_ReceiveData(USART1);										//��������
//		 USART_ClearFlag(USART1, USART_FLAG_ORE);
//    }
	 
	 /*����*/
	if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
	{
		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//��DR�Ĵ���д���ݵ�ʱ�򣬴��ھͻ��Զ�����
			
		if(Tx_Counter == Tx_Counter_Temp)					//���ݰ��������
		{
		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//�ر�TXE�ж�
			Tx_Counter = 0;		//������ɼ�������
			Tx_Counter_Temp = 0;
		}
	}
	
	/*����*/
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)	//��ȡ�����жϱ�־λUSART_IT_RXNE 
//	{	
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//����жϱ�־λ
//		Rx_Buffer[Rx_Counter++] = USART_ReceiveData(USART1);//�������ݵ�������������������Զ�����жϱ�־λ
//		
//		//ע�����֡ͷ(QS)֡β(f)
//		if((Rx_Buffer[0] == 0x51) && (Rx_Buffer[1] == 0x53) && (Rx_Buffer[Rx_Counter-1]) == 0x66)
//		{
//			memcpy(CmdRx_Buffer,Rx_Buffer,Rx_Counter);	//ֱ��ʹ���ڴ濽����Ч�ʱȽϸ�
//			CmdRx_Buffer[Rx_Counter] = '\0';		//������'\0',������������
//			Rx_Counter = 0;							//��������
//			printf("CMD OK!");						//���ճɹ������ͱ�־
//			
//			HMI_Receive();
//			
//		}	
//		else
//			printf("no...\n");
//		
//	}
}


/*----------------------------------
**�������ƣ�fputc
**�����������ض���fputc����������ʹ��printf
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
------------------------------------*/
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	 	//USART_GetFlagStatus���õ�����״̬λ
																				//USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  				//����һ���ַ�	
	return Data;										  					//���ط��͵�ֵ
}


/*�жϷ��ͷ�ʽ�������ַ�*/
static void Send_Data(unsigned char *DataToSend ,unsigned char data_num)
{
	uint8_t i;
	
	for(i = 0;i < data_num;i++)	//Ҫ���͵����ݷŵ����ͻ�����
		Tx_Buffer[Tx_Counter++] = *(DataToSend+i);

	if(!(USART1->CR1 & USART_CR1_TXEIE))					//�жϷ�ʽ����
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 	//ֻҪ���ͼĴ����գ��ͻ�һֱ���ж�
}


/*----------------------------------
**�������ƣ�Send_Senser
**�������������ʹ����6050����������
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Send_Senser(void)
{
	u8 cnt = 0;
	uint16_t sum = 0;
	u8 i = 0;

	data_to_send[cnt++]=0xAA;	 //֡ͷ��AAAA
	data_to_send[cnt++]=0xAA;
	data_to_send[cnt++]=0xF1;	 //�����֣�����λ������ʾ�ĸ������й�
	data_to_send[cnt++]=0;	     //��Ҫ�������ݵ��ֽ�������ʱ��0�������ڸ�ֵ��
	
	//��һ�����ݶ�
	data_to_send[cnt++] = BYTE3(ADC_NOW);//ȡdata[0]���ݵĸ��ֽڣ�
	data_to_send[cnt++] = BYTE2(ADC_NOW);
	data_to_send[cnt++] = BYTE1(ADC_NOW);
	data_to_send[cnt++] = BYTE0(ADC_NOW);
	
	//�ڶ������ݶ�
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	//���������ݶ�
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
	
	data_to_send[3] = cnt-4;	//���������ݵ��ֽ�����

	for(i=0;i<cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[cnt++]=sum;  //���һλ��У���

	Send_Data(data_to_send, cnt);   //����һ�����ݰ�

}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
