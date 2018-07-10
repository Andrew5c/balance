
/*
** 24λ���ADCоƬ�����÷Ŵ��·��128���Ŵ�
*/

#include "main.h"

long int ADC_LAST = 0;	//��һ�ֵ�ADC��ֵ����
long int ADC_NOW = 0;


static void CS1237_GPIO_Init(void);
static void One_CLK(void);
static void CS1237_delay_us(u32 time);
static void CS1237_delay_ms(u32 time);

static void mode_IPU(void);
static void mode_Out_PP(void);


//���ų�ʼ��֮��Ĭ���ǵ͵�ƽ
static void CS1237_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Pin = SCLK_PIN | DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
	
}

//����Ϊ����ģʽ
static void mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
}
//����Ϊ���ģʽ
static void mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DOUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CS1237_GPIO_PORT, &GPIO_InitStructure);
}


//����һ������
static void One_CLK(void)
{
	SCLK_1;
	CS1237_delay_us(10);
	SCLK_0;
	CS1237_delay_us(10);
}

//�ߵ�ƽ���ٱ���100us��cs1237����͹���ģʽ��оƬ�ر����е�·�����Ľӽ�0
void CS1237_Power_Down(void)
{
	SCLK_1;
	CS1237_delay_us(100);
	SCLK_1;
	CS1237_delay_us(100);
}

//cs1237���»���,SCLK�ص��͵�ƽ������10us
void CS1237_Restart(void)
{
	SCLK_0;
	CS1237_delay_us(20);
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
void CS1237_Write_Config(unsigned char config)
{
	unsigned char i = 0;
	unsigned char _dat = 0x80;
	unsigned char count_i = 0;//�����ʱ��

	SCLK_0;//ʱ������
	
	mode_IPU();
	while(DOUT_IN() == 1)//оƬ׼�������������ʱ���Ѿ�Ϊ0������Ҳ��Ҫ��CS1237ȫ������Ϊ0���㶼׼����
	{
		CS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return;//��ʱ����ֱ���˳�����
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		One_CLK();
	}
	mode_Out_PP();
	//��30-36��ʱ�����ڣ�����Ĵ�����д������������ݣ���λ�����룩��������д��Ӧ����0x65
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//30
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//31
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//32
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//33
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//34
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//35
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//36
	
	One_CLK();//37���л�DOUT�ķ���  

	for(i=0;i<8;i++)// 38 - 45�����壬д��Ĵ������������ݣ���λ��д�룩
	{
		SCLK_1;CS1237_delay_us(40);
		if((config & _dat) != 0)
			DOUT_1;
		else
			DOUT_0;
		SCLK_0;CS1237_delay_us(40);
		_dat >>= 1;
	}
	One_CLK();//46�����壬�л�DOUT���ţ���������DOUT����
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
unsigned char CS1237_Read_Config(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;		//��ȡ��������
	unsigned char count_i = 0;	//�����ʱ��
	
	SCLK_0;
	
	mode_IPU();
	while(DOUT_IN() == 1)//оƬ׼�����������  ʱ���Ѿ�Ϊ0������Ҳ��Ҫ��CS1237ȫ������Ϊ0���㶼׼����
	{
		CS1237_delay_us(5000);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return 1;//��ʱ����ֱ���˳�����
		}
	}
	
	for(i=0;i<29;i++)// 1 - 29
	{
		One_CLK();
	}
	
	mode_Out_PP();
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//30
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//31
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//32
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//33
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//34
	SCLK_1;CS1237_delay_us(30);DOUT_1;SCLK_0;CS1237_delay_us(30);//35
	SCLK_1;CS1237_delay_us(30);DOUT_0;SCLK_0;CS1237_delay_us(30);//36
	
	DOUT_1;
	
	One_CLK();//37     д����0x56���л�DOUT����
	
	mode_IPU();
	for(i=0;i<8;i++)// 38 - 45�������ˣ���ȡ����
	{
		One_CLK();
		dat <<= 1;
		if(DOUT_IN() == 1)
			dat++;
	}
	One_CLK();//46������������������
	
	return dat;
}


/*---------------------------------
** ���ƣ�CS1237_Read_ADC
** ���ܣ���ȡCS1237��ADC����
** ��ڣ���
** ���ڣ�20λ��ADC����
** ˵����
----------------------------------*/
long int CS1237_Read_ADC(void)
{
	unsigned char i = 0;
	long int dat = 0;
	unsigned char count_i = 0;
	
//	mode_Out_PP();
//	DOUT_1;//�˿����棬32��һ����Ҫ
	SCLK_0;
	
	mode_IPU();
	while(DOUT_IN() == 1)
	{
		CS1237_delay_ms(1);
		count_i++;
		if(count_i > 150)
		{
			SCLK_1;
			DOUT_1;
			return 0;//10s��ʱ����ֱ���˳�����
		}
	}
		
	//DOUT_1;//���棿
	//mode_IPU();
	
	//��ȡ24λ��Чת��
	for(i = 0; i<24; i++)
	{
		SCLK_1;
		CS1237_delay_us(10);
		dat <<= 1;
		if(DOUT_IN() == 1)
			dat++;
		SCLK_0;CS1237_delay_us(10);	
	}
	//һ����Ҫ����27������
	for(i=0; i<3; i++)
		One_CLK();
	
	mode_Out_PP();
	DOUT_1;
	
	i = 24 - ADC_BIT;
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
�����˲�����  C = B*FILTER + C*(1-FILTER)
�ŵ㣺
	  �������Ը��ž������õ���������
	  �����ڲ���Ƶ�ʽϸߵĳ���
ȱ�㣺
	  ��λ�ͺ������ȵ�
	  �ͺ�̶�ȡ����aֵ��С
	  ���������˲�Ƶ�ʸ��ڲ���Ƶ�ʵ�1/2�ĸ����ź�
----------------------------------*/
long int CS1237_Read_18bit_ADC(void)
{
	//��������
	ADC_NOW = CS1237_Read_ADC();
	
	//��ȡ����ȷ������
	if(ADC_NOW != 0)
	{
		ADC_LAST = ADC_LAST*FILTER + ADC_NOW*(1 - FILTER);
	}
	return ADC_LAST; 
}


//��ʼ��ADC��ز���
void CS1237_Init(void)
{
	//�˿ڳ�ʼ��
	CS1237_GPIO_Init();
	
	CS1237_delay_ms(1);
	
	//��оƬ��д��һ��Ĭ�Ͽ�����
	CS1237_Write_Config(0x0c);
	//ֱ����ȡ����ȷ������
	while(CS1237_Read_Config() != 0x0c)
	{
		//�����ȡ�����ݺ�д������ݲ�һ����˵��û��ͨ�ųɹ�
		printf(" \n\tCS1237 read error...\n");
		continue;
	}
	
	printf("\n\tCS1237 read Success!!\n");
	
	ADC_LAST = CS1237_Read_ADC();
	ADC_LAST = CS1237_Read_ADC();
	ADC_LAST = CS1237_Read_ADC();
}


/*
** �ڲ���ʱ��������8����������1us
*/
static void CS1237_delay_us(u32 time)
{    
  u32 i = 8 * time;  
  while(i--); 
}
//ms��ʱ
static void CS1237_delay_ms(u32 time)  
{  
  u32 i=8000*time;  
  while(i--);  
}  


