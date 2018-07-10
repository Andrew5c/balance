
//ESP8266�ڵ�Ƭ������Ϊ�ͻ��ˣ���������������ݡ�

#include "main.h"


//����WiFi
#define WIFI_NAME      "123"
#define WIFI_PASSWORD  "123456789"

//����Զ�̷�����
#define TCP_SERVER_IP   "192.168.43.95"		//ESP8266��Ϊ�������ĵ�ַ
#define TCP_SERVER_PORT "1234"					//�Զ���˿ں�

#define TCP_LEWEI_IP  "tcp.lewei50.com"		//��Ϊ����Զ�̷���������
#define TCP_LEWEI_PORT "9960"						//�˿ں�


volatile uint8_t ucTcpClosedFlag = 0;
struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


//��¼ע����Ϣ
char userkey[100] = {"{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!"};
//����������
char value_common[100] = {"{\"method\": \"upload\",\"data\":[{\"Name\":\"balance\",\"Value\":\"0\"}]}&^!"};


/*
** ����WiFi�ͷ�����
** ��ʱֻ��������������ʽ�ģ���֪��������������������ĳ���ִ��
*/
void ESP8266_Link_WIFI(void)
{
	//����WiFi
	OLED_Clear();
	OLED_Show_String(0,2,"Linking WIFI...");
	OLED_Show_String(0,4,"***");
	ESP8266_Cmd ( "AT+RST", "ready", "OK", 1000 );
	while ( ! ESP8266_JoinAP ( WIFI_NAME, WIFI_PASSWORD ) );
	OLED_Show_String(0,2,"Linking WIFI OK!");
	OLED_Show_String(0,4,"******");
	Delay_ms(1000);
	
	//���ӷ�����
	OLED_Show_String(0,2,"Linking Web... ");
	OLED_Show_String(0,4,"*********");
	while (! ESP8266_Link_Server( enumTCP, TCP_LEWEI_IP, TCP_LEWEI_PORT, Single_ID_0 ));
	
	//������͸ģʽSTA
	OLED_Show_String(0,2,"Open STA Mode... ");
	OLED_Show_String(0,4,"************");
	while (! ESP8266_Net_Mode_Choose(STA));
	
	//���봩͸ģʽ�շ�
	while (! ESP8266_UnvarnishSend());
	OLED_Show_String(0,2,"Linking Web OK !");
	OLED_Show_String(0,4,"***************");
	LED_Start();
	Delay_ms(1000);
	OLED_Clear();
}

/*
** ���͵�¼��Ϣ�����������������
*/
void ESP8266_Keep_Linking(void)
{
	ESP8266_SendString(ENABLE, userkey, 0, Single_ID_0 );
	Delay_ms(500);
}

/*
** �ϴ����ݣ���Ҫ��ʱ�����ϴ�
*/
void ESP8266_Update_Data(float _data)
{
	char data_buf[100] = {0};
	
	//OLED��ʾ�����ϴ�����
	OLED_Clear();
	OLED_Show_String(0,2,"Update data...");
	OLED_Show_String(0,4,"********");
	Delay_ms(1000);
	sprintf(data_buf,"{\"method\": \"upload\",\"data\":[{\"Name\":\"balance\",\"Value\":\"%.1f\"}]}&^!", _data);
	//�ϴ�����
	ESP8266_SendString(ENABLE, data_buf, 0, Single_ID_0 );	
	//OLED������ʾ
	OLED_Show_String(0,2,"Update data OK!");
	OLED_Show_String(0,4,"***************");
	Delay_ms(1000);
	OLED_Clear();
}


//��Ϊ�ͻ������ӷ�����
void ESP8266_client_link_server(void)
{
//	uint8_t ucStatus;
	
//	char cStr [ 100 ] = { 0 };
		
   printf ( "\r\nConnecting...\r\n" );

	Delay_ms(1000);		//��ʱ2s����ģ���Զ�����WiFi
	
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
//	ESP8266_AT_Test ();

//	ESP8266_Net_Mode_Choose ( STA );

   while ( ! ESP8266_JoinAP ( WIFI_NAME, WIFI_PASSWORD ) );
	
	printf("Connect success!");

//	ESP8266_Enable_MultipleId ( DISABLE );
	
	Delay_ms(2000);
	
	//���ӷ�����
	while (! ESP8266_Link_Server( enumTCP, TCP_LEWEI_IP, TCP_LEWEI_PORT, Single_ID_0 ));
	
	//������͸ģʽSTA
	while (! ESP8266_Net_Mode_Choose(STA));
	
	//���봩͸ģʽ�շ�
	while (! ESP8266_UnvarnishSend());
	
	printf ( "\r\nSending...\r\n" );
	
	//�ȷ���һ�ε�¼��Ϣ
	ESP8266_Send_Message(userkey,value_common);
	
	printf ( "Send Success!\r\n" );
	printf ("\n---------*****-----------\n");
	//�˳���û�м��������ӺͶϿ��������ܣ�ֻ�ܷ���һ�����ݣ����Ͽ����ӡ�
}

//��ʱ�������ݣ���ֹ��ֹ����
void ESP8266_Send_Message(char *_string_1, char *_string_2)
{
	Delay_ms(2000);
	
	//������Ϊ�����涨�ĸ�ʽ������������͵�¼ע����Ϣ����������ҳ��ע����û���ź��Զ�������ر��
	//ESP8266_SendString ( ENABLE, "{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!", 0, Single_ID_0 );   //��������
	ESP8266_SendString(ENABLE, _string_1, 0, Single_ID_0 );
	
	Delay_ms(2000);
	
	//�涨��ʽ���;�������
	//ESP8266_SendString ( ENABLE, "{\"method\": \"upload\",\"data\":[{\"Name\":\"P2\",\"Value\":\"GOOD\"}]}&^!", 0, Single_ID_0 );   //��������
	ESP8266_SendString(ENABLE, _string_2, 0, Single_ID_0 );
	
	Delay_ms(2000);
}

/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ���������Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	macESP8266_Usart( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ�������ݣ���������
		return true;
	
	Delay_ms ( waittime );                 //��ʱ�ȴ�ģ��Ӧ��
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	//printf( "%s", strEsp8266_Fram_Record .Data_RX_BUF );//��ӡ��ģ�����Ӧ
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
				   ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}



/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
}

/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������,��ʱ8266��Ϊ�ͻ���
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch ( enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	 sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );
	
}

/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ����ʱ��8266��Ϊ���������������ֻ���TCP��������
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_UnvarnishSend ( void )
{
	if ( ! ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) )
		return false;
	
	return 
	  ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 500 );
	
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		macESP8266_Usart ( "%s", pStr );
		
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


