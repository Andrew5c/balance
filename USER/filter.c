/*
** ���õļ���һ�������˲��㷨
*/

#include "main.h"

#define N 6 


/*
** �����˲���
** ������������ÿ�β���ֵ�뵱ǰ��Чֵ�Ƚϣ�
�������ֵ����ǰ��Чֵ������������㣬�������ֵ<>��ǰ��Чֵ���������+1�����жϼ������Ƿ�>=����N(���)��������������,�򽫱���ֵ�滻��ǰ��Чֵ,���������
** �ŵ㣺
		���ڱ仯�����ı�������нϺõ��˲�Ч��,�ɱ������ٽ�ֵ�����������ķ�����/����������ʾ������ֵ������
** ȱ�㣺
		���ڿ��ٱ仯�Ĳ������ˣ�����ڼ������������һ�β�������ֵǡ���Ǹ���ֵ,��Ὣ����ֵ������Чֵ����ϵͳ
*/
long int filter_eliminate_dithering(long int value)  
{  
   long int count = 0;  
   long int new_value = 0;  
	
   new_value = CS1237_Read_18bit_ADC(); 
	
   while (value != new_value)  
   {  
      count++;  
      if (count >= N)   
			return new_value;  
      new_value = CS1237_Read_18bit_ADC();  
   }  
	
   return value;      
} 

/*
** ��λֵƽ���˲��������������ƽ���˲�����
** ����������
		�൱����λֵ�˲�+����ƽ���˲�����������N�����ݣ�ȥ��һ�����ֵ��һ����Сֵ��Ȼ�����N-2�����ݵ�����ƽ��ֵ��
** Nֵ��ѡȡ��3-14
** �ŵ㣺
		�ں��������˲������ŵ�
		����żȻ���ֵ������Ը��ţ������������������������Ĳ���ֵƫ�
** ȱ�㣺
		�����ٶȽ��������㷨ƽ���˲�һ�����˷�RAM��
*/
/*
char filter_median_value(void)  
{  
   char count,i,j;  
   char value_buf[N];  
   int  sum=0,temp=0;  
   for  (count=0;count<N;count++)  
   {  
      value_buf[count] = get_ad();  
      Delay_ms(1);  
   }  
   for (j=0;j<N-1;j++)  
   {  
      for (i=0;i<N-j;i++)  
      {  
         if ( value_buf[i]>value_buf[i+1] )  
         {  
            temp = value_buf[i];  
            value_buf[i] = value_buf[i+1];   
             value_buf[i+1] = temp;  
         }  
      }  
   }  
   for(count=1;count<N-1;count++)  
      sum += value[count];  
   return (char)(sum/(N-2));  
}  
*/

