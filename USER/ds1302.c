
#include "main.h"


//READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};//读取时间的命令地址
//WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};//写时间的命令地址


my_time TIME ;	 //显示时间的结构体
u8 init_time[] = {0x00,0x29,0x00,0x28,0x06,0x02,0x18}; //初始化时间：秒 分 时 日 月 周 年
//u8 init_time[8];

static void DS1302_GPIO_Init(void);
static void DS1302_WriteByte(u8 byte_1);//写一个字节; byte是保留字，不能作为变量
static void DS1302_WriteData(u8 addr,u8 data_);//给某地址写数据,data是c51内部的关键字，表示将变量定义在数据存储区，故此处用data_;
static u8 DS1302_ReadByte(void);//读一个字节
static u8 DS1302_ReadData(u8 addr);//读取某寄存器数据;
static void DS1302_delay_us(u16 time);  //简单延时1us


/*----------------------------------
**函数名称：DS1302_GPIO_Init
**功能描述：DS1302引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void DS1302_GPIO_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;  
    
    //开启GPIOD的时钟  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

    //设置GPIO的基本参数  
    GPIO_InitStruct.GPIO_Pin = DS1302_SCK_PIN | DS1302_CE_PIN ;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //这两个普通端口设为推挽输出  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);  

	 GPIO_InitStruct.GPIO_Pin = DS1302_IO_PIN;         //这里最好设成开漏，当然也可以普通推挽，但是需要后面一直切换输入输出模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;     //开漏输出，需要接上拉，不需要切换输入输出了。
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);

}

/*----------------------------------
**函数名称：DS1302_WriteByte
**功能描述：DS1302写一个字节操作，从最低位开始写入
**参数说明：byte_1为要写入的字节
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void DS1302_WriteByte(u8 byte_1)
{
	u8 i = 0;
	u8 t = 0x01;
	
	for(i = 0;i<8;i++)
	{
		if((byte_1 & t) != 0)     //之前的问题出在这里，32的位带操作不能赋值0和1之外的值。
		{
			DS1302_DATOUT = 1;
		}
		else
		{
			DS1302_DATOUT = 0;
		}
		
		DS1302_delay_us(2);
		DS1302_SCK = 1;  //上升沿写入
		DS1302_delay_us(2);
		DS1302_SCK = 0; 
		DS1302_delay_us(2);
		
		t<<= 1;
	}
	DS1302_DATOUT = 1;      //释放IO，后面读取的话会准确很多
	DS1302_delay_us(2);     //因为如果写完之后IO被置了低电平，开漏输出模式下读取的时候会有影响，最好先拉高，再读取
}

/*----------------------------------
**函数名称：DS1302_WriteData
**功能描述：DS1302写数据
**参数说明：addr：为要写入的地址
			data_：为要写入的数据
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void DS1302_WriteData(u8 addr,u8 data_)
{	
	DS1302_CE = 0;		DS1302_delay_us(2);	
	DS1302_SCK = 0;		DS1302_delay_us(2);	
	DS1302_CE = 1;		DS1302_delay_us(2);	//使能片选信号
	
	DS1302_WriteByte((addr<<1)|0x80);	//方便后面写入,转化之后是地址寄存器的值，
	DS1302_WriteByte(data_);
	DS1302_CE = 0;		DS1302_delay_us(2);//传送数据结束，失能片选
	DS1302_SCK = 0;     DS1302_delay_us(2);//拉低，准备下一次写数据
}

/*----------------------------------
**函数名称：DS1302_ReadByte
**功能描述：DS1302读取一个字节，上升沿读取
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static u8 DS1302_ReadByte(void)
{
	u8 i = 0;
	u8 data_ = 0;
	
	//DS1302_DAT_INPUT();  //因为上面已经把端口设置为开漏，电路外部接了山拉电阻，可以不切换输入输出模式，直接使用。
	
	DS1302_SCK = 0;
	DS1302_delay_us(3);
	
	for(i=0;i<7;i++)   //这里发现设为8的话输出数据不对，很乱
	{
		if((DS1302_DATIN) == 1) 
		{
			data_ = data_ | 0x80;	//低位在前，逐位读取,刚开始不对，估计是这个的问题
		}
		data_>>= 1;
		DS1302_delay_us(3);
		
		DS1302_SCK = 1;  //因为刚开始SCK是低电平，这里拉高作为第一个上升沿
		DS1302_delay_us(3);
		DS1302_SCK = 0;
		DS1302_delay_us(3);
	}
	 return (data_);
}

/*----------------------------------
**函数名称：DS1302_ReadData
**功能描述：DS1302读取数据
**参数说明：addr：为需要读取数据的地址
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static u8 DS1302_ReadData(u8 addr)
{
	u8 data_ = 0;

	DS1302_CE = 0;		DS1302_delay_us(2);
	DS1302_SCK = 0;		DS1302_delay_us(2);
	DS1302_CE = 1;		DS1302_delay_us(2);   //读写操作时CE必须为高，切在SCK为低时改变
	
	DS1302_WriteByte((addr<<1)|0x81);   //写入读时间的命令
	data_ = DS1302_ReadByte(); 
	
	DS1302_SCK = 1;  	DS1302_delay_us(2);
	DS1302_CE = 0;	    DS1302_delay_us(2);
	DS1302_DATOUT = 0;  DS1302_delay_us(3);  //这里很多人说需要拉低，但是我发现去掉这个也可以显示啊，不过为了保险，还是加上。
	DS1302_DATOUT = 1;  DS1302_delay_us(2);

	return data_;
}

/*----------------------------------
**函数名称：DS1302_Init
**功能描述：DS1302总初始化，在此可以修改写入时间
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void DS1302_Init(void)
{
//	int i = 0;
	
	DS1302_GPIO_Init();  //端口初始化
	
	DS1302_CE = 0;  DS1302_delay_us(2);
	DS1302_SCK = 0; DS1302_delay_us(2);  
	
//	i  = DS1302_ReadData(0x00);  //读取秒寄存器,
/*
**  如果需要修改时间，就把这个if判断去掉，直接撤销写保护，然后写入新的时间。
**  修改完时间之后，再次打开这个if判断，在下载一遍程序即可。
*/
//  if((i & 0x80) != 0)//通过判断秒寄存器是否还有数据来决定下次上电的时候是否初始化时间，就是掉电保护
//	{
//	 	DS1302_WriteData(7,0x00); //撤销写保护，允许写入数据,0x8e,0x00

