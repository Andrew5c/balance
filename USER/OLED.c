
//SPI方式对OLED显示屏进行操作
//3.3V的电源即可

#include "main.h"
#include "word.h"//包含字模头文件

//在内部调用函数
static u32 oled_pow(u8 m,u8 n);
static void OLED_GPIO_INIT(void);

/*----------------------------------
**函数名称：OLED_GPIO_INIT
**功能描述：OLED显示屏引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
static void OLED_GPIO_INIT(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;  
      
    //开启GPIOD的时钟  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
    //设置GPIO的基本参数  
    GPIO_InitStruct.GPIO_Pin = OLED_CS_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_D0_PIN | OLED_D1_PIN;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz  
      
    GPIO_Init(OLED_PORT, &GPIO_InitStruct);  
      
    GPIO_SetBits(OLED_PORT, OLED_CS_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_D0_PIN | OLED_D1_PIN);  
}

/*----------------------------------
**函数名称：SPI_Write
**功能描述：SPI写操作
**参数说明：data：需要写入的数据
						Mode：为选择写数据还是写命令
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void SPI_Write(u8 data, u8 Mode)  
{      
    u8 i = 0; 

    if(Mode)  
    {  
        OLED_DC(1);        //DC引脚输入高，表示写数据  
    }  
    else  
    {  
        OLED_DC(0);        //DC引脚输入低，表示写命令  
    }  
    OLED_CS(0);            //CS引脚输入低，片选使能  
    for(i = 0; i < 8; i++)  
    {  
        OLED_D0(0);        //D0引脚输入低  
        if(data&0x80)    //判断传输的数据最高位为1还是0  
        {  
            OLED_D1(1);    //D1引脚输入高  
        }  
        else  
        {  
            OLED_D1(0);    //D1引脚输入低  
        }  
        OLED_D0(1);        //D1引脚输入高  
        data<<=1;        //将数据左移一位  
    }  
    OLED_DC(1);            //DC引脚输入低  
    OLED_CS(1);            //CS引脚输入高，片选失能  
}  

/*----------------------------------
**函数名称：OLED_Coord
**功能描述：设置OLED显示坐标
**参数说明：X:光标的横坐标（0-127）
						Y：纵坐标（0-7）
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Coord(u8 x, u8 y)  
{  	
	 SPI_Write((0xb0 + y) ,OLED_Order); 
    SPI_Write((((x & 0xf0)>>4) | 0x10), OLED_Order);//高4位  
    SPI_Write((x & 0x0f)|0x01, OLED_Order);//低4位  
}  

/*----------------------------------
**函数名称：OLED_Clear
**功能描述：OLED清屏
**参数说明：无
**作者：Andrew
**日期：2018.1.24
//清屏，一开始这里写错了，把写命令写成了写数据，导致清屏不正确，发现屏幕上有很多噪点，说明没有清屏成功。
-----------------------------------*/
void OLED_Clear(void)  
{  
    u8 i = 0, j = 0;  

    for(i = 0; i < 8; i++)  
    {  
		SPI_Write(0xb0 + i,OLED_Order);
		SPI_Write(0x00,OLED_Order);
		SPI_Write(0x10,OLED_Order);
        for(j = 0; j < 128; j++)  
        {  
            SPI_Write(0x00, OLED_Data);  
        }  
    }  
}  

