
#include <math.h>
#include "least_square.h"

//ʵ���������ݸ���
unsigned char num_count = 0;

//ԭʼ����
float x_value[MAX_NUM] = {0};
float y_value[MAX_NUM] = {50,100,150,200,250,300,350,400,450,500};


/*
** ˵������С���˷���������ϡ� 
** ԭ�������������������Ϣ����ϳ�һ��ֱ�ߣ�ʹ���е����ֱ�ߵĲ��ƽ������С�� 
** ȱ�㣺���������̶Ⱥܵ͡����Բ��ü�Ȩ��С���˷��� 
** ����ֵ�����ֱ�ߵ�б��K 
** ʱ�䣺2018.7.7 
*/
float Least_Square(void)
{
	int i;
	float a0,a1,a2,temp,temp0,temp1,temp2;
	float sy=0,sx=0,sxx=0,syy=0,sxy=0,sxxy=0,sxxx=0,sxxxx=0;	//������ر���
	
	for(i=0; i<num_count; i++)
	{
		sx+=x_value[i];//����xi�ĺ� 
		sy+=y_value[i];//����yi�ĺ� 
		sxx+=x_value[i]*x_value[i];//����xi��ƽ���ĺ� 
		sxxx+=pow(x_value[i],3);//����xi�������ĺ� 
		sxxxx+=pow(x_value[i],4);//����xi��4�η��ĺ� 
		sxy+=x_value[i]*y_value[i];//����xi��yi�ĵĺ� 
		sxxy+=x_value[i]*x_value[i]*y_value[i];//����xiƽ����yi�ĺ� 
	}
	
	temp=num_count*sxx-sx*sx;//���̵�ϵ������ʽ
	//temp0=sy*sxx-sx*sxy;
	temp1=num_count*sxy-sy*sx;
	
	//a0=temp0/temp;	//�ؾ�
	a1=temp1/temp;	//б��
	
	return a1;
}


