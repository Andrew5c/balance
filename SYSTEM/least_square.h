
#ifndef __LEAST_SQUARE_H
#define __LEAST_SQUARE_H


//最大处理数据个数
#define MAX_NUM		100

//实际输入数据个数
extern unsigned char num_count;

//原始数据
extern float x_value[MAX_NUM];
extern float y_value[MAX_NUM];

float Least_Square(void);


#endif

