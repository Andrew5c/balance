
#include "main.h"

#define SEND_DATA		0

//当前读取的重量
float now_weight = 0;
//触摸屏输入的单价
int price = 0;
//计算金额
float money = 0;
//累加金额的时候，数组存放之前的金额
float add_money = 0;
//格式化缓冲区
char string_buf[50];

 
//判断低功耗需要的变量
float low_power_weight_1 = 0;
float low_power_weight_2 = 0;
unsigned char low_power_num = 0;

//判断连接服务器
volatile unsigned char start_linking = 0;
unsigned char main_loop_num = 0;

extern unsigned char start_dynamic_calibration ;	 //开始动态标定标志

int main(void)
{
	OLED_Init();				//OLED显示
	DS1302_Init();				//实时时钟
	ADC_All_Init();			//采集电源电压
	
	ESP8266_Init(115200);	//ESP8266串口通信配置
	
	USART_1_Init(9600);  	 //HMI串口屏通信配置
	
	Beep_Init();				//蜂鸣器作为报警提示
	LED_Init();					//LED提示
	Key_Init();					//按键唤醒停机模式
	
	CS1237_Init();				//CS1237初始化
	Get_Weight_Coe();			//获取称重系数和上电时称得的重量，后面都减去这个值
	HMI_Send_Start();			//确保和触摸屏的正常通信
	HMI_Unsleep_Mode();		//退出息屏
	
	//先给显示屏一个值，别让人家等急了，以为东西坏的
	sprintf(string_buf,"t0.txt=\"0\"");
	HMI_Send_String(string_buf);
	Delay_ms(20);
	sprintf(string_buf,"t2.txt=\"0\"");
	HMI_Send_String(string_buf);
	
	Beep_Warning(3,100);
	LED_Start();

	while(1)
	{
		#if SEND_DATA
		printf("%ld\n",CS1237_Read_18bit_ADC());
		printf("%.2f g\n",now_weight); 
		Send_Senser();
		#endif
		
		//读取实时重量并刷新
		now_weight = Get_Weight();
				
		if(start_dynamic_calibration != 1)		//动态标定的时候，让系统停止其他工作，
		{
			//检测超出量程报警
			if(now_weight >= 500)		//在这里定时器定时时间到，然后卡死在外部中断和定时器中断之间循环。
				Beep_Warning(3,100);
			else if(now_weight <= 0.8)
				now_weight = 0;
			
			//发送实时重量
			sprintf(string_buf,"t4.txt=\"%.1f\"",now_weight);
			HMI_Send_String(string_buf);
		
			//计算价格并刷新
			//当前新的计算金额加上之前按键累加按键时备份的金额
			money = now_weight * price + add_money;		
			sprintf(string_buf,"t2.txt=\"%.1f\"",money);
			HMI_Send_String(string_buf);
			Delay_ms(10);						//因为9600的波特率，比较低，这里延时等待数据发送完毕，不然后面一直进入串口中断影响后面的发送和接收。
		
			//OLED显示时间和剩余电量
			Display_Real_Time();
			OLED_Show_RealTime_Battery(110,0);	
		}
		
		//查看DMA缓冲区有无数据
		if(USART_DMA_Receive() == 1)
			HMI_Receive();
		
		//低功耗判断
		//仿真发现在没有收到触摸屏的按下时，循环一次的时间大致为1s，这样省去了一个定时器，避免了中断
		low_power_weight_1 = now_weight;
		if((low_power_weight_1 < 1) && ((low_power_weight_1-low_power_weight_2 < 1) || (low_power_weight_2-low_power_weight_1 < 1)))
		{
			low_power_num++;
			LED1_Twinkling(2);
			LED2_Twinkling(2);
			Delay_ms(1000);
		}
		else
		{
			low_power_num = 0;
			low_power_weight_2 = low_power_weight_1;
		}
		
		//重量低于1g并且在40秒内没有变化，即开始进入低功耗
		if(low_power_num >= 50)
		{
			//计数清零，准备下一次的计数
			low_power_num = 0;
			//蜂鸣器首先响用来提示
			Beep_Warning(3,200);		
			//进入低功耗
			Low_Power_Mode();
		}
		
		//登录服务器，保持连接
		if(start_linking == 1)
		{
			main_loop_num++;
			if(main_loop_num >= 20)
			{
				main_loop_num = 0;
				ESP8266_Keep_Linking();
				OLED_Show_String(0,0,(unsigned char *)"wifi");
			}
		}

	}	
}
	

