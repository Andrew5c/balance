
#ifndef __CLIENT_H
#define __CLIENT_H

#include <stm32f10x.h>
#include <stdbool.h>


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif



/******************************* ESP8266 �������Ͷ��� ***************************/
typedef enum{
	STA,
	AP,
	STA_AP  
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;


/******************************* ESP8266 �ⲿȫ�ֱ������� ***************************/
#define RX_BUF_MAX_LEN     500                                     //�����ջ����ֽ���

extern struct  STRUCT_USARTx_Fram                                  //��������֡�Ĵ���ṹ��
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
  
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;//ʹ��λ�Σ����ٿռ���˷ѣ����������ǰ15λ��ʾ��������������һλ��ʾ������ɱ�־��
  }; 
	
} strEsp8266_Fram_Record;


//�ɱ������
#define     macESP8266_Usart( fmt, ... )           USART2_printf ( USART2, fmt, ##__VA_ARGS__ ) 
//#define     macPC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )


//�ⲿ����
extern volatile unsigned int time_flag_1 ;

extern volatile unsigned char receive_flag;
extern char receive_buf[150];


extern char userkey[100];
extern char value_common[100];



//��������
void ESP8266_client_link_server(void);

bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime );
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode );
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord );
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx );
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
bool ESP8266_UnvarnishSend ( void );
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );

void ESP8266_Send_Message(char *_string_1, char *_string_2);

void ESP8266_Link_WIFI(void);
void ESP8266_Update_Data(float _data);
void ESP8266_Keep_Linking(void);


#endif


