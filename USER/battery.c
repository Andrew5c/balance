
#include "word.h"
#include "main.h"


/*----------------------------------
**函数名称：OLED_Show_Battery
**功能描述：屏幕右上角根据电压显示小电池
**参数说明：X,Y:为坐标
			n：为电池模型在取模库中的位置序号(0-13)
**作者：Andrew
**日期：2018.1.26
-----------------------------------*/
void OLED_Show_Battery(u8 x,u8 y,u8 n)
{
	u8 t = 0;
	
	OLED_Coord(x,y);
    for(t=0;t<16;t++)  //每行16个元素，一个电池模型取模之后只有一行16进制数据
	{
		SPI_Write(BMP[n][t],OLED_Data);
    }
}

/*----------------------------------
**函数名称：OLED_Show_RealTime_Battery
**功能描述：根据ADC采样的电池电压显示不同的电池模型，从而达到显示实时电量的效果
**参数说明：X,Y:为坐标
			n：为电池模型在取模库中的位置序号(0-13)
**作者：Andrew
**日期：2018.1.26
-----------------------------------*/
void OLED_Show_RealTime_Battery(u8 x,u8 y)
{
	float ADC_Val = 0;
	unsigned int ADC_Int = 0;
	
	ADC_Val = ADC_cal_aveg();			 //获取ADC电压值
	ADC_Int = (int)(ADC_Val / 0.24);  //这个0.24为 3.3/14 得来的数，就是每一级的步长，然后采样电压值除以步长就是级数
	OLED_Show_Battery(x,y,ADC_Int);   //上面的级数直接在这里面使用，配合电池模型顺序即可。
	
}



