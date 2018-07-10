
#ifndef __BALANCE_H
#define __BALANCE_H

//让动态标定改变这个斜率
extern float Weight_Coe_100g ;

void Get_Weight_Coe(void);
float Get_Weight(void);
void remove_empty_weight(void);


#endif