/*----------------------------------
**函数名称：OLED_Display_Off
**功能描述：关闭OLED显示功能，不能在显示字符
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Display_Off(void)  
{
	 SPI_Write(0x8D,OLED_Order);
    SPI_Write(0x10,OLED_Order);
    SPI_Write(0xAE,OLED_Order);
}  

/*----------------------------------
**函数名称：OLED_Display_On
**功能描述：开OLED显示
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Display_On(void)
{  
    //电荷泵设置（初始化时必须打开，否则看不到显示）
    SPI_Write(0x8D, OLED_Order);  
    SPI_Write(0x14, OLED_Order);//bit2   0：关闭        1：打开  
    SPI_Write(0xAF, OLED_Order);//0xAF:开显示      
}  

/*----------------------------------
**函数名称：OLED_Init
**功能描述：OLED各种初始化，也是各种寄存器初始值设置
**参数说明：无
**作者：Andrew
**日期：2018.1.24
**说明：
	2018.5.19：不要在OLED初始化之前就调用其他的显示函数，不然程序会卡死在初始化函数中
-----------------------------------*/
void OLED_Init(void)  
{  
    OLED_GPIO_INIT();  //端口初始化
      
    OLED_RST(1);     
    Delay_ms(100);   
    OLED_RST(0);  
    Delay_ms(100);  
    OLED_RST(1);
    
    SPI_Write(0xAE, OLED_Order);//0xAE:关显示     

    SPI_Write(0x00, OLED_Order);//设置低列地址  
    SPI_Write(0x10, OLED_Order);//设置高列地址  
      
    //设置行显示的开始地址(0-63)  
    //40-47: (01xxxxx)  
    SPI_Write(0x40, OLED_Order);
      
    //设置对比度  
    SPI_Write(0x81, OLED_Order);
    SPI_Write(0xff, OLED_Order);//这个值越大，屏幕越亮(和上条指令一起使用)(0x00-0xff)  
      
    SPI_Write(0xA1, OLED_Order);//0xA1: 左右反置，  0xA0: 正常显示（默认0xA0）  
    SPI_Write(0xC8, OLED_Order);//0xC8: 上下反置，  0xC0: 正常显示（默认0xC0）  
      
    //0xA6: 表示正常显示（在面板上1表示点亮，0表示不亮）  
    //0xA7: 表示逆显示（在面板上0表示点亮，1表示不亮）  
    SPI_Write(0xA6, OLED_Order);  
      
    SPI_Write(0xA8, OLED_Order);//设置多路复用率（1-64）  
    SPI_Write(0x3F, OLED_Order);//（0x01-0x3f）(默认为3f)  
      
      
    //设置显示抵消移位映射内存计数器  
    SPI_Write(0xD3, OLED_Order);  
    SPI_Write(0x00, OLED_Order);//（0x00-0x3f）(默认为0x00)  
      
    //设置显示时钟分频因子/振荡器频率  
    SPI_Write(0xD5, OLED_Order);  
    //低4位定义显示时钟(屏幕的刷新时间)（默认：0000）分频因子= [3:0]+1  
    //高4位定义振荡器频率（默认：1000）  
    SPI_Write(0x80, OLED_Order);//  
      
    //时钟预充电周期  
    SPI_Write(0xD9, OLED_Order);  
    SPI_Write(0xF1, OLED_Order);//[3:0],PHASE 1;   [7:4] PHASE 2  
      
    //设置COM硬件应脚配置  
    SPI_Write(0xDA, OLED_Order);  
    SPI_Write(0x12, OLED_Order);//[5:4]  默认：01  
      
    SPI_Write(0xDB, OLED_Order);//  
    SPI_Write(0x40, OLED_Order);//  
      
    //设置内存寻址方式  
    SPI_Write(0x20, OLED_Order);  
    //00: 表示水平寻址方式  
    //01: 表示垂直寻址方式  
    //10: 表示页寻址方式（默认方式）  
    SPI_Write(0x02, OLED_Order);//      
      
    //电荷泵设置（初始化时必须打开，否则看不到显示）  
    SPI_Write(0x8D, OLED_Order);  
    SPI_Write(0x14, OLED_Order);//bit2   0：关闭        1：打开  
      
    //设置是否全部显示 0xA4: 禁止全部显示  
    SPI_Write(0xA4, OLED_Order);  
  
    //0xA6: 表示正常显示（在面板上1表示点亮，0表示不亮）  
    //0xA7: 表示逆显示（在面板上0表示点亮，1表示不亮）  
    SPI_Write(0xA6, OLED_Order);//  
      
    SPI_Write(0xAF, OLED_Order);//0xAF:开显示     
	 SPI_Write(0xAF, OLED_Order); //不知道为什么要写两次
	
    OLED_Clear();
	 OLED_Coord(0,0);
}  

