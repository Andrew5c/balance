/*
** 常用的几种一阶数字滤波算法
*/

#include "main.h"

#define N 6 


/*
** 消抖滤波法
** 方法解析：将每次采样值与当前有效值比较：
如果采样值＝当前有效值，则计数器清零，如果采样值<>当前有效值，则计数器+1，并判断计数器是否>=上限N(溢出)，如果计数器溢出,则将本次值替换当前有效值,并清计数器
** 优点：
		对于变化缓慢的被测参数有较好的滤波效果,可避免在临界值附近控制器的反复开/关跳动或显示器上数值抖动。
** 缺点：
		对于快速变化的参数不宜，如果在计数器溢出的那一次采样到的值恰好是干扰值,则会将干扰值当作有效值导入系统
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
** 中位值平均滤波法（防脉冲干扰平均滤波法）
** 方法解析：
		相当于中位值滤波+算术平均滤波，连续采样N个数据，去掉一个最大值和一个最小值，然后计算N-2个数据的算术平均值。
** N值的选取：3-14
** 优点：
		融合了两种滤波法的优点
		对于偶然出现的脉冲性干扰，可消除由于脉冲干扰所引起的采样值偏差。
** 缺点：
		测量速度较慢，和算法平均滤波一样，浪费RAM。
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

