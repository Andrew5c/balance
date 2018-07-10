
#include "main.h"


//READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};//��ȡʱ��������ַ
//WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};//дʱ��������ַ


my_time TIME ;	 //��ʾʱ��Ľṹ��
u8 init_time[] = {0x00,0x29,0x00,0x28,0x06,0x02,0x18}; //��ʼ��ʱ�䣺�� �� ʱ �� �� �� ��
//u8 init_time[8];

static void DS1302_GPIO_Init(void);
static void DS1302_WriteByte(u8 byte_1);//дһ���ֽ�; byte�Ǳ����֣�������Ϊ����
static void DS1302_WriteData(u8 addr,u8 data_);//��ĳ��ַд����,data��c51�ڲ��Ĺؼ��֣���ʾ���������������ݴ洢�����ʴ˴���data_;
static u8 DS1302_ReadByte(void);//��һ���ֽ�
static u8 DS1302_ReadData(u8 addr);//��ȡĳ�Ĵ�������;
static void DS1302_delay_us(u16 time);  //����ʱ1us


/*----------------------------------
**�������ƣ�DS1302_GPIO_Init
**����������DS1302���ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void DS1302_GPIO_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;  
    
    //����GPIOD��ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

    //����GPIO�Ļ�������  
    GPIO_InitStruct.GPIO_Pin = DS1302_SCK_PIN | DS1302_CE_PIN ;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //��������ͨ�˿���Ϊ�������  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //����ٶ�50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);  

	 GPIO_InitStruct.GPIO_Pin = DS1302_IO_PIN;         //���������ɿ�©����ȻҲ������ͨ���죬������Ҫ����һֱ�л��������ģʽ
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;     //��©�������Ҫ������������Ҫ�л���������ˡ�
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //����ٶ�50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);

}

/*----------------------------------
**�������ƣ�DS1302_WriteByte
**����������DS1302дһ���ֽڲ����������λ��ʼд��
**����˵����byte_1ΪҪд����ֽ�
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void DS1302_WriteByte(u8 byte_1)
{
	u8 i = 0;
	u8 t = 0x01;
	
	for(i = 0;i<8;i++)
	{
		if((byte_1 & t) != 0)     //֮ǰ������������32��λ���������ܸ�ֵ0��1֮���ֵ��
		{
			DS1302_DATOUT = 1;
		}
		else
		{
			DS1302_DATOUT = 0;
		}
		
		DS1302_delay_us(2);
		DS1302_SCK = 1;  //������д��
		DS1302_delay_us(2);
		DS1302_SCK = 0; 
		DS1302_delay_us(2);
		
		t<<= 1;
	}
	DS1302_DATOUT = 1;      //�ͷ�IO�������ȡ�Ļ���׼ȷ�ܶ�
	DS1302_delay_us(2);     //��Ϊ���д��֮��IO�����˵͵�ƽ����©���ģʽ�¶�ȡ��ʱ�����Ӱ�죬��������ߣ��ٶ�ȡ
}

/*----------------------------------
**�������ƣ�DS1302_WriteData
**����������DS1302д����
**����˵����addr��ΪҪд��ĵ�ַ
			data_��ΪҪд�������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static void DS1302_WriteData(u8 addr,u8 data_)
{	
	DS1302_CE = 0;		DS1302_delay_us(2);	
	DS1302_SCK = 0;		DS1302_delay_us(2);	
	DS1302_CE = 1;		DS1302_delay_us(2);	//ʹ��Ƭѡ�ź�
	
	DS1302_WriteByte((addr<<1)|0x80);	//�������д��,ת��֮���ǵ�ַ�Ĵ�����ֵ��
	DS1302_WriteByte(data_);
	DS1302_CE = 0;		DS1302_delay_us(2);//�������ݽ�����ʧ��Ƭѡ
	DS1302_SCK = 0;     DS1302_delay_us(2);//���ͣ�׼����һ��д����
}

/*----------------------------------
**�������ƣ�DS1302_ReadByte
**����������DS1302��ȡһ���ֽڣ������ض�ȡ
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static u8 DS1302_ReadByte(void)
{
	u8 i = 0;
	u8 data_ = 0;
	
	//DS1302_DAT_INPUT();  //��Ϊ�����Ѿ��Ѷ˿�����Ϊ��©����·�ⲿ����ɽ�����裬���Բ��л��������ģʽ��ֱ��ʹ�á�
	
	DS1302_SCK = 0;
	DS1302_delay_us(3);
	
	for(i=0;i<7;i++)   //���﷢����Ϊ8�Ļ�������ݲ��ԣ�����
	{
		if((DS1302_DATIN) == 1) 
		{
			data_ = data_ | 0x80;	//��λ��ǰ����λ��ȡ,�տ�ʼ���ԣ����������������
		}
		data_>>= 1;
		DS1302_delay_us(3);
		
		DS1302_SCK = 1;  //��Ϊ�տ�ʼSCK�ǵ͵�ƽ������������Ϊ��һ��������
		DS1302_delay_us(3);
		DS1302_SCK = 0;
		DS1302_delay_us(3);
	}
	 return (data_);
}

/*----------------------------------
**�������ƣ�DS1302_ReadData
**����������DS1302��ȡ����
**����˵����addr��Ϊ��Ҫ��ȡ���ݵĵ�ַ
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
static u8 DS1302_ReadData(u8 addr)
{
	u8 data_ = 0;

	DS1302_CE = 0;		DS1302_delay_us(2);
	DS1302_SCK = 0;		DS1302_delay_us(2);
	DS1302_CE = 1;		DS1302_delay_us(2);   //��д����ʱCE����Ϊ�ߣ�����SCKΪ��ʱ�ı�
	
	DS1302_WriteByte((addr<<1)|0x81);   //д���ʱ�������
	data_ = DS1302_ReadByte(); 
	
	DS1302_SCK = 1;  	DS1302_delay_us(2);
	DS1302_CE = 0;	    DS1302_delay_us(2);
	DS1302_DATOUT = 0;  DS1302_delay_us(3);  //����ܶ���˵��Ҫ���ͣ������ҷ���ȥ�����Ҳ������ʾ��������Ϊ�˱��գ����Ǽ��ϡ�
	DS1302_DATOUT = 1;  DS1302_delay_us(2);

	return data_;
}

/*----------------------------------
**�������ƣ�DS1302_Init
**����������DS1302�ܳ�ʼ�����ڴ˿����޸�д��ʱ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void DS1302_Init(void)
{
//	int i = 0;
	
	DS1302_GPIO_Init();  //�˿ڳ�ʼ��
	
	DS1302_CE = 0;  DS1302_delay_us(2);
	DS1302_SCK = 0; DS1302_delay_us(2);  
	
//	i  = DS1302_ReadData(0x00);  //��ȡ��Ĵ���,
/*
**  �����Ҫ�޸�ʱ�䣬�Ͱ����if�ж�ȥ����ֱ�ӳ���д������Ȼ��д���µ�ʱ�䡣
**  �޸���ʱ��֮���ٴδ����if�жϣ�������һ����򼴿ɡ�
*/
//  if((i & 0x80) != 0)//ͨ���ж���Ĵ����Ƿ��������������´��ϵ��ʱ���Ƿ��ʼ��ʱ�䣬���ǵ��籣��
//	{
//	 	DS1302_WriteData(7,0x00); //����д����������д������,0x8e,0x00

