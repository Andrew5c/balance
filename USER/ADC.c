

#include "main.h"

#define N  30  //DMA缓冲区大小，也是滤波参数

//volatile确保本条指令不会被编辑器优化
volatile uint16_t AD_value[N];  //ADC采样的数据经过DMA传输的数据放在这里 
volatile uint16_t after_filter; //滤波后的值缓冲

static void ADC_GPIO_Init(void);
static void ADC_Mode_Config(void);
static void DMA_Configuration(void);
//static u16 get_adc_val(void);
static void ADC_filter(void);


/*----------------------------------
**函数名称：ADC_All_Init
**功能描述：ADC初始化，开启DMA传输中断，并触发ADC开始采样，30个数据进入一次中断
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void ADC_All_Init(void)
{
	ADC_GPIO_Init();		
	DMA_Configuration();
	ADC_Mode_Config();  //到这里已经触发ADC
}

/*----------------------------------
**函数名称：ADC_GPIO_Init
**功能描述：ADC外设的引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//PA1的引脚是ADC1通道
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //PA1配置成ADC输入引脚，对应ADC通道1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //设置成输入模式
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*----------------------------------
**函数名称：ADC_Mode_Config
**功能描述：ADC模式配置
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void ADC_Mode_Config(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//使能ADC时钟
	
	//ADC最大转换频率不要超过14M，不然不准，这里选择8分频，也就是72/8 = 9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ADC1配置复位  
	ADC_DeInit(ADC1); 
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC1工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;   //扫描模式（适用于多通道），一个通道不需要扫描
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//模数转换工作在扫描模式（多通道）还是单次（单通道）模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件触发而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//规定进行规则转换的ADC通道的数目。这个数目的取值范围是1到16
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channels configuration [规则模式通道配置]*/ //55.5个采样周期,决定转换时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 ,1, ADC_SampleTime_55Cycles5);  //第三个参数配置每个ADC每个通道的采样顺序，多通道采样的时候用到

	/* Enable ADC1 DMA [使能ADC1 DMA]*/
	ADC_DMACmd(ADC1, ENABLE);//把外设与DMA建立联系
	/* Enable ADC1 [使能ADC1]*/
	ADC_Cmd(ADC1, ENABLE);           //使能ADC1
	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);  //复位校准寄存器，英语很重要啊
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));//等待校准结束
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);//开始复位
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));//等待复位结束
	
	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //触发ADC
}

/*----------------------------------
**函数名称：DMA_Configuration
**功能描述：DMA配置
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	 		//使能DMA1时钟,ADC1对应DMA1
	
	DMA_DeInit(DMA1_Channel1);//复位DMA通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//ADC片内外设的地址，就是给DMA一个起始地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)AD_value; //数据放在这里面，数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//设置DMA单向传输，外设作为DMA数据传输来源
	DMA_InitStructure.DMA_BufferSize = 30;//DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址不变
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据宽度，半字，16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存数据大小，就是外设需要传输的数据的大小
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //不断采集传输，循环模式,10个数据完成之后自动进行下一次采集
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//设置DMA通道优先级，因为这里暂时只使用了一个DMA通道，任何优先级都行
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止内存到内存的传输，因为我们使用的是内存到外设的传输。
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	//这个通道1不是随便选的，根据手册上的DMA请求映射，ADC1就对应DMA的通道1
	DMA_Cmd(DMA1_Channel1, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //使能DMA传输完成中断 
	
	//------------------------------------------
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //中断分组2
	 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrupt 
}

/*----------------------------------
**函数名称：DMA1_Channel1_IRQHandler
**功能描述：DMA中断服务函数
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)  //30个数据传输完成
	{
	  ADC_filter();
	  DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}

/*----------------------------------
**函数名称：ADC_filter
**功能描述：对ADC进行平均滤波
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
static void ADC_filter(void)
{
	int sum = 0;
	u8 i = 0;
	
	for(i = 0;i < N;i++)
	{
		sum+= AD_value[i];
	}
	after_filter = sum / N;    //每一次调用这个函数after_filter就更新一次；
}

/*----------------------------------
**函数名称：get_adc_val
**功能描述：获取ADC原始值
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
//static u16 get_adc_val(void)
//{
//	 //第三个参数配置每个ADC每个通道的采样顺序，多通道采样的时候用到
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 ,1, ADC_SampleTime_55Cycles5); 
//	
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //触发ADC
//	
//    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//等待转换结束  
//	
//	return ADC_GetConversionValue(ADC1);  //返回ADC原始值
//}

/*----------------------------------
**函数名称：ADC_cal_aveg
**功能描述：由原始值计算电压,返回值范围为0-3.3，测量电压不要超过3.3
**参数说明：无
**作者：Andrew
**日期：2018.1.25
-----------------------------------*/
float ADC_cal_aveg(void)
{
	float temp = 0;
	
	temp = (float)(after_filter) * (3.3 / 4096);//计算电压
	
	return temp;
}


