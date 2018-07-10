
#include "main.h"

//调用的外部变量
extern int price ;
extern float money ;
extern float add_money ;
extern unsigned char DMA_Receive_LEN ;
extern volatile unsigned char start_linking;
extern float now_weight;


//内部使用的变量
unsigned char money_num = 0;
float money_buf[10] = {0};
unsigned char start_dynamic_calibration = 0;	 //开始动态标定标志
unsigned char ensure_dynamic_calibration = 0; //确认标定的数据


/*
** 确保串口HMI正常通信
*/
void HMI_Send_Start(void)
{
	Delay_ms(30);
	HMI_Send_String("'\0'");
	Delay_ms(30);
}


/*---------------------------------
** 名称：HMI_Send_String
** 功能：中断方式发送字符串给串口屏，包括结束字符
** 入口：需要发送的数据的指针
** 出口：无
** 说明：先把数据拷贝到缓冲区，然后开启串口中断，主程序可以继续执行
----------------------------------*/
void HMI_Send_String(char *buf1)
{
	u8 i = 0;
	
	while(buf1[i] != '\0')
	{
		Tx_Buffer[Tx_Counter++] = *(buf1+i);
		i++;
	}
	
	Tx_Buffer[Tx_Counter++] = 0XFF;
	Tx_Buffer[Tx_Counter++] = 0XFF;
	Tx_Buffer[Tx_Counter++] = 0XFF;
	
	if(!(USART1->CR1 & USART_CR1_TXEIE))					//中断方式发送
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 	//只要发送寄存器空，就会一直有中断
}

/*---------------------------------
** 名称：HMI_Receive
** 功能：根据返回内容，分情况处理
** 入口：无
** 出口：无
** 说明：这里需要和串口屏的配置相互协调，规定好发送的字符和格式
----------------------------------*/
void HMI_Receive(void)
{
	static unsigned char y_num = 0;
	char i = 0;
	char buf[50] = {0};
	
	//前两个数据是0x65和页面ID，已经在中断接受中处理
	switch(CmdRx_Buffer[2])
	{
		//去皮r
		case 0x72:
			remove_empty_weight();
			Beep_Warning(1,100);
			break;
			
		//累加金额a
		case 0x61:
			add_money = money;		//备份当前总金额
			Beep_Warning(1,100);
			price = 0;
			break;
		
		//确认单价d
		case 0x64:
			if(DMA_Receive_LEN == 5)
				price = CmdRx_Buffer[DMA_Receive_LEN-2] - 48;
			else if(DMA_Receive_LEN == 6)
				price = (CmdRx_Buffer[DMA_Receive_LEN-3]-48)*10 + (CmdRx_Buffer[DMA_Receive_LEN-2]-48);
			else if(DMA_Receive_LEN == 7)
				price = (CmdRx_Buffer[DMA_Receive_LEN-4]-48)*100 + (CmdRx_Buffer[DMA_Receive_LEN-3]-48)*10 + (CmdRx_Buffer[DMA_Receive_LEN-2]-48);	
			Beep_Warning(1,100);
			
			//本次商品的金额存储起来
			money_buf[money_num++] = price * now_weight;
			break;
		
		//清除单价和金额c
		case 0x63:
			price = 0;
			add_money = 0;
			money_num = 0;
			memset(money_buf,0,sizeof(money_buf));
			Beep_Warning(2,100);
			break;
		
		//查看明细p
		case 0x70:
			Beep_Warning(1,100);
			for(i = 0; i<5; i++)
			{
				sprintf(buf,"t%d.txt=\"%.1f\"", i+9, money_buf[i]);	//发送之前各个商品的价格
				HMI_Send_String(buf);
				money_buf[7]+= money_buf[i];
			}	
			sprintf(buf,"t6.txt=\"%.1f\"",money_buf[7]);	//合计总价并发送
			HMI_Send_String(buf);
			break;
			
		//上传数据u
		case 0x75:	
			ESP8266_Update_Data(money);	
			Beep_Warning(2,100);		
			break;

		//联网w
		case 0x77:
			Beep_Warning(1,100);
			ESP8266_Link_WIFI();		//连接WiFi
			start_linking = !start_linking;
			break;
		
		//进入动态标定程序 A
		case 0x41:
			start_dynamic_calibration = 1;
			Beep_Warning(2, 100);
			sprintf(buf,"t26.txt=\"%.5f\"",Weight_Coe_100g);	//进入动态标定的时候，发送一次当前称重系数
			HMI_Send_String(buf);
			Delay_ms(30);
			sprintf(buf,"t25.txt=\"%.1f\"",y_value[y_num++]);	//进入动态标定的时候，发送一次重量提示
			HMI_Send_String(buf);
			Delay_ms(30);
			break;
		
		//确认动态标定的数据 B
		case 0x42:
			ensure_dynamic_calibration = 1;
			Beep_Warning(1, 100);
			sprintf(buf,"t25.txt=\"%.1f\"",y_value[y_num++]);	//更新下一个需要称的重量
			HMI_Send_String(buf);
			Delay_ms(30);
			break;
			
		//完成动态标定，调用拟合函数进行线性拟合 C
		case 0x43:
			start_dynamic_calibration = 0;
			y_num = 0;											//计数清零，准备下一次标定
			Weight_Coe_100g = 1000 * Least_Square();	//拟合的斜率更新称重系数
			Get_Weight_Coe();									//更新完数据之后，重新获取一下0g重量
			Beep_Warning(3, 100);
			LED2_Twinkling(3);
			sprintf(buf,"t26.txt=\"%.5f\"",Weight_Coe_100g);	//拟合完成之后，发送新的称重系数
			HMI_Send_String(buf);
			Delay_ms(20);
			break;
		
		//如果只是进入动态标定，没有操作就返回的话，清零进入时的各种标志	D
		case 0x44:
			start_dynamic_calibration = 0;
			y_num = 0;
			Beep_Warning(1,100);
			break;
		
		default :break;		
	}
	//清空缓冲寄存器
	memset(CmdRx_Buffer,0,sizeof(CmdRx_Buffer));

}


//HMI息屏
void HMI_Sleep_Mode(void)
{
	char buf[10] = {0};
	
	sprintf(buf,"sleep=1");
	HMI_Send_String(buf);
	Delay_ms(20);
}

//HMI退出息屏
void HMI_Unsleep_Mode(void)
{
	char buf[10] = {0};
	
	sprintf(buf,"sleep=0");
	HMI_Send_String(buf);
	Delay_ms(20);
}

