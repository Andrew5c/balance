/*
** ���ļ�����������Ҫ�ĺ���
** ���ȱ��澻�غ�500g�����������������ϵ��
** �������ʵ�ֵ�ȥƤ�أ���ʵ�����Ȱѱ���˵���̷��ڳ����棬�Ƴ����̵ľ��أ�
	���������ϵ�����µ� empty_cow �����У�Ȼ���������ʱ�򣬼�ȥ��Ƥ�ؾ���
	������̵�������
** ��û�п���ȥƤ��֮ǰ����ʵ�����л��Ǽ�ȥ��һ��Ƥ�صģ�ֻ�������Ƥ����У׼����
	ʹ���ڿ�����ʱ����ʾ 0g��
	
** ���ص�ʱ��ע����ж�
*/

#include "main.h"

//�Ŵ�1000���ĳ���ϵ������ֱ�ߵ�б��
float Weight_Coe_100g = 21.75010;

//float Weight_Coe_200g = Weight_Coe_100g + 0.0001;
//float Weight_Coe_300g = Weight_Coe_100g + 0.0002;
//float Weight_Coe_400g = Weight_Coe_100g + 0.0004;
//float Weight_Coe_500g = Weight_Coe_100g + 0.0004;


//�ϵ�ʱ��Ƥ�أ��ڰ���ȥƤ������ʱ����ֵ�ᱻ�޸�
float empty_coe = 0;

extern float now_weight;
extern unsigned char start_dynamic_calibration ;	 //��ʼ��̬�궨��־
extern unsigned char ensure_dynamic_calibration ; //ȷ�ϱ궨������


static float Choose_Coe(float adc_data);


/*---------------------------------
** ���ƣ�Get_Weight_Coe
** ���ܣ�����ʱ��ȡ��ǰ��������Ϊ���������Ļ�׼��
** ��ڣ���
** ���ڣ���
** ˵����������������Ϊ����ϵ���ĳ�ʼ��
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
** ���ƣ�Get_Weight
** ���ܣ����ݶ�ȡ��CS1237 ADCֵ����ʵ������
** ��ڣ���
** ���ڣ���������λ��g
** ˵������������ʵʱ��������
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
	
	//��̬�궨�����У�����Ļ����ʵʱ��adcԭʼ��ֵ��
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
		//ȷ����ǰ���ݼ��ɱ���
		else if(ensure_dynamic_calibration == 1)
		{
			ensure_dynamic_calibration = 0;
			x_value[adc_num++] = (float)temp_adc;
			num_count = adc_num;		//��ʱ�������ݸ���
		}

	}
	else
	{
		send_time = 0;
		adc_num = 0;
	}

	//��ȡADC����ת��Ϊ��λ����
	dat = filter_eliminate_dithering(temp_adc);	
	//ѡ�����ϵ��
	dat *= Choose_Coe(dat);
	//dat *= Weight_Coe_100g;
	dat -= empty_coe;
	
	if(dat < 0)
		dat = 0;
	
	//ǰ��K�Ŵ���1000���������ȳ�10��Ϊ��С����������������
	dat_2 = dat / 10;
	//dat_2 /= 100;
	if(dat_2 % 100 > 5)
	{
		dat_2 /= 10;
		dat_2 += 1;
	}
	else
		dat_2 /= 10;
	//ת��ΪС�����
	dat_3 = (float)dat_2 / 10.00f;
	return dat_3;

}

/*---------------------------------
** ���ƣ�remove_empty_weight
** ���ܣ�ȥƤ�ع��ܣ���ʵ�������¸���һ�� empty_coe ����
** ��ڣ���
** ���ڣ���
** ˵������ĿҪ��ȥƤ����������100g
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
	//ȥƤ��������100g����������
	else
		Beep_Warning(2,50);
}

//���ݶ�ȡ��ADC���ֶν���ѡ�����ϵ��
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


