
#ifndef __POWER_H
#define __POWER_H


//低功耗模式
extern  void Low_Power_Mode(void);

//从低功耗模式回来的时候需要重新配置时钟源
extern void Restart_From_Low_Power(void);


#endif

