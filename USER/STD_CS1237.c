
/*
** 24位差分ADC芯片，内置放大电路，128倍放大
*/

#include "main.h"


#define SCS1237_GPIO_PORT		GPIOA

#define SSCLK_PIN					GPIO_Pin_1
#define SDOUT_PIN					GPIO_Pin_0

//之前使用这种方式可以把IO口双向使用，但是在这里就不能用了，不知道为什么
//#define SCLK           PAout(2)
//#define SDOUT_IN        PAin(4)   //io设置为输入
//#define DOUT_OUT       PAout(4)  //io设置为输出

//使用输入输出切换的方式进行读写
#define SSCLK_1		GPIO_SetBits(SCS1237_GPIO_PORT,SSCLK_PIN)
#define SSCLK_0		GPIO_ResetBits(SCS1237_GPIO_PORT,SSCLK_PIN)

#define SDOUT_1		GPIO_SetBits(SCS1237_GPIO_PORT,SDOUT_PIN)
#define SDOUT_0		GPIO_ResetBits(SCS1237_GPIO_PORT,SDOUT_PIN)

//读取端口电平
#define SDOUT_IN()		GPIO_ReadInputDataBit(SCS1237_GPIO_PORT,SDOUT_PIN)

#define SCS_CONFIG   0X0C		//芯片配置寄存器地址
#define SADC_BIT     20			//ADC有效位数，带符号位，最高24位
#define SFILTER		  0.00001	//滤波系数，小于1


long int STD_ADC_LAST = 0;	//上一轮的ADC数值保存
long int STD_ADC_NOW = 0;


static void SCS1237_GPIO_Init(void);
static void SOne_CLK(void);
static void SCS1237_delay_us(u32 time);
static void SCS1237_delay_ms(u32 time);

static void Smode_IPU(void);
static void Smode_Out_PP(void);


//引脚初始化之后，默认是低电平
static void SCS1237_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = SSCLK_PIN | SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
	
}

//设置为输入模式
static void Smode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
}
//设置为输出模式
static void Smode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
}


//生成一个脉冲
static void SOne_CLK(void)
{
	SSCLK_1;
	SCS1237_delay_us(10);
	SSCLK_0;
	SCS1237_delay_us(10);
}

//高电平最少保持100us，cs1237进入低功耗模式，芯片关闭所有电路，功耗接近0
void SCS1237_Power_Down(void)
{
	SSCLK_1;
	SCS1237_delay_us(100);
	SSCLK_1;
	SCS1237_delay_us(100);
}

//cs1237重新唤醒,SCLK回到低电平并保持10us
void SCS1237_Restart(void)
{
	SSCLK_0;
	SCS1237_delay_us(20);
}


/*---------------------------------
** 名称：CS1237_Write_Config
** 功能：向控制寄存器中写控制字
** 入口：需要写入的控制字
** 出口：无
** 说明：
写控制寄存器地址：0X65
写入默认控制字：0X0C
----------------------------------*/
void SCS1237_Write_Config(unsigned char config)
{
	unsigned char i = 0;
	unsigned char _dat = 0x80;
	unsigned char count_i = 0;//溢出计时器

	SSCLK_0;//时钟拉低
	
	Smode_IPU();
	while(SDOUT_IN() == 1)//芯片准备好数据输出，时钟已经为0，数据也需要等CS1237全部拉低为0才算都准备好
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return;//超时，则直接退出程序
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		SOne_CLK();
	}
	Smode_Out_PP();
	//第30-36个时钟周期，输入寄存器的写或读命令字数据（高位先输入），这里是写，应输入0x65
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//30
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//31
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//32
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//33
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//34
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//35
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//36
	
	SOne_CLK();//37，切换DOUT的方向  

	for(i=0;i<8;i++)// 38 - 45个脉冲，写入寄存器的配置数据（高位先写入）
	{
		SSCLK_1;SCS1237_delay_us(40);
		if((config & _dat) != 0)
			SDOUT_1;
		else
			SDOUT_0;
		SSCLK_0;SCS1237_delay_us(40);
		_dat >>= 1;
	}
	SOne_CLK();//46个脉冲，切换DOUT引脚，并且拉高DOUT引脚
}


