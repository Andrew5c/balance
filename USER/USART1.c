/*
** 蓝牙模块连接这个串口
*/


#include "main.h"


/*
** 数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
** stm32是小端模式，低字节存放在低地址空间，高字节存放在高地址空间
*/
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
#define BYTE4(dwTemp)       (*((char *)(&dwTemp) + 4))

//DMA缓冲区接收的最大数据长度
#define  DMA_Max_Receive_Len  20

unsigned char USART_RX_Buf[DMA_Max_Receive_Len] = {0};
unsigned char DMA_Receive_LEN = 0;


//串口发送电机和摆杆编码器的数据，int型，可正可负
unsigned char Tx_Buffer[256];
unsigned char Tx_Counter = 0;
unsigned char Tx_Counter_Temp = 0;
unsigned char Rx_Buffer[50];				//接受缓冲
unsigned char Rx_Counter = 0;          //接受计数变量
unsigned char data_to_send[50] = {0};  //发送数据打包
unsigned char CmdRx_Buffer[50] = {0};	//符合格式的接收字符缓冲


static void DMA_Start(void);
static void Send_Data(unsigned char *DataToSend ,unsigned char data_num);
static void USART_RX_DMA_Config(void);

/*----------------------------------
**函数名称：USART_1_Init
**功能描述：串口参数配置，包括中断向量的配置
**参数说明：BaudRate：需要配置的波特率
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void USART_1_Init(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体
	NVIC_InitTypeDef  NVIC_InitStructure;									//中断向量结构体
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	//端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//TX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   	//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
	
	//参数模式配置
	USART_DeInit(USART1); 														//复位串口
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率自己设置，蓝牙设置了9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式

	USART_Init(USART1, &USART_InitStructure);								//根据参数初始化串口寄存器
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);							//使能串口中断接收
//	USART_Cmd(USART1, ENABLE);     											//使能串口外设
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); 						//使能串口1的DMA发送
	
	//如下语句解决第1个字节无法正确发送出去的问题 
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	
	
	//配置串口中断向量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//设定中断源为
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//中断占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);							   	//根据参数初始化中断寄存器
	
	USART_RX_DMA_Config();
}

/*---------------------------------
** 名称：USART_RX_DMA_Config
** 功能：配置串口DMA接收
** 入口：无
** 出口：无
** 说明：串口1的RX接在DMA的通道5上面
----------------------------------*/
static void USART_RX_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    //使能DMA传输
	
	DMA_DeInit(DMA1_Channel5); //将DMA的通道5寄存器重设为缺省值
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR; 	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART_RX_Buf; 		//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//数据传输方向，外设读取到内存
	DMA_InitStructure.DMA_BufferSize = DMA_Max_Receive_Len;					//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 				//工作在正常模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Rx_DMA 所标识的寄存器
	
	//先使能DMA，在开启串口，防止上电时第一次无法传输数据。
	DMA_Cmd(DMA1_Channel5, ENABLE); 				//使能USART1 TX DMA1 所指示的通道
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); 		//使能串口1的DMA发送
	USART_Cmd(USART1, ENABLE); //使能串口1
	
	DMA_Start();//开始一次DMA传输
}

/*
** 开启DMA传输前先关闭并重新设置缓冲区数组
*/
static void DMA_Start(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE ); //关闭USART1 TX DMA1 所指示的通道 
	DMA_SetCurrDataCounter(DMA1_Channel5,DMA_Max_Receive_Len);//从新设置缓冲大小,指向数组0
	DMA_Cmd(DMA1_Channel5, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}


/*----------------------------------
** 名称：USART_DMA_Receive
** 功能：检测DMA缓冲区是否接收到有效数据
** 入口：无
** 出口：无
** 说明：在主函数中循环检测DMA缓冲区是否有数据
----------------------------------*/
unsigned char USART_DMA_Receive(void)
{		
	if(USART_RX_Buf[0])
	{
		//延时等待后面的数据接收完成
		//Delay_ms(10);
		
		//计算接收到数据的长度
		DMA_Receive_LEN = DMA_Max_Receive_Len - DMA_GetCurrDataCounter(DMA1_Channel5);
		
		//接收的数据拷贝到缓冲区
		memcpy(CmdRx_Buffer,USART_RX_Buf,DMA_Receive_LEN);
		CmdRx_Buffer[DMA_Receive_LEN] = '\0';
		
		//清空缓冲，并开始下一次的接收
		memset(USART_RX_Buf,0,DMA_Max_Receive_Len);
		DMA_Start();
		return 1;
	}
	else
		return 0;
}


