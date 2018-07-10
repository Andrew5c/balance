
#include "main.h"

extern volatile uint8_t ucTcpClosedFlag;

volatile unsigned char receive_flag = 0;
char receive_buf[150] = {0};


void ESP8266_Init(uint32_t baud)
{
	Usart2_GPIO_Init();
	Usart2_Configuration(baud);
}


/*----------------------------------
**�������ƣ�Usart_GPIO_Init
**��������������ʹ�����ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart2_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//���ô���
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   	//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
}


/*----------------------------------
**�������ƣ�Usart_Configuration
**�������������ڲ������ã������ж�����������
**����˵����BaudRate����Ҫ���õĲ�����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart2_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��
	NVIC_InitTypeDef  NVIC_InitStructure;									//�ж������ṹ��
	
	USART_DeInit(USART2); 														//��λ����
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//�������Լ����ã�����������9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	
	USART_Init(USART2, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);			//ʹ�ܴ����жϽ���
	USART_ITConfig ( USART2, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	

	USART_Cmd(USART2, ENABLE);     											//ʹ�ܴ�������
	
	//�����������1���ֽ��޷���ȷ���ͳ�ȥ������ 
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	
	
	//���ô����ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���2
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//�趨�ж�ԴΪ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//�ж�ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);							   	//���ݲ�����ʼ���жϼĴ���
	
}




/*----------------------------------
**�������ƣ�USART1_IRQHandler
**���������������жϷ�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void USART2_IRQHandler()
{
	uint8_t ucCh;
//	unsigned int i = 0;
	
	//���յ�����
	if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART2 );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )          //Ԥ��1���ֽ�д������
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
		
		//�ж��Ƿ���ܽ���
		if(strEsp8266_Fram_Record.Data_RX_BUF[ strEsp8266_Fram_Record .InfBit .FramLength-2]=='^'&&\
			strEsp8266_Fram_Record.Data_RX_BUF[ strEsp8266_Fram_Record .InfBit .FramLength-1]=='!')
		{
			receive_flag = 1;
			//������������
			memcpy(receive_buf, strEsp8266_Fram_Record .Data_RX_BUF, strEsp8266_Fram_Record .InfBit .FramLength);
			//��ӽ�����־
			receive_buf[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
			
			strEsp8266_Fram_Record .InfBit .FramLength = 0;
		}
	
	}
	
	//USART_IT_IDLE �ж���ֻ��⵽����֡ʱ�������ж�
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                 //����֡�������
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( USART2 );                                     //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
	
		//ucTcpClosedFlag = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
   }	
	

}


