
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
**函数名称：Usart_GPIO_Init
**功能描述：串口使用引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart2_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//配置串口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   	//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
}


/*----------------------------------
**函数名称：Usart_Configuration
**功能描述：串口参数配置，包括中断向量的配置
**参数说明：BaudRate：需要配置的波特率
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart2_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体
	NVIC_InitTypeDef  NVIC_InitStructure;									//中断向量结构体
	
	USART_DeInit(USART2); 														//复位串口
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率自己设置，蓝牙设置了9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	
	USART_Init(USART2, &USART_InitStructure);								//根据参数初始化串口寄存器
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);			//使能串口中断接收
	USART_ITConfig ( USART2, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

	USART_Cmd(USART2, ENABLE);     											//使能串口外设
	
	//如下语句解决第1个字节无法正确发送出去的问题 
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	
	
	//配置串口中断向量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组2
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//设定中断源为
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//中断占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);							   	//根据参数初始化中断寄存器
	
}




/*----------------------------------
**函数名称：USART1_IRQHandler
**功能描述：串口中断服务函数
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void USART2_IRQHandler()
{
	uint8_t ucCh;
//	unsigned int i = 0;
	
	//接收到数据
	if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART2 );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )          //预留1个字节写结束符
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
		
		//判断是否接受结束
		if(strEsp8266_Fram_Record.Data_RX_BUF[ strEsp8266_Fram_Record .InfBit .FramLength-2]=='^'&&\
			strEsp8266_Fram_Record.Data_RX_BUF[ strEsp8266_Fram_Record .InfBit .FramLength-1]=='!')
		{
			receive_flag = 1;
			//拷贝到缓冲区
			memcpy(receive_buf, strEsp8266_Fram_Record .Data_RX_BUF, strEsp8266_Fram_Record .InfBit .FramLength);
			//添加结束标志
			receive_buf[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
			
			strEsp8266_Fram_Record .InfBit .FramLength = 0;
		}
	
	}
	
	//USART_IT_IDLE 中断是只检测到空闲帧时产生的中断
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                 //数据帧接收完毕
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( USART2 );                                     //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
		//ucTcpClosedFlag = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
   }	
	

}