/*---------------------------------
** 名称：CS1237_Read_Config
** 功能：读取CS1237的控制寄存器中的数据
** 入口：无
** 出口：读出的控制字
** 说明：
读控制寄存器地址：0X56
读出刚才写的控制字0X0C，如果读出的控制字和刚才写入的一样，说明通信成功
----------------------------------*/
unsigned char SCS1237_Read_Config(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;		//读取到的数据
	unsigned char count_i = 0;	//溢出计时器
	
	SSCLK_0;
	
	Smode_IPU();
	while(SDOUT_IN() == 1)//芯片准备好数据输出  时钟已经为0，数据也需要等CS1237全部拉低为0才算都准备好
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return 1;//超时，则直接退出程序
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		SOne_CLK();
	}
	
	Smode_Out_PP();
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//30
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//31
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//32
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//33
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//34
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//35
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//36
	
	SDOUT_1;
	
	SOne_CLK();//37     写入了0x56，切换DOUT方向
	
	Smode_IPU();
	for(i=0;i<8;i++)// 38 - 45个脉冲了，读取数据
	{
		SOne_CLK();
		dat <<= 1;
		if(SDOUT_IN() == 1)
			dat++;
	}
	SOne_CLK();//46个脉冲拉高数据引脚
	
	return dat;
}


/*---------------------------------
** 名称：CS1237_Read_ADC
** 功能：读取CS1237的ADC数据
** 入口：无
** 出口：20位的ADC数据
** 说明：
----------------------------------*/
long int SCS1237_Read_ADC(void)
{
	unsigned char i = 0;
	long int dat = 0;
	unsigned char count_i = 0;
	
//	Smode_Out_PP();
//	SDOUT_1;//端口锁存，32不一定需要
	SSCLK_0;
	
	Smode_IPU();
	while(SDOUT_IN() == 1)
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return 0;//超时，则直接退出程序
		}
	}
		
	//SDOUT_1;//锁存？
	//Smode_IPU();
	
	//获取24位有效转换
	for(i = 0; i<24; i++)
	{
		SSCLK_1;
		SCS1237_delay_us(10);
		dat <<= 1;
		if(SDOUT_IN() == 1)
			dat++;
		SSCLK_0;SCS1237_delay_us(10);	
	}
	//一共需要输入27个脉冲
	for(i=0; i<3; i++)
		SOne_CLK();
	
	Smode_Out_PP();
	SDOUT_1;
	
	i = 24 - SADC_BIT;
	dat >>= i;		//丢弃多余的位数
	
	return dat;
}


/*---------------------------------
** 名称：CS1237_Read_18bit_ADC
** 功能：对原始的ADC数据进行低通滤波
** 入口：无
** 出口：18位的ADC数据
** 说明：
一阶数字滤波器（一阶低通滤波器）
本次滤波数据  C = B*SFILTER + C*(1-SFILTER)
优点：
	  对周期性干扰具有良好的抑制作用
	  适用于波动频率较高的场合
缺点：
	  相位滞后，灵敏度低
	  滞后程度取决于a值大小
	  不能消除滤波频率高于采样频率的1/2的干扰信号
----------------------------------*/
long int SCS1237_Read_18bit_ADC(void)
{
	//本次数据
	STD_ADC_NOW = SCS1237_Read_ADC();
	
	//读取到正确的数据
	if(STD_ADC_NOW != 0)
	{
		STD_ADC_LAST = STD_ADC_LAST*SFILTER + STD_ADC_NOW*(1 - SFILTER);
	}
	return STD_ADC_LAST; 
}


//初始化ADC相关参数
void SCS1237_Init(void)
{
	//端口初始化
	SCS1237_GPIO_Init();
	
	SCS1237_delay_ms(1);
	
	//向芯片中写入一个默认控制字
	SCS1237_Write_Config(0x0c);
	//直到读取到正确的数据
	while(SCS1237_Read_Config() != 0x0c)
	{
		//如果读取的数据和写入的数据不一样，说明没有通信成功
		printf(" \n\tSCS1237 read error...\n");
		continue;
	}
	
	printf("\n\tSCS1237 read Success!!\n");
	
	STD_ADC_LAST = SCS1237_Read_ADC();
	STD_ADC_LAST = SCS1237_Read_ADC();
	STD_ADC_LAST = SCS1237_Read_ADC();
}


/*
** 内部延时函数，乘8这里正好是1us
*/
static void SCS1237_delay_us(u32 time)
{    
  u32 i = 8 * time;  
  while(i--); 
}
//ms延时
static void SCS1237_delay_ms(u32 time)  
{  
  u32 i=8000*time;  
  while(i--);  
}  