//		for(i = 0;i<7;i++)
//		{
//			DS1302_WriteData(i,init_time[i]); 
//		}
//	}
//	 	DS1302_WriteData(7,0x80);//��д�������ܣ���ֹ��λʱʱ�䱻����
}

/*----------------------------------
**�������ƣ�adjust_real_time
**�������������������е��ã�����������ʱ�䣬���������������д�롢
**����˵������
**���ߣ�Andrew
**���ڣ�2018.5.1
-----------------------------------*/
void adjust_real_time(void)
{
	unsigned char i;	
	
	DS1302_WriteData(7,0x00); //����д����������д������,0x8e,0x00
	
	for(i = 0;i<7;i++)
	{
		DS1302_WriteData(i,init_time[i]); 
	}
	
	DS1302_WriteData(7,0x80);//��д�������ܣ���ֹ��λʱʱ�䱻����
}

/*----------------------------------
**�������ƣ�DS1302_ReadTime
**����������DS1302��ȡʱ�䵽������
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void DS1302_ReadTime(void)
{
	  u8 i;
	  for(i = 0;i<7;i++)
	  {
	     init_time[i] = DS1302_ReadData(i);
	  }
}

//�ڲ���ʱ,�޸�i�ﵽ��ͬ����ʱЧ�����ҷ���Ļ���i = 80�����ʱ1us
//��������������Ϊ5��ʱ��DS1302Ҳ�ǿ���������ȡ�ġ�
//��������ʱ�Ӳ�������Ļ���ʱ��Ƶ��Ĭ��72M��������Ҫ��8M�Ĳ��С���ʵ��������ʱҲ��72M
static void DS1302_delay_us(u16 time)
{    
   u16 i = 0;  
   while(time--)
   {
      i = 60;  //�Լ�����
      while(i--);
   }
}

/*----------------------------------
**�������ƣ�Display_Real_Time
**������������OLED����ʾʵʱʱ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.25
-----------------------------------*/
void Display_Real_Time(void)
{
	DS1302_ReadTime();   //�Ȼ�ȡʱ�䵽������
	
	//BCD��ת��ASCII��
	TIME.year =  ((init_time[6]&0x70)>>4)*10 + (init_time[6]&0x0f); //����λ�ӵ���λ
	TIME.month = ((init_time[4]&0x70)>>4)*10 + (init_time[4]&0x0f);
	TIME.date =  ((init_time[3]&0x70)>>4)*10 + (init_time[3]&0x0f);
	TIME.week =  ((init_time[5]&0x70)>>4)*10 + (init_time[5]&0x0f);
	TIME.hour =  ((init_time[2]&0x70)>>4)*10 + (init_time[2]&0x0f);
	TIME.minute = ((init_time[1]&0x70)>>4)*10 + (init_time[1]&0x0f);
	TIME.second = ((init_time[0]&0x70)>>4)*10 + (init_time[0]&0x0f);
	
//�������õڶ���Ļ��ʾ	
//	OLED_ShowNum(0,0,20,2,8);  //20**��
//	OLED_ShowNum(16,0,TIME.year,2,16);  //�Ҿ��ü�����Ӧ�ò�����ʾ�˰ɣ�˭��֪����������һ�갡����Ҫ������	
//	OLED_ShowChar(24,0,".");
//	OLED_ShowNum(0,0,TIME.month,2,16);
//	OLED_ShowNum(24,0,TIME.date,2,16);
	
	OLED_Show_Big_Time(16+0,2,TIME.hour,2); //�ڵڶ��п�ʼ��ʾ����ͷ��16��Ԫ��
	
	OLED_Show_16X32_Num(16+2*16,2,10);  //ð���ڵ�ʮ����ֱ���������ʾ�ɣ���ר��дһ��������

	OLED_Show_Big_Time(16+3*16,2,TIME.minute,2);//��ʾ����
	
//	OLED_ShowNum(18+5*16,4,TIME.second,2,16);//��ʾ�룬����С������ʾ�����½�
	
}


