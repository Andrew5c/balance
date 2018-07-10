
//SPI��ʽ��OLED��ʾ�����в���
//3.3V�ĵ�Դ����

#include "main.h"
#include "word.h"//������ģͷ�ļ�

//���ڲ����ú���
static u32 oled_pow(u8 m,u8 n);
static void OLED_GPIO_INIT(void);

/*----------------------------------
**�������ƣ�OLED_GPIO_INIT
**����������OLED��ʾ�����ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
static void OLED_GPIO_INIT(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;  
      
    //����GPIOD��ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
    //����GPIO�Ļ�������  
    GPIO_InitStruct.GPIO_Pin = OLED_CS_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_D0_PIN | OLED_D1_PIN;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //�������  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //����ٶ�50MHz  
      
    GPIO_Init(OLED_PORT, &GPIO_InitStruct);  
      
    GPIO_SetBits(OLED_PORT, OLED_CS_PIN | OLED_RST_PIN | OLED_DC_PIN | OLED_D0_PIN | OLED_D1_PIN);  
}

/*----------------------------------
**�������ƣ�SPI_Write
**����������SPIд����
**����˵����data����Ҫд�������
						Mode��Ϊѡ��д���ݻ���д����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void SPI_Write(u8 data, u8 Mode)  
{      
    u8 i = 0; 

    if(Mode)  
    {  
        OLED_DC(1);        //DC��������ߣ���ʾд����  
    }  
    else  
    {  
        OLED_DC(0);        //DC��������ͣ���ʾд����  
    }  
    OLED_CS(0);            //CS��������ͣ�Ƭѡʹ��  
    for(i = 0; i < 8; i++)  
    {  
        OLED_D0(0);        //D0���������  
        if(data&0x80)    //�жϴ�����������λΪ1����0  
        {  
            OLED_D1(1);    //D1���������  
        }  
        else  
        {  
            OLED_D1(0);    //D1���������  
        }  
        OLED_D0(1);        //D1���������  
        data<<=1;        //����������һλ  
    }  
    OLED_DC(1);            //DC���������  
    OLED_CS(1);            //CS��������ߣ�Ƭѡʧ��  
}  

/*----------------------------------
**�������ƣ�OLED_Coord
**��������������OLED��ʾ����
**����˵����X:���ĺ����꣨0-127��
						Y�������꣨0-7��
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Coord(u8 x, u8 y)  
{  	
	 SPI_Write((0xb0 + y) ,OLED_Order); 
    SPI_Write((((x & 0xf0)>>4) | 0x10), OLED_Order);//��4λ  
    SPI_Write((x & 0x0f)|0x01, OLED_Order);//��4λ  
}  

/*----------------------------------
**�������ƣ�OLED_Clear
**����������OLED����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
//������һ��ʼ����д���ˣ���д����д����д���ݣ�������������ȷ��������Ļ���кܶ���㣬˵��û�������ɹ���
-----------------------------------*/
void OLED_Clear(void)  
{  
    u8 i = 0, j = 0;  

    for(i = 0; i < 8; i++)  
    {  
		SPI_Write(0xb0 + i,OLED_Order);
		SPI_Write(0x00,OLED_Order);
		SPI_Write(0x10,OLED_Order);
        for(j = 0; j < 128; j++)  
        {  
            SPI_Write(0x00, OLED_Data);  
        }  
    }  
}  

