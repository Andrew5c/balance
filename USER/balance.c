/*
** 此文件包含称重需要的函数
** 首先保存净重和500g的重量，计算出称重系数
** 这个里面实现的去皮重，其实就是先把比如说托盘放在称上面，称出托盘的净重，
	把这个净重系数更新到 empty_cow 变量中，然后后面计算的时候，减去的皮重就是
	这个托盘的重量。
** 在没有开启去皮重之前，其实程序中还是减去了一个皮重的，只不过这个皮重是校准量，
	使称在开机的时候，显示 0g。
	
** 称重的时候注意关中断
*/

#include "main.h"

//放大1000倍的称重系数，即直线的斜率
float Weight_Coe_100g = 21.75010;

//float Weight_Coe_200g = Weight_Coe_100g + 0.0001;
//float Weight_Coe_300g = Weight_Coe_100g + 0.0002;
//float Weight_Coe_400g = Weight_Coe_100g + 0.0004;
//float Weight_Coe_500g = Weight_Coe_100g + 0.0004;


//上电时的皮重，在按下去皮按键的时候，数值会被修改
float empty_coe = 0;

extern float now_weight;
extern unsigned char start_dynamic_calibration ;	 //开始动态标定标志
extern unsigned char ensure_dynamic_calibration ; //确认标定的数据


static float Choose_Coe(float adc_data);


/*---------------------------------
** 名称：Get_Weight_Coe
** 功能：开机时读取当前重量，作为后面重量的基准点
** 入口：无
** 出口：无
** 说明：在主函数中作为称重系数的初始化
----------------------------------*/
void Get_Weight_Coe(void)
{
	unsigned char filter_num = 10;
	unsigned long int temp = 0;
	
	while(filter_num--)
	{
		temp += CS1237_Read_18bit_ADC();
	}	
	
	temp = temp / 10;
	empty_coe = Weight_Coe_100g * temp;
}

/*---------------------------------
** 名称：Get_Weight
** 功能：根据读取的CS1237 ADC值计算实际重量
** 入口：无
** 出口：重量，单位：g
** 说明：主函数中实时计算重量
----------------------------------*/
float Get_Weight(void)
{
	float dat = 0;
	unsigned long int dat_2 = 0;
	float dat_3 = 0;
	char string_buf[30];
	static unsigned char adc_num = 0;
	static unsigned char send_time = 0;
	
	
	long int temp_adc = CS1237_Read_18bit_ADC();
	
	//动态标定过程中，向屏幕发送实时的adc原始数值。
	if(start_dynamic_calibration == 1)
	{
		send_time++;
		if(send_time == 4)
		{
			send_time = 0;
			sprintf(string_buf,"t24.txt=\"%ld\"",temp_adc);
			HMI_Send_String(string_buf);
			Delay_ms(50);
		}
		//确定当前数据即可保存
		else if(ensure_dynamic_calibration == 1)
		{
			ensure_dynamic_calibration = 0;
			x_value[adc_num++] = (float)temp_adc;
			num_count = adc_num;		//及时更新数据个数
		}

	}
	else
	{
		send_time = 0;
		adc_num = 0;
	}

	//读取ADC，并转化为单位：克
	dat = filter_eliminate_dithering(temp_adc);	
	//选择称重系数
	dat *= Choose_Coe(dat);
	//dat *= Weight_Coe_100g;
	dat -= empty_coe;
	
	if(dat < 0)
		dat = 0;
	
	//前面K放大了1000倍，这里先除10，为了小数点后面的四舍五入
	dat_2 = dat / 10;
	//dat_2 /= 100;
	if(dat_2 % 100 > 5)
	{
		dat_2 /= 10;
		dat_2 += 1;
	}
	else
		dat_2 /= 10;
	//转换为小数输出
	dat_3 = (float)dat_2 / 10.00f;
	return dat_3;

}

/*---------------------------------
** 名称：remove_empty_weight
** 功能：去皮重功能，其实就是重新更新一下 empty_coe 参数
** 入口：无
** 出口：无
** 说明：题目要求去皮重量不超过100g
----------------------------------*/
void remove_empty_weight(void)
{
	unsigned char remove_filter_num =  2;
	unsigned long int temp = 0;
	
	if(now_weight < 100)
	{
		while(remove_filter_num--)
		{
			temp += CS1237_Read_18bit_ADC();
		}	
		temp = temp / 2;
		empty_coe = Weight_Coe_100g * temp;
	}
	//去皮重量超过100g，给出警告
	else
		Beep_Warning(2,50);
}

//根据读取的ADC，分段进行选择称重系数
static float Choose_Coe(float adc_data)
{
	if(adc_data <= 1028225)					//<100g
		return Weight_Coe_100g;
	else if(adc_data <= 1032814)			//<200g
		return (Weight_Coe_100g + 0.00020);
	else if(adc_data <= 1036750)			//<300g
		return (Weight_Coe_100g + 0.00035);
	else if(adc_data <= 1041000)			//<400g
		return (Weight_Coe_100g + 0.00095);
	else
		return (Weight_Coe_100g + 0.00130);	//<500g

}


