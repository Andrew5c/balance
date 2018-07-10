
/*
** 24λ���ADCоƬ�����÷Ŵ��·��128���Ŵ�
*/

#include "main.h"


#define SCS1237_GPIO_PORT		GPIOA

#define SSCLK_PIN					GPIO_Pin_1
#define SDOUT_PIN					GPIO_Pin_0

//֮ǰʹ�����ַ�ʽ���԰�IO��˫��ʹ�ã�����������Ͳ������ˣ���֪��Ϊʲô
//#define SCLK           PAout(2)
//#define SDOUT_IN        PAin(4)   //io����Ϊ����
//#define DOUT_OUT       PAout(4)  //io����Ϊ���

//ʹ����������л��ķ�ʽ���ж�д
#define SSCLK_1		GPIO_SetBits(SCS1237_GPIO_PORT,SSCLK_PIN)
#define SSCLK_0		GPIO_ResetBits(SCS1237_GPIO_PORT,SSCLK_PIN)

#define SDOUT_1		GPIO_SetBits(SCS1237_GPIO_PORT,SDOUT_PIN)
#define SDOUT_0		GPIO_ResetBits(SCS1237_GPIO_PORT,SDOUT_PIN)

//��ȡ�˿ڵ�ƽ
#define SDOUT_IN()		GPIO_ReadInputDataBit(SCS1237_GPIO_PORT,SDOUT_PIN)

#define SCS_CONFIG   0X0C		//оƬ���üĴ�����ַ
#define SADC_BIT     20			//ADC��Чλ����������λ�����24λ
#define SFILTER		  0.00001	//�˲�ϵ����С��1


long int STD_ADC_LAST = 0;	//��һ�ֵ�ADC��ֵ����
long int STD_ADC_NOW = 0;


static void SCS1237_GPIO_Init(void);
static void SOne_CLK(void);
static void SCS1237_delay_us(u32 time);
static void SCS1237_delay_ms(u32 time);

static void Smode_IPU(void);
static void Smode_Out_PP(void);


//���ų�ʼ��֮��Ĭ���ǵ͵�ƽ
static void SCS1237_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Pin = SSCLK_PIN | SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
	
}

//����Ϊ����ģʽ
static void Smode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
}
//����Ϊ���ģʽ
static void Smode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SDOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCS1237_GPIO_PORT, &GPIO_InitStructure);
}


//����һ������
static void SOne_CLK(void)
{
	SSCLK_1;
	SCS1237_delay_us(10);
	SSCLK_0;
	SCS1237_delay_us(10);
}

//�ߵ�ƽ���ٱ���100us��cs1237����͹���ģʽ��оƬ�ر����е�·�����Ľӽ�0
void SCS1237_Power_Down(void)
{
	SSCLK_1;
	SCS1237_delay_us(100);
	SSCLK_1;
	SCS1237_delay_us(100);
}

//cs1237���»���,SCLK�ص��͵�ƽ������10us
void SCS1237_Restart(void)
{
	SSCLK_0;
	SCS1237_delay_us(20);
}


/*---------------------------------
** ���ƣ�CS1237_Write_Config
** ���ܣ�����ƼĴ�����д������
** ��ڣ���Ҫд��Ŀ�����
** ���ڣ���
** ˵����
д���ƼĴ�����ַ��0X65
д��Ĭ�Ͽ����֣�0X0C
----------------------------------*/
void SCS1237_Write_Config(unsigned char config)
{
	unsigned char i = 0;
	unsigned char _dat = 0x80;
	unsigned char count_i = 0;//�����ʱ��

	SSCLK_0;//ʱ������
	
	Smode_IPU();
	while(SDOUT_IN() == 1)//оƬ׼�������������ʱ���Ѿ�Ϊ0������Ҳ��Ҫ��CS1237ȫ������Ϊ0���㶼׼����
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return;//��ʱ����ֱ���˳�����
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		SOne_CLK();
	}
	Smode_Out_PP();
	//��30-36��ʱ�����ڣ�����Ĵ�����д������������ݣ���λ�����룩��������д��Ӧ����0x65
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//30
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//31
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//32
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//33
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//34
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//35
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//36
	
	SOne_CLK();//37���л�DOUT�ķ���  

	for(i=0;i<8;i++)// 38 - 45�����壬д��Ĵ������������ݣ���λ��д�룩
	{
		SSCLK_1;SCS1237_delay_us(40);
		if((config & _dat) != 0)
			SDOUT_1;
		else
			SDOUT_0;
		SSCLK_0;SCS1237_delay_us(40);
		_dat >>= 1;
	}
	SOne_CLK();//46�����壬�л�DOUT���ţ���������DOUT����
}