/*----------------------------------
**�������ƣ�OLED_Display_Off
**�����������ر�OLED��ʾ���ܣ���������ʾ�ַ�
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Display_Off(void)  
{
	 SPI_Write(0x8D,OLED_Order);
    SPI_Write(0x10,OLED_Order);
    SPI_Write(0xAE,OLED_Order);
}  

/*----------------------------------
**�������ƣ�OLED_Display_On
**������������OLED��ʾ
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Display_On(void)
{  
    //��ɱ����ã���ʼ��ʱ����򿪣����򿴲�����ʾ��
    SPI_Write(0x8D, OLED_Order);  
    SPI_Write(0x14, OLED_Order);//bit2   0���ر�        1����  
    SPI_Write(0xAF, OLED_Order);//0xAF:����ʾ      
}  

/*----------------------------------
**�������ƣ�OLED_Init
**����������OLED���ֳ�ʼ����Ҳ�Ǹ��ּĴ�����ʼֵ����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
**˵����
	2018.5.19����Ҫ��OLED��ʼ��֮ǰ�͵�����������ʾ��������Ȼ����Ῠ���ڳ�ʼ��������
-----------------------------------*/
void OLED_Init(void)  
{  
    OLED_GPIO_INIT();  //�˿ڳ�ʼ��
      
    OLED_RST(1);     
    Delay_ms(100);   
    OLED_RST(0);  
    Delay_ms(100);  
    OLED_RST(1);
    
    SPI_Write(0xAE, OLED_Order);//0xAE:����ʾ     

    SPI_Write(0x00, OLED_Order);//���õ��е�ַ  
    SPI_Write(0x10, OLED_Order);//���ø��е�ַ  
      
    //��������ʾ�Ŀ�ʼ��ַ(0-63)  
    //40-47: (01xxxxx)  
    SPI_Write(0x40, OLED_Order);
      
    //���öԱȶ�  
    SPI_Write(0x81, OLED_Order);
    SPI_Write(0xff, OLED_Order);//���ֵԽ����ĻԽ��(������ָ��һ��ʹ��)(0x00-0xff)  
      
    SPI_Write(0xA1, OLED_Order);//0xA1: ���ҷ��ã�  0xA0: ������ʾ��Ĭ��0xA0��  
    SPI_Write(0xC8, OLED_Order);//0xC8: ���·��ã�  0xC0: ������ʾ��Ĭ��0xC0��  
      
    //0xA6: ��ʾ������ʾ���������1��ʾ������0��ʾ������  
    //0xA7: ��ʾ����ʾ���������0��ʾ������1��ʾ������  
    SPI_Write(0xA6, OLED_Order);  
      
    SPI_Write(0xA8, OLED_Order);//���ö�·�����ʣ�1-64��  
    SPI_Write(0x3F, OLED_Order);//��0x01-0x3f��(Ĭ��Ϊ3f)  
      
      
    //������ʾ������λӳ���ڴ������  
    SPI_Write(0xD3, OLED_Order);  
    SPI_Write(0x00, OLED_Order);//��0x00-0x3f��(Ĭ��Ϊ0x00)  
      
    //������ʾʱ�ӷ�Ƶ����/����Ƶ��  
    SPI_Write(0xD5, OLED_Order);  
    //��4λ������ʾʱ��(��Ļ��ˢ��ʱ��)��Ĭ�ϣ�0000����Ƶ����= [3:0]+1  
    //��4λ��������Ƶ�ʣ�Ĭ�ϣ�1000��  
    SPI_Write(0x80, OLED_Order);//  
      
    //ʱ��Ԥ�������  
    SPI_Write(0xD9, OLED_Order);  
    SPI_Write(0xF1, OLED_Order);//[3:0],PHASE 1;   [7:4] PHASE 2  
      
    //����COMӲ��Ӧ������  
    SPI_Write(0xDA, OLED_Order);  
    SPI_Write(0x12, OLED_Order);//[5:4]  Ĭ�ϣ�01  
      
    SPI_Write(0xDB, OLED_Order);//  
    SPI_Write(0x40, OLED_Order);//  
      
    //�����ڴ�Ѱַ��ʽ  
    SPI_Write(0x20, OLED_Order);  
    //00: ��ʾˮƽѰַ��ʽ  
    //01: ��ʾ��ֱѰַ��ʽ  
    //10: ��ʾҳѰַ��ʽ��Ĭ�Ϸ�ʽ��  
    SPI_Write(0x02, OLED_Order);//      
      
    //��ɱ����ã���ʼ��ʱ����򿪣����򿴲�����ʾ��  
    SPI_Write(0x8D, OLED_Order);  
    SPI_Write(0x14, OLED_Order);//bit2   0���ر�        1����  
      
    //�����Ƿ�ȫ����ʾ 0xA4: ��ֹȫ����ʾ  
    SPI_Write(0xA4, OLED_Order);  
  
    //0xA6: ��ʾ������ʾ���������1��ʾ������0��ʾ������  
    //0xA7: ��ʾ����ʾ���������0��ʾ������1��ʾ������  
    SPI_Write(0xA6, OLED_Order);//  
      
    SPI_Write(0xAF, OLED_Order);//0xAF:����ʾ     
	 SPI_Write(0xAF, OLED_Order); //��֪��ΪʲôҪд����
	
    OLED_Clear();
	 OLED_Coord(0,0);
}  

