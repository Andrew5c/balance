
/*
** 24位差分ADC芯片，内置放大电路，128倍放大
*/

#include "main.h"

long int ADC_LAST = 0;	//上一轮的ADC数值保存
long int ADC_NOW = 0;


static void CS1237_GPIO_Init(void);
static void One_CLK(void);
static void CS1237_delay_us(u32 time);
static void CS1237_delay_ms(u32 time);

static void mode_IPU(void);
static void mode_Out_PP(void);


//引脚初始化之后，默认是低电平
static void CS1237_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = SCLK_PIN | DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
	
}

//设置为输入模式
static void mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
}
//设置为输出模式
static void mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
}


//生成一个脉冲
static void One_CLK(void)
{
	SCLK_1;
	CS1237_delay_us(10);
	SCLK_0;
	CS1237_delay_us(10);
}

//高电平最少保持100us，cs1237进入低功耗模式，芯片关闭所有电路，功耗接近0
void CS1237_Power_Down(void)
{
	SCLK_1;
	CS1237_delay_us(100);
	SCLK_1;
	CS1237_delay_us(100);
}

//cs1237重新唤醒,SCLK回到低电平并保持10us
void CS1237_Restart(void)
{
	SCLK_0;
	CS1237_delay_us(20);
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
void CS1237_Write_Config(unsigned char config)
{
	unsigned char i = 0;
	unsigned char _dat = 0x80;
	unsigned char count_i = 0;//溢出计时器

	SCLK_0;//时钟拉低
	
	mode_IPU();
	while(DOUT_IN() == 1)//芯片准备好数据输出，时钟已经为0，数据也需要等CS1237全部拉低为0才算都准备好
	{
		CS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return;//超时，则直接退出程序
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		One_CLK();
	}
	mode_Out_PP();
	//第30-36个时钟周期，输入寄存器的写或读命令字数据（高位先输入），这里是写，应输入0x65
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//30
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//31
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//32
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//33
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//34
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//35
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//36
	
	One_CLK();//37，切换DOUT的方向  

	for(i=0;i<8;i++)// 38 - 45个脉冲，写入寄存器的配置数据（高位先写入）
	{
		SCLK_1;CS1237_delay_us(40);
		if((config & _dat) != 0)
			DOUT_1;
		else
			DOUT_0;
		SCLK_0;CS1237_delay_us(40);
		_dat >>= 1;
	}
	One_CLK();//46个脉冲，切换DOUT引脚，并且拉高DOUT引脚
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
unsigned char CS1237_Read_Config(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;		//读取到的数据
	unsigned char count_i = 0;	//溢出计时器
	
	SCLK_0;
	
	mode_IPU();
	while(DOUT_IN() == 1)//芯片准备好数据输出  时钟已经为0，数据也需要等CS1237全部拉低为0才算都准备好
	{
		CS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return 1;//超时，则直接退出程序
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		One_CLK();
	}
	
	mode_Out_PP();
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//30
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//31
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//32
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//33
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//34
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//35
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//36
	
	DOUT_1;
	
	One_CLK();//37     写入了0x56，切换DOUT方向
	
	mode_IPU();
	for(i=0;i<8;i++)// 38 - 45个脉冲了，读取数据
	{
		One_CLK();
		dat <<= 1;
		if(DOUT_IN() == 1)
			dat++;
	}
	One_CLK();//46个脉冲拉高数据引脚
	
	return dat;
}


/*---------------------------------
** 名称：CS1237_Read_ADC
** 功能：读取CS1237的ADC数据
** 入口：无
** 出口：20位的ADC数据
** 说明：
----------------------------------*/
long int CS1237_Read_ADC(void)
{
	unsigned char i = 0;
	long int dat = 0;
	unsigned char count_i = 0;
	
//	mode_Out_PP();
//	DOUT_1;//端口锁存，32不一定需要
	SCLK_0;
	
	mode_IPU();
	while(DOUT_IN() == 1)
	{
		CS1237_delay_ms(1);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return 0;//10s超时，则直接退出程序
		}
	}
		
	//DOUT_1;//锁存？
	//mode_IPU();
	
	//获取24位有效转换
	for(i = 0; i<24; i++)
	{
		SCLK_1;
		CS1237_delay_us(10);
		dat <<= 1;
		if(DOUT_IN() == 1)
			dat++;
		SCLK_0;CS1237_delay_us(10);	
	}
	//一共需要输入27个脉冲
	for(i=0; i<3; i++)
		One_CLK();
	
	mode_Out_PP();
	DOUT_1;
	
	i = 24 - ADC_BIT;
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
本次滤波数据  C = B*FILTER + C*(1-FILTER)
优点：
	  对周期性干扰具有良好的抑制作用
	  适用于波动频率较高的场合
缺点：
	  相位滞后，灵敏度低
	  滞后程度取决于a值大小
	  不能消除滤波频率高于采样频率的1/2的干扰信号
----------------------------------*/
long int CS1237_Read_18bit_ADC(void)
{
	//本次数据
	ADC_NOW = CS1237_Read_ADC();
	
	//读取到正确的数据
	if(ADC_NOW != 0)
	{
		ADC_LAST = ADC_LAST*FILTER + ADC_NOW*(1 - FILTER);
	}
	return ADC_LAST; 
}


//初始化ADC相关参数
void CS1237_Init(void)
{
	//端口初始化
	CS1237_GPIO_Init();
	
	CS1237_delay_ms(1);
	
	//向芯片中写入一个默认控制字
	CS1237_Write_Config(0x0c);
	//直到读取到正确的数据
	while(CS1237_Read_Config() != 0x0c)
	{
		//如果读取的数据和写入的数据不一样，说明没有通信成功
		printf(" \n\tCS1237 read error...\n");
		continue;
	}
	
	printf("\n\tCS1237 read Success!!\n");
	
	ADC_LAST = CS1237_Read_ADC();
	ADC_LAST = CS1237_Read_ADC();
	ADC_LAST = CS1237_Read_ADC();
}


/*
** 内部延时函数，乘8这里正好是1us
*/
static void CS1237_delay_us(u32 time)
{    
  u32 i = 8 * time;  
  while(i--); 
}
//ms延时
static void CS1237_delay_ms(u32 time)  
{  
  u32 i=8000*time;  
  while(i--);  
}  