/*----------------------------------
**函数名称：OLED_ShowChinese
**功能描述：光标处显示汉字
**参数说明：X,Y为字体开始坐标
			Chinese为要显示汉字取模后的码表在字库里的序号
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_ShowChinese(u8 x, u8 y, u8 chinese)  
{  
	u8 t,adder=0;
	
	OLED_Coord(x,y);

    for(t=0;t<16;t++)  //每行16个元素，一个字需要两行字符串
	{
		SPI_Write(Hzk[2*chinese][t],OLED_Data);
		adder+=1;
     }	
	
	OLED_Coord(x,y+1);
	 
    for(t=0;t<16;t++)
	{	
		SPI_Write(Hzk[2*chinese+1][t],OLED_Data);
		adder+=1;
      }
} 

/*----------------------------------
**函数名称：OLED_ShowChar
**功能描述：光标处显示一个字符
**参数说明：X,Y为坐标
			chr为要显示的字符，用单引号表示
			改变前面SIZE的数值可以选择需要字符的字体大小，字库有两种字体供选择
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_ShowChar(u8 x, u8 y, u8 chr)  
{  
	unsigned char c=0,   i=0;	
	
	c = chr - ' ';    //得到偏移后的值
	
		if(x > Max_Column - 1)
			{x=0;y=y+2;}
			
		if(SIZE ==16)  //8*16字符
		{
			OLED_Coord(x,y);
			
			for(i=0;i<8;i++)
				SPI_Write(F8X16[c*16+i],OLED_Data);
			
			OLED_Coord(x,y+1);
			
			for(i=0;i<8;i++)
				SPI_Write(F8X16[c*16+i+8],OLED_Data);
		}
		else    //6*8字符
		{	
			OLED_Coord(x,y+1);
			
			for(i=0;i<6;i++)
				SPI_Write(F6x8[c][i],OLED_Data);
			
		}
}  
/*----------------------------------
**函数名称：OLED_Show_String
**功能描述：光标处显示字符串，字符串可以用数组表示，unsigned char string_2[] = {"THIS IS A TEST  "};
**参数说明：X,Y为坐标
			* chr：字符串首地址
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Show_String(u8 x, u8 y, u8 *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j]);

		x+= 8 ;

		if(x>120){x=0;y+=2;}  //自动换行写

		j++;
	}
}

/*----------------------------------
**函数名称：oled_pow
**功能描述：取幂次方函数
**参数说明：m为底，n为幂，m^n
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
static u32 oled_pow(u8 m,u8 n)
{
	u32 result = 1; 
	while(n--)result*=m;    
	return result;
}

/*----------------------------------
**函数名称：OLED_ShowNum
**功能描述：显示数字
**参数说明：X,Y :为数字起始坐标 
			num：要显示的数字
			len：为数字的位数
			size：为需要数字的字体大小，其实改变的就是数字之间的距离
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t = 0, temp = 0;
//	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;   //取出每一位数
		OLED_ShowChar(x+(size/2)*t,y,temp+'0');
		
//下面这一段就是把为0的数字不显示，比如03，加了下面这一段之后就只显示3
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size/2)*t,y,' ');
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
} 


//---------自定义显示大字体数字--------

/*----------------------------------
**函数名称：OLED_Show_16X32_Num
**功能描述：显示16*32字体的数字，锁屏显示时间时用
**参数说明：X,Y:为数字起始坐标
			num：为该数字在 16X32 字模中的序号
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Show_16X32_Num(u8 x, u8 y, u8 num)  
{  
	u8 t,adder=0;
	
	//第一行
	OLED_Coord(x,y);
    for(t=0;t<16;t++)  //每行16个元素，一个字需要两行字符串
	{
		SPI_Write(F16X32[4*num + 0][t],OLED_Data);
		adder+=1;
    }
	//第二行
	OLED_Coord(x,y + 1); 
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 1][t],OLED_Data);
		adder+=1;
    }	
	//第三行
	OLED_Coord(x,y + 2);
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 2][t],OLED_Data);
		adder+=1;
    }	
	//第四行
	OLED_Coord(x,y + 3);
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 3][t],OLED_Data);
		adder+=1;
    }
	
} 
/*----------------------------------
**函数名称：OLED_Show_Big_Time
**功能描述：调用上面的函数显示大字体时间
**参数说明：X,Y:为坐标
			num：为要显示的数字
			len：为数字的位数
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void OLED_Show_Big_Time(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t = 0, temp = 0;
	
	for(t=0;t<len;t++)
	{
		temp = (num/oled_pow(10,len-t-1)) % 10;   //取出每一位数
		OLED_Show_16X32_Num(x+(t*16),y,temp);
	}
}




/*
** 清除小数点后一位之后的其余数据，防止有上次一数据的残留。
** 用于动态显示浮点数,并且不会因为清除全部数据而闪烁
** 但是注意清除的数据不要数组越界
*/
void Clear_Left_Num(unsigned char *num_string)
{
	while(*num_string != '.')
		num_string++;
	//一位小数点后面的数据用空格刷新
	*(num_string+2) = ' ';
	*(num_string+3) = ' ';
	*(num_string+4) = ' ';
}

/*
** 这个效果比较好
** 第二版动态刷新浮点数消隐函数
** sprintf 函数在格式化之后会在结果字符串的后面添加一个 \0 ，
** 可以以这个 \0 为标志，后面的残留数据全部清除
*/
void Clear_Left_Num2(unsigned char *num_string)
{
	while(*num_string != '\0')
		num_string++;
	*(num_string) = ' ';
	*(num_string+1) = ' ';
	*(num_string+2) = ' ';
//	*(num_string+3) = ' ';
}

//显示单价、重量、金额
void OLED_Show_Tips(void)
{
	//迷你电子秤
	OLED_ShowChinese(24,0,6);
	OLED_ShowChinese(24+16,0,7);
	OLED_ShowChinese(24+32,0,8);
	OLED_ShowChinese(24+48,0,9);
	OLED_ShowChinese(24+64,0,10);
	//重量
	OLED_ShowChinese(0,2,0);
	OLED_ShowChinese(16,2,1);
	OLED_ShowChar(33,2,':');
	//单价
	OLED_ShowChinese(0,4,2);
	OLED_ShowChinese(16,4,3);
	OLED_ShowChar(33,4,':');
	//金额
	OLED_ShowChinese(0,6,4);
	OLED_ShowChinese(16,6,5);
	OLED_ShowChar(33,6,':');
}