/*----------------------------------
**�������ƣ�OLED_ShowChinese
**������������괦��ʾ����
**����˵����X,YΪ���忪ʼ����
			ChineseΪҪ��ʾ����ȡģ���������ֿ�������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_ShowChinese(u8 x, u8 y, u8 chinese)  
{  
	u8 t,adder=0;
	
	OLED_Coord(x,y);

    for(t=0;t<16;t++)  //ÿ��16��Ԫ�أ�һ������Ҫ�����ַ���
	{
		SPI_Write(Hzk[2*chinese][t],OLED_Data);
		adder+=1;
     }	
	
	OLED_Coord(x,y+1);
	 
    for(t=0;t<16;t++)
	{	
		SPI_Write(Hzk[2*chinese+1][t],OLED_Data);
		adder+=1;
      }
} 

/*----------------------------------
**�������ƣ�OLED_ShowChar
**������������괦��ʾһ���ַ�
**����˵����X,YΪ����
			chrΪҪ��ʾ���ַ����õ����ű�ʾ
			�ı�ǰ��SIZE����ֵ����ѡ����Ҫ�ַ��������С���ֿ����������幩ѡ��
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_ShowChar(u8 x, u8 y, u8 chr)  
{  
	unsigned char c=0,   i=0;	
	
	c = chr - ' ';    //�õ�ƫ�ƺ��ֵ
	
		if(x > Max_Column - 1)
			{x=0;y=y+2;}
			
		if(SIZE ==16)  //8*16�ַ�
		{
			OLED_Coord(x,y);
			
			for(i=0;i<8;i++)
				SPI_Write(F8X16[c*16+i],OLED_Data);
			
			OLED_Coord(x,y+1);
			
			for(i=0;i<8;i++)
				SPI_Write(F8X16[c*16+i+8],OLED_Data);
		}
		else    //6*8�ַ�
		{	
			OLED_Coord(x,y+1);
			
			for(i=0;i<6;i++)
				SPI_Write(F6x8[c][i],OLED_Data);
			
		}
}  
/*----------------------------------
**�������ƣ�OLED_Show_String
**������������괦��ʾ�ַ������ַ��������������ʾ��unsigned char string_2[] = {"THIS IS A TEST  "};
**����˵����X,YΪ����
			* chr���ַ����׵�ַ
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Show_String(u8 x, u8 y, u8 *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j]);

		x+= 8 ;

		if(x>120){x=0;y+=2;}  //�Զ�����д

		j++;
	}
}

/*----------------------------------
**�������ƣ�oled_pow
**����������ȡ�ݴη�����
**����˵����mΪ�ף�nΪ�ݣ�m^n
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
static u32 oled_pow(u8 m,u8 n)
{
	u32 result = 1; 
	while(n--)result*=m;    
	return result;
}

/*----------------------------------
**�������ƣ�OLED_ShowNum
**������������ʾ����
**����˵����X,Y :Ϊ������ʼ���� 
			num��Ҫ��ʾ������
			len��Ϊ���ֵ�λ��
			size��Ϊ��Ҫ���ֵ������С����ʵ�ı�ľ�������֮��ľ���
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t = 0, temp = 0;
//	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;   //ȡ��ÿһλ��
		OLED_ShowChar(x+(size/2)*t,y,temp+'0');
		
//������һ�ξ��ǰ�Ϊ0�����ֲ���ʾ������03������������һ��֮���ֻ��ʾ3
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size/2)*t,y,' ');
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
} 


//---------�Զ�����ʾ����������--------

/*----------------------------------
**�������ƣ�OLED_Show_16X32_Num
**������������ʾ16*32��������֣�������ʾʱ��ʱ��
**����˵����X,Y:Ϊ������ʼ����
			num��Ϊ�������� 16X32 ��ģ�е����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Show_16X32_Num(u8 x, u8 y, u8 num)  
{  
	u8 t,adder=0;
	
	//��һ��
	OLED_Coord(x,y);
    for(t=0;t<16;t++)  //ÿ��16��Ԫ�أ�һ������Ҫ�����ַ���
	{
		SPI_Write(F16X32[4*num + 0][t],OLED_Data);
		adder+=1;
    }
	//�ڶ���
	OLED_Coord(x,y + 1); 
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 1][t],OLED_Data);
		adder+=1;
    }	
	//������
	OLED_Coord(x,y + 2);
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 2][t],OLED_Data);
		adder+=1;
    }	
	//������
	OLED_Coord(x,y + 3);
    for(t=0;t<16;t++)
	{	
		SPI_Write(F16X32[4*num + 3][t],OLED_Data);
		adder+=1;
    }
	
} 
/*----------------------------------
**�������ƣ�OLED_Show_Big_Time
**������������������ĺ�����ʾ������ʱ��
**����˵����X,Y:Ϊ����
			num��ΪҪ��ʾ������
			len��Ϊ���ֵ�λ��
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void OLED_Show_Big_Time(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t = 0, temp = 0;
	
	for(t=0;t<len;t++)
	{
		temp = (num/oled_pow(10,len-t-1)) % 10;   //ȡ��ÿһλ��
		OLED_Show_16X32_Num(x+(t*16),y,temp);
	}
}




/*
** ���С�����һλ֮����������ݣ���ֹ���ϴ�һ���ݵĲ�����
** ���ڶ�̬��ʾ������,���Ҳ�����Ϊ���ȫ�����ݶ���˸
** ����ע����������ݲ�Ҫ����Խ��
*/
void Clear_Left_Num(unsigned char *num_string)
{
	while(*num_string != '.')
		num_string++;
	//һλС�������������ÿո�ˢ��
	*(num_string+2) = ' ';
	*(num_string+3) = ' ';
	*(num_string+4) = ' ';
}

