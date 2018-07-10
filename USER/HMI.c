
#include "main.h"

//���õ��ⲿ����
extern int price ;
extern float money ;
extern float add_money ;
extern unsigned char DMA_Receive_LEN ;
extern volatile unsigned char start_linking;
extern float now_weight;


//�ڲ�ʹ�õı���
unsigned char money_num = 0;
float money_buf[10] = {0};
unsigned char start_dynamic_calibration = 0;	 //��ʼ��̬�궨��־
unsigned char ensure_dynamic_calibration = 0; //ȷ�ϱ궨������


/*
** ȷ������HMI����ͨ��
*/
void HMI_Send_Start(void)
{
	Delay_ms(30);
	HMI_Send_String("'\0'");
	Delay_ms(30);
}


/*---------------------------------
** ���ƣ�HMI_Send_String
** ���ܣ��жϷ�ʽ�����ַ����������������������ַ�
** ��ڣ���Ҫ���͵����ݵ�ָ��
** ���ڣ���
** ˵�����Ȱ����ݿ�������������Ȼ���������жϣ���������Լ���ִ��
----------------------------------*/
void HMI_Send_String(char *buf1)
{
	u8 i = 0;
	
	while(buf1[i] != '\0')
	{
		Tx_Buffer[Tx_Counter++] = *(buf1+i);
		i++;
	}
	
	Tx_Buffer[Tx_Counter++] = 0XFF;
	Tx_Buffer[Tx_Counter++] = 0XFF;
	Tx_Buffer[Tx_Counter++] = 0XFF;
	
	if(!(USART1->CR1 & USART_CR1_TXEIE))					//�жϷ�ʽ����
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 	//ֻҪ���ͼĴ����գ��ͻ�һֱ���ж�
}

/*---------------------------------
** ���ƣ�HMI_Receive
** ���ܣ����ݷ������ݣ����������
** ��ڣ���
** ���ڣ���
** ˵����������Ҫ�ʹ������������໥Э�����涨�÷��͵��ַ��͸�ʽ
----------------------------------*/
void HMI_Receive(void)
{
	static unsigned char y_num = 0;
	char i = 0;
	char buf[50] = {0};
	
	//ǰ����������0x65��ҳ��ID���Ѿ����жϽ����д���
	switch(CmdRx_Buffer[2])
	{
		//ȥƤr
		case 0x72:
			remove_empty_weight();
			Beep_Warning(1,100);
			break;
			
		//�ۼӽ��a
		case 0x61:
			add_money = money;		//���ݵ�ǰ�ܽ��
			Beep_Warning(1,100);
			price = 0;
			break;
		
		//ȷ�ϵ���d
		case 0x64:
			if(DMA_Receive_LEN == 5)
				price = CmdRx_Buffer[DMA_Receive_LEN-2] - 48;
			else if(DMA_Receive_LEN == 6)
				price = (CmdRx_Buffer[DMA_Receive_LEN-3]-48)*10 + (CmdRx_Buffer[DMA_Receive_LEN-2]-48);
			else if(DMA_Receive_LEN == 7)
				price = (CmdRx_Buffer[DMA_Receive_LEN-4]-48)*100 + (CmdRx_Buffer[DMA_Receive_LEN-3]-48)*10 + (CmdRx_Buffer[DMA_Receive_LEN-2]-48);	
			Beep_Warning(1,100);
			
			//������Ʒ�Ľ��洢����
			money_buf[money_num++] = price * now_weight;
			break;
		
		//������ۺͽ��c
		case 0x63:
			price = 0;
			add_money = 0;
			money_num = 0;
			memset(money_buf,0,sizeof(money_buf));
			Beep_Warning(2,100);
			break;
		
		//�鿴��ϸp
		case 0x70:
			Beep_Warning(1,100);
			for(i = 0; i<5; i++)
			{
				sprintf(buf,"t%d.txt=\"%.1f\"", i+9, money_buf[i]);	//����֮ǰ������Ʒ�ļ۸�
				HMI_Send_String(buf);
				money_buf[7]+= money_buf[i];
			}	
			sprintf(buf,"t6.txt=\"%.1f\"",money_buf[7]);	//�ϼ��ܼ۲�����
			HMI_Send_String(buf);
			break;
			
		//�ϴ�����u
		case 0x75:	
			ESP8266_Update_Data(money);	
			Beep_Warning(2,100);		
			break;

		//����w
		case 0x77:
			Beep_Warning(1,100);
			ESP8266_Link_WIFI();		//����WiFi
			start_linking = !start_linking;
			break;
		
		//���붯̬�궨���� A
		case 0x41:
			start_dynamic_calibration = 1;
			Beep_Warning(2, 100);
			sprintf(buf,"t26.txt=\"%.5f\"",Weight_Coe_100g);	//���붯̬�궨��ʱ�򣬷���һ�ε�ǰ����ϵ��
			HMI_Send_String(buf);
			Delay_ms(30);
			sprintf(buf,"t25.txt=\"%.1f\"",y_value[y_num++]);	//���붯̬�궨��ʱ�򣬷���һ��������ʾ
			HMI_Send_String(buf);
			Delay_ms(30);
			break;
		
		//ȷ�϶�̬�궨������ B
		case 0x42:
			ensure_dynamic_calibration = 1;
			Beep_Warning(1, 100);
			sprintf(buf,"t25.txt=\"%.1f\"",y_value[y_num++]);	//������һ����Ҫ�Ƶ�����
			HMI_Send_String(buf);
			Delay_ms(30);
			break;
			
		//��ɶ�̬�궨��������Ϻ�������������� C
		case 0x43:
			start_dynamic_calibration = 0;
			y_num = 0;											//�������㣬׼����һ�α궨
			Weight_Coe_100g = 1000 * Least_Square();	//��ϵ�б�ʸ��³���ϵ��
			Get_Weight_Coe();									//����������֮�����»�ȡһ��0g����
			Beep_Warning(3, 100);
			LED2_Twinkling(3);
			sprintf(buf,"t26.txt=\"%.5f\"",Weight_Coe_100g);	//������֮�󣬷����µĳ���ϵ��
			HMI_Send_String(buf);
			Delay_ms(20);
			break;
		
		//���ֻ�ǽ��붯̬�궨��û�в����ͷ��صĻ����������ʱ�ĸ��ֱ�־	D
		case 0x44:
			start_dynamic_calibration = 0;
			y_num = 0;
			Beep_Warning(1,100);
			break;
		
		default :break;		
	}
	//��ջ���Ĵ���
	memset(CmdRx_Buffer,0,sizeof(CmdRx_Buffer));

}


//HMIϢ��
void HMI_Sleep_Mode(void)
{
	char buf[10] = {0};
	
	sprintf(buf,"sleep=1");
	HMI_Send_String(buf);
	Delay_ms(20);
}

//HMI�˳�Ϣ��
void HMI_Unsleep_Mode(void)
{
	char buf[10] = {0};
	
	sprintf(buf,"sleep=0");
	HMI_Send_String(buf);
	Delay_ms(20);
}

