

#include "main.h"

#define N  30  //DMA��������С��Ҳ���˲�����

//volatileȷ������ָ��ᱻ�༭���Ż�
volatile uint16_t AD_value[N];  //ADC���������ݾ���DMA��������ݷ������� 
volatile uint16_t after_filter; //�˲����ֵ����

static void ADC_GPIO_Init(void);
static void ADC_Mode_Config(void);
static void DMA_Configuration(void);
//static u16 get_adc_val(void);
static void ADC_filter(void);


/*----------------------------------
**�������ƣ�ADC_All_Init
**����������ADC��ʼ��������DMA�����жϣ�������ADC��ʼ������30�����ݽ���һ���ж�
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void ADC_All_Init(void)
{
	ADC_GPIO_Init();		
	DMA_Configuration();
	ADC_Mode_Config();  //�������Ѿ�����ADC
}

/*----------------------------------
**�������ƣ�ADC_GPIO_Init
**����������ADC��������ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//�� ADC IO�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//PA1��������ADC1ͨ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //PA1���ó�ADC�������ţ���ӦADCͨ��1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //���ó�����ģʽ
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*----------------------------------
**�������ƣ�ADC_Mode_Config
**����������ADCģʽ����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void ADC_Mode_Config(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ʹ��ADCʱ��
	
	//ADC���ת��Ƶ�ʲ�Ҫ����14M����Ȼ��׼������ѡ��8��Ƶ��Ҳ����72/8 = 9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ADC1���ø�λ  
	ADC_DeInit(ADC1); 
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC1�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;   //ɨ��ģʽ�������ڶ�ͨ������һ��ͨ������Ҫɨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ģ��ת��������ɨ��ģʽ����ͨ�������ǵ��Σ���ͨ����ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ת������������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//�涨���й���ת����ADCͨ������Ŀ�������Ŀ��ȡֵ��Χ��1��16
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channels configuration [����ģʽͨ������]*/ //55.5����������,����ת��ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 ,1, ADC_SampleTime_55Cycles5);  //��������������ÿ��ADCÿ��ͨ���Ĳ���˳�򣬶�ͨ��������ʱ���õ�

	/* Enable ADC1 DMA [ʹ��ADC1 DMA]*/
	ADC_DMACmd(ADC1, ENABLE);//��������DMA������ϵ
	/* Enable ADC1 [ʹ��ADC1]*/
	ADC_Cmd(ADC1, ENABLE);           //ʹ��ADC1
	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);  //��λУ׼�Ĵ�����Ӣ�����Ҫ��
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�У׼����
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);//��ʼ��λ
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ���λ����
	
	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //����ADC
}

/*----------------------------------
**�������ƣ�DMA_Configuration
**����������DMA����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	 		//ʹ��DMA1ʱ��,ADC1��ӦDMA1
	
	DMA_DeInit(DMA1_Channel1);//��λDMAͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//ADCƬ������ĵ�ַ�����Ǹ�DMAһ����ʼ��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)AD_value; //���ݷ��������棬����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//����DMA�����䣬������ΪDMA���ݴ�����Դ
	DMA_InitStructure.DMA_BufferSize = 30;//DMA�����С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݿ�ȣ����֣�16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�ڴ����ݴ�С������������Ҫ��������ݵĴ�С
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //���ϲɼ����䣬ѭ��ģʽ,10���������֮���Զ�������һ�βɼ�
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//����DMAͨ�����ȼ�����Ϊ������ʱֻʹ����һ��DMAͨ�����κ����ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ֹ�ڴ浽�ڴ�Ĵ��䣬��Ϊ����ʹ�õ����ڴ浽����Ĵ��䡣
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	//���ͨ��1�������ѡ�ģ������ֲ��ϵ�DMA����ӳ�䣬ADC1�Ͷ�ӦDMA��ͨ��1
	DMA_Cmd(DMA1_Channel1, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //ʹ��DMA��������ж� 
	
	//------------------------------------------
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //�жϷ���2
	 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrupt 
}

/*----------------------------------
**�������ƣ�DMA1_Channel1_IRQHandler
**����������DMA�жϷ�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)  //30�����ݴ������
	{
	  ADC_filter();
	  DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}

/*----------------------------------
**�������ƣ�ADC_filter
**������������ADC����ƽ���˲�
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void ADC_filter(void)
{
	int sum = 0;
	u8 i = 0;
	
	for(i = 0;i < N;i++)
	{
		sum+= AD_value[i];
	}
	after_filter = sum / N;    //ÿһ�ε����������after_filter�͸���һ�Σ�
}

/*----------------------------------
**�������ƣ�get_adc_val
**������������ȡADCԭʼֵ
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
//static u16 get_adc_val(void)
//{
//	 //��������������ÿ��ADCÿ��ͨ���Ĳ���˳�򣬶�ͨ��������ʱ���õ�
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 ,1, ADC_SampleTime_55Cycles5); 
//	
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //����ADC
//	
//    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//�ȴ�ת������  
//	
//	return ADC_GetConversionValue(ADC1);  //����ADCԭʼֵ
//}

/*----------------------------------
**�������ƣ�ADC_cal_aveg
**������������ԭʼֵ�����ѹ,����ֵ��ΧΪ0-3.3��������ѹ��Ҫ����3.3
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
float ADC_cal_aveg(void)
{
	float temp = 0;
	
	temp = (float)(after_filter) * (3.3 / 4096);//�����ѹ
	
	return temp;
}


