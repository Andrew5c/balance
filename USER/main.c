
#include "main.h"

#define SEND_DATA		0

//��ǰ��ȡ������
float now_weight = 0;
//����������ĵ���
int price = 0;
//������
float money = 0;
//�ۼӽ���ʱ��������֮ǰ�Ľ��
float add_money = 0;
//��ʽ��������
char string_buf[50];

 
//�жϵ͹�����Ҫ�ı���
float low_power_weight_1 = 0;
float low_power_weight_2 = 0;
unsigned char low_power_num = 0;

//�ж����ӷ�����
volatile unsigned char start_linking = 0;
unsigned char main_loop_num = 0;

extern unsigned char start_dynamic_calibration ;	 //��ʼ��̬�궨��־

int main(void)
{
	OLED_Init();				//OLED��ʾ
	DS1302_Init();				//ʵʱʱ��
	ADC_All_Init();			//�ɼ���Դ��ѹ
	
	ESP8266_Init(115200);	//ESP8266����ͨ������
	
	USART_1_Init(9600);  	 //HMI������ͨ������
	
	Beep_Init();				//��������Ϊ������ʾ
	LED_Init();					//LED��ʾ
	Key_Init();					//��������ͣ��ģʽ
	
	CS1237_Init();				//CS1237��ʼ��
	Get_Weight_Coe();			//��ȡ����ϵ�����ϵ�ʱ�Ƶõ����������涼��ȥ���ֵ
	HMI_Send_Start();			//ȷ���ʹ�����������ͨ��
	HMI_Unsleep_Mode();		//�˳�Ϣ��
	
	//�ȸ���ʾ��һ��ֵ�������˼ҵȼ��ˣ���Ϊ��������
	sprintf(string_buf,"t0.txt=\"0\"");
	HMI_Send_String(string_buf);
	Delay_ms(20);
	sprintf(string_buf,"t2.txt=\"0\"");
	HMI_Send_String(string_buf);
	
	Beep_Warning(3,100);
	LED_Start();

	while(1)
	{
		#if SEND_DATA
		printf("%ld\n",CS1237_Read_18bit_ADC());
		printf("%.2f g\n",now_weight); 
		Send_Senser();
		#endif
		
		//��ȡʵʱ������ˢ��
		now_weight = Get_Weight();
				
		if(start_dynamic_calibration != 1)		//��̬�궨��ʱ����ϵͳֹͣ����������
		{
			//��ⳬ�����̱���
			if(now_weight >= 500)		//�����ﶨʱ����ʱʱ�䵽��Ȼ�������ⲿ�жϺͶ�ʱ���ж�֮��ѭ����
				Beep_Warning(3,100);
			else if(now_weight <= 0.8)
				now_weight = 0;
			
			//����ʵʱ����
			sprintf(string_buf,"t4.txt=\"%.1f\"",now_weight);
			HMI_Send_String(string_buf);
		
			//����۸�ˢ��
			//��ǰ�µļ��������֮ǰ�����ۼӰ���ʱ���ݵĽ��
			money = now_weight * price + add_money;		
			sprintf(string_buf,"t2.txt=\"%.1f\"",money);
			HMI_Send_String(string_buf);
			Delay_ms(10);						//��Ϊ9600�Ĳ����ʣ��Ƚϵͣ�������ʱ�ȴ����ݷ�����ϣ���Ȼ����һֱ���봮���ж�Ӱ�����ķ��ͺͽ��ա�
		
			//OLED��ʾʱ���ʣ�����
			Display_Real_Time();
			OLED_Show_RealTime_Battery(110,0);	
		}
		
		//�鿴DMA��������������
		if(USART_DMA_Receive() == 1)
			HMI_Receive();
		
		//�͹����ж�
		//���淢����û���յ��������İ���ʱ��ѭ��һ�ε�ʱ�����Ϊ1s������ʡȥ��һ����ʱ�����������ж�
		low_power_weight_1 = now_weight;
		if((low_power_weight_1 < 1) && ((low_power_weight_1-low_power_weight_2 < 1) || (low_power_weight_2-low_power_weight_1 < 1)))
		{
			low_power_num++;
			LED1_Twinkling(2);
			LED2_Twinkling(2);
			Delay_ms(1000);
		}
		else
		{
			low_power_num = 0;
			low_power_weight_2 = low_power_weight_1;
		}
		
		//��������1g������40����û�б仯������ʼ����͹���
		if(low_power_num >= 50)
		{
			//�������㣬׼����һ�εļ���
			low_power_num = 0;
			//������������������ʾ
			Beep_Warning(3,200);		
			//����͹���
			Low_Power_Mode();
		}
		
		//��¼����������������
		if(start_linking == 1)
		{
			main_loop_num++;
			if(main_loop_num >= 20)
			{
				main_loop_num = 0;
				ESP8266_Keep_Linking();
				OLED_Show_String(0,0,(unsigned char *)"wifi");
			}
		}

	}	
}
	

