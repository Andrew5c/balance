
#include "main.h"

/*
** 按键上面不同的按键变量
A		B		C		D		*		#
10		11		12		13		14		15
*/

static void mode_Out(void);
static void mode_IPU(void);


/*
** 4*4矩阵按键需要8个端口进行循环扫描
** 列：PA0-PA3 推挽输出
** 行：PA4-PA7 上拉输入
*/
void Key_Board_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1、使能GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//2、GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//引脚拉高
	GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	
}

static void mode_Out(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


static void mode_IPU(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入 ，悬空时处于高电平
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*
** 可以在定时器里面定时循环这个函数，循环扫描按键有没有按下
** 根据按下不同的按键，返回不同的值
** 返回16说明没有按键按下
*/
unsigned char Key_Board_Scan(void)
{
	//根据这个变量的不同值来确定具体的按键
	unsigned char key_value = 0;
	
	if((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F)
	{
		//消抖
		Delay_ms(20);
		if((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F)
		{
			//循环给列的端口置高电平，检测行的电平状态
			//下面的情况全部颠倒，需要重写
			
			//第一列
			GPIO_SetBits(GPIOA, GPIO_Pin_0);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x81: key_value = 1; break;
				case 0x41: key_value = 4; break;
				case 0x21: key_value = 7; break;
				case 0x11: key_value = 14; break;
				default:break;
			}
			
			//第二列
			GPIO_SetBits(GPIOA, GPIO_Pin_1);
			GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x82: key_value = 2; break;
				case 0x42: key_value = 5; break;
				case 0x22: key_value = 8; break;
				case 0x12: key_value = 0; break;
				default:break;
			}
			
			//第三列
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x84: key_value = 3; break;
				case 0x44: key_value = 6; break;
				case 0x24: key_value = 9; break;
				case 0x04: key_value = 15; break;
				default:break;
			}
			
			//第四列
			GPIO_SetBits(GPIOA, GPIO_Pin_3);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
			switch((GPIO_ReadInputData(GPIOA)) & (0XFF))
			{
				case 0x88: key_value = 10; break;
				case 0x48: key_value = 11; break;
				case 0x28: key_value = 12; break;
				case 0x08: key_value = 13; break;
				default:break;
			}
							
			//检测完毕，重置端口状态
			GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
			GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
			
			//等待按键抬起，程序卡死在这
			//测试读出OXFFFF
			while((GPIO_ReadInputData(GPIOA) & 0XFF) != 0X0F) ;
			
			//返回按键值
			return key_value;
		}
		//没有达到消抖的时间，可认为是抖动引起
		else
			return 16;

	}
	//没有按键按下
	else
		return 16;

}