//		for(i = 0;i<7;i++)
//		{
//			DS1302_WriteData(i,init_time[i]); 
//		}
//	}
//	 	DS1302_WriteData(7,0x80);//打开写保护功能，防止复位时时间被重置
}

/*----------------------------------
**函数名称：adjust_real_time
**功能描述：在主函数中调用，当蓝牙发来时间，调用这个函数进行写入、
**参数说明：无
**作者：Andrew
**日期：2018.5.1
-----------------------------------*/
void adjust_real_time(void)
{
	unsigned char i;	
	
	DS1302_WriteData(7,0x00); //撤销写保护，允许写入数据,0x8e,0x00
	
	for(i = 0;i<7;i++)
	{
		DS1302_WriteData(i,init_time[i]); 
	}
	
	DS1302_WriteData(7,0x80);//打开写保护功能，防止复位时时间被重置
}

/*----------------------------------
**函数名称：DS1302_ReadTime
**功能描述：DS1302读取时间到缓冲区
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void DS1302_ReadTime(void)
{
	  u8 i;
	  for(i = 0;i<7;i++)
	  {
	     init_time[i] = DS1302_ReadData(i);
	  }
}

//内部延时,修改i达到不同的延时效果。我仿真的话是i = 80大概延时1us
//但是这里我设置为5的时候DS1302也是可以正常读取的。
//主函数对时钟不做处理的话，时钟频率默认72M，板子上要是8M的才行。到实际中运行时也是72M
static void DS1302_delay_us(u16 time)
{    
   u16 i = 0;  
   while(time--)
   {
      i = 60;  //自己定义
      while(i--);
   }
}

/*----------------------------------
**函数名称：Display_Real_Time
**功能描述：在OLED上显示实时时间
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void Display_Real_Time(void)
{
	DS1302_ReadTime();   //先获取时间到缓冲区
	
	//BCD码转换ASCII码
	TIME.year =  ((init_time[6]&0x70)>>4)*10 + (init_time[6]&0x0f); //高三位加低四位
	TIME.month = ((init_time[4]&0x70)>>4)*10 + (init_time[4]&0x0f);
	TIME.date =  ((init_time[3]&0x70)>>4)*10 + (init_time[3]&0x0f);
	TIME.week =  ((init_time[5]&0x70)>>4)*10 + (init_time[5]&0x0f);
	TIME.hour =  ((init_time[2]&0x70)>>4)*10 + (init_time[2]&0x0f);
	TIME.minute = ((init_time[1]&0x70)>>4)*10 + (init_time[1]&0x0f);
	TIME.second = ((init_time[0]&0x70)>>4)*10 + (init_time[0]&0x0f);
	
//年月日用第二屏幕显示	
//	OLED_ShowNum(0,0,20,2,8);  //20**年
//	OLED_ShowNum(16,0,TIME.year,2,16);  //我觉得几几年应该不用显示了吧，谁不知道现在是那一年啊，主要是日期	
//	OLED_ShowChar(24,0,".");
//	OLED_ShowNum(0,0,TIME.month,2,16);
//	OLED_ShowNum(24,0,TIME.date,2,16);
	
	OLED_Show_Big_Time(16+0,2,TIME.hour,2); //在第二行开始显示，开头留16个元素
	
	OLED_Show_16X32_Num(16+2*16,2,10);  //冒号在第十个，直接用这个显示吧，不专门写一个函数了

	OLED_Show_Big_Time(16+3*16,2,TIME.minute,2);//显示分钟
	
//	OLED_ShowNum(18+5*16,4,TIME.second,2,16);//显示秒，就用小字体显示在右下角
	
}