/*
** ���Ч���ȽϺ�
** �ڶ��涯̬ˢ�¸�������������
** sprintf �����ڸ�ʽ��֮����ڽ���ַ����ĺ������һ�� \0 ��
** ��������� \0 Ϊ��־������Ĳ�������ȫ�����
*/
void Clear_Left_Num2(unsigned char *num_string)
{
	while(*num_string != '\0')
		num_string++;
	*(num_string) = ' ';
	*(num_string+1) = ' ';
	*(num_string+2) = ' ';
//	*(num_string+3) = ' ';
}

//��ʾ���ۡ����������
void OLED_Show_Tips(void)
{
	//������ӳ�
	OLED_ShowChinese(24,0,6);
	OLED_ShowChinese(24+16,0,7);
	OLED_ShowChinese(24+32,0,8);
	OLED_ShowChinese(24+48,0,9);
	OLED_ShowChinese(24+64,0,10);
	//����
	OLED_ShowChinese(0,2,0);
	OLED_ShowChinese(16,2,1);
	OLED_ShowChar(33,2,':');
	//����
	OLED_ShowChinese(0,4,2);
	OLED_ShowChinese(16,4,3);
	OLED_ShowChar(33,4,':');
	//���
	OLED_ShowChinese(0,6,4);
	OLED_ShowChinese(16,6,5);
	OLED_ShowChar(33,6,':');
}