/*---------------------------------
** ���ƣ�CS1237_Read_Config
** ���ܣ���ȡCS1237�Ŀ��ƼĴ����е�����
** ��ڣ���
** ���ڣ������Ŀ�����
** ˵����
�����ƼĴ�����ַ��0X56
�����ղ�д�Ŀ�����0X0C����������Ŀ����ֺ͸ղ�д���һ����˵��ͨ�ųɹ�
----------------------------------*/
unsigned char SCS1237_Read_Config(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;		//��ȡ��������
	unsigned char count_i = 0;	//�����ʱ��
	
	SSCLK_0;
	
	Smode_IPU();
	while(SDOUT_IN() == 1)//оƬ׼�����������  ʱ���Ѿ�Ϊ0������Ҳ��Ҫ��CS1237ȫ������Ϊ0���㶼׼����
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return 1;//��ʱ����ֱ���˳�����
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		SOne_CLK();
	}
	
	Smode_Out_PP();
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//30
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//31
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//32
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//33
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//34
	SSCLK_1;SCS1237_delay_us(30);SDOUT_1;SSCLK_0;SCS1237_delay_us(30);//35
	SSCLK_1;SCS1237_delay_us(30);SDOUT_0;SSCLK_0;SCS1237_delay_us(30);//36
	
	SDOUT_1;
	
	SOne_CLK();//37     д����0x56���л�DOUT����
	
	Smode_IPU();
	for(i=0;i<8;i++)// 38 - 45�������ˣ���ȡ����
	{
		SOne_CLK();
		dat <<= 1;
		if(SDOUT_IN() == 1)
			dat++;
	}
	SOne_CLK();//46������������������
	
	return dat;
}


/*---------------------------------
** ���ƣ�CS1237_Read_ADC
** ���ܣ���ȡCS1237��ADC����
** ��ڣ���
** ���ڣ�20λ��ADC����
** ˵����
----------------------------------*/
long int SCS1237_Read_ADC(void)
{
	unsigned char i = 0;
	long int dat = 0;
	unsigned char count_i = 0;
	
//	Smode_Out_PP();
//	SDOUT_1;//�˿����棬32��һ����Ҫ
	SSCLK_0;
	
	Smode_IPU();
	while(SDOUT_IN() == 1)
	{
		SCS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SSCLK_1;
			SDOUT_1;
			return 0;//��ʱ����ֱ���˳�����
		}
	}
		
	//SDOUT_1;//���棿
	//Smode_IPU();
	
	//��ȡ24λ��Чת��
	for(i = 0; i<24; i++)
	{
		SSCLK_1;
		SCS1237_delay_us(10);
		dat <<= 1;
		if(SDOUT_IN() == 1)
			dat++;
		SSCLK_0;SCS1237_delay_us(10);	
	}
	//һ����Ҫ����27������
	for(i=0; i<3; i++)
		SOne_CLK();
	
	Smode_Out_PP();
	SDOUT_1;
	
	i = 24 - SADC_BIT;
	dat >>= i;		//���������λ��
	
	return dat;
}


/*---------------------------------
** ���ƣ�CS1237_Read_18bit_ADC
** ���ܣ���ԭʼ��ADC���ݽ��е�ͨ�˲�
** ��ڣ���
** ���ڣ�18λ��ADC����
** ˵����
һ�������˲�����һ�׵�ͨ�˲�����
�����˲�����  C = B*SFILTER + C*(1-SFILTER)
�ŵ㣺
	  �������Ը��ž������õ���������
	  �����ڲ���Ƶ�ʽϸߵĳ���
ȱ�㣺
	  ��λ�ͺ������ȵ�
	  �ͺ�̶�ȡ����aֵ��С
	  ���������˲�Ƶ�ʸ��ڲ���Ƶ�ʵ�1/2�ĸ����ź�
----------------------------------*/
long int SCS1237_Read_18bit_ADC(void)
{
	//��������
	STD_ADC_NOW = SCS1237_Read_ADC();
	
	//��ȡ����ȷ������
	if(STD_ADC_NOW != 0)
	{
		STD_ADC_LAST = STD_ADC_LAST*SFILTER + STD_ADC_NOW*(1 - SFILTER);
	}
	return STD_ADC_LAST; 
}


//��ʼ��ADC��ز���
void SCS1237_Init(void)
{
	//�˿ڳ�ʼ��
	SCS1237_GPIO_Init();
	
	SCS1237_delay_ms(1);
	
	//��оƬ��д��һ��Ĭ�Ͽ�����
	SCS1237_Write_Config(0x0c);
	//ֱ����ȡ����ȷ������
	while(SCS1237_Read_Config() != 0x0c)
	{
		//�����ȡ�����ݺ�д������ݲ�һ����˵��û��ͨ�ųɹ�
		printf(" \n\tSCS1237 read error...\n");
		continue;
	}
	
	printf("\n\tSCS1237 read Success!!\n");
	
	STD_ADC_LAST = SCS1237_Read_ADC();
	STD_ADC_LAST = SCS1237_Read_ADC();
	STD_ADC_LAST = SCS1237_Read_ADC();
}


/*
** �ڲ���ʱ��������8����������1us
*/
static void SCS1237_delay_us(u32 time)
{    
  u32 i = 8 * time;  
  while(i--); 
}
//ms��ʱ
static void SCS1237_delay_ms(u32 time)  
{  
  u32 i=8000*time;  
  while(i--);  
}  