/*----------------------------------
**函数名称：USART1_IRQHandler
**功能描述：串口中断服务函数
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/ 
void USART1_IRQHandler(void)
{
	/*异常中断处理*/
//	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//    {
//       USART_ReceiveData(USART1);										//丢弃数据
//		 USART_ClearFlag(USART1, USART_FLAG_ORE);
//    }
	 
	 /*发送*/
	if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
	{
		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//向DR寄存器写数据的时候，串口就会自动发送
			
		if(Tx_Counter == Tx_Counter_Temp)					//数据包发送完成
		{
		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//关闭TXE中断
			Tx_Counter = 0;		//发送完成计数清零
			Tx_Counter_Temp = 0;
		}
	}
	
	/*接收*/
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)	//读取接收中断标志位USART_IT_RXNE 
//	{	
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//清楚中断标志位
//		Rx_Buffer[Rx_Counter++] = USART_ReceiveData(USART1);//接收数据到缓冲区，这个函数会自动清除中断标志位
//		
//		//注意接收帧头(QS)帧尾(f)
//		if((Rx_Buffer[0] == 0x51) && (Rx_Buffer[1] == 0x53) && (Rx_Buffer[Rx_Counter-1]) == 0x66)
//		{
//			memcpy(CmdRx_Buffer,Rx_Buffer,Rx_Counter);	//直接使用内存拷贝，效率比较高
//			CmdRx_Buffer[Rx_Counter] = '\0';		//最后添加'\0',构成完整数组
//			Rx_Counter = 0;							//计数清零
//			printf("CMD OK!");						//接收成功，发送标志
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
**函数名称：fputc
**功能描述：重定义fputc函数，方便使用printf
**参数说明：无
**作者：Andrew
**日期：2018.1.24
------------------------------------*/
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	 	//USART_GetFlagStatus：得到发送状态位
																				//USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  				//发送一个字符	
	return Data;										  					//返回发送的值
}


/*中断发送方式缓冲区字符*/
static void Send_Data(unsigned char *DataToSend ,unsigned char data_num)
{
	uint8_t i;
	
	for(i = 0;i < data_num;i++)	//要发送的数据放到发送缓冲区
		Tx_Buffer[Tx_Counter++] = *(DataToSend+i);

	if(!(USART1->CR1 & USART_CR1_TXEIE))					//中断方式发送
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 	//只要发送寄存器空，就会一直有中断
}


/*----------------------------------
**函数名称：Send_Senser
**功能描述：发送打包的6050传感器数据
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Send_Senser(void)
{
	u8 cnt = 0;
	uint16_t sum = 0;
	u8 i = 0;

	data_to_send[cnt++]=0xAA;	 //帧头：AAAA
	data_to_send[cnt++]=0xAA;
	data_to_send[cnt++]=0xF1;	 //功能字：与上位机中显示哪个波形有关
	data_to_send[cnt++]=0;	     //需要发送数据的字节数，暂时给0，后面在赋值。
	
	//第一个数据段
	data_to_send[cnt++] = BYTE3(ADC_NOW);//取data[0]数据的高字节，
	data_to_send[cnt++] = BYTE2(ADC_NOW);
	data_to_send[cnt++] = BYTE1(ADC_NOW);
	data_to_send[cnt++] = BYTE0(ADC_NOW);
	
	//第二个数据段
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	//第三个数据段
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
//	data_to_send[cnt++] = 0;
	
	data_to_send[3] = cnt-4;	//计算总数据的字节数。

	for(i=0;i<cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[cnt++]=sum;  //最后一位是校验和

	Send_Data(data_to_send, cnt);   //发送一个数据包

}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
