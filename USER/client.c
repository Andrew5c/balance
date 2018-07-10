
//ESP8266在单片机端作为客户端，向服务器发送数据。

#include "main.h"


//连接WiFi
#define WIFI_NAME      "123"
#define WIFI_PASSWORD  "123456789"

//连接远程服务器
#define TCP_SERVER_IP   "192.168.43.95"		//ESP8266作为服务器的地址
#define TCP_SERVER_PORT "1234"					//自定义端口号

#define TCP_LEWEI_IP  "tcp.lewei50.com"		//乐为物联远程服务器域名
#define TCP_LEWEI_PORT "9960"						//端口号


volatile uint8_t ucTcpClosedFlag = 0;
struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


//登录注册信息
char userkey[100] = {"{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!"};
//传感器数据
char value_common[100] = {"{\"method\": \"upload\",\"data\":[{\"Name\":\"balance\",\"Value\":\"0\"}]}&^!"};


/*
** 连接WiFi和服务器
** 暂时只能先做成阻塞形式的，不知道如何让他不妨碍其他的程序执行
*/
void ESP8266_Link_WIFI(void)
{
	//连接WiFi
	OLED_Clear();
	OLED_Show_String(0,2,"Linking WIFI...");
	OLED_Show_String(0,4,"***");
	ESP8266_Cmd ( "AT+RST", "ready", "OK", 1000 );
	while ( ! ESP8266_JoinAP ( WIFI_NAME, WIFI_PASSWORD ) );
	OLED_Show_String(0,2,"Linking WIFI OK!");
	OLED_Show_String(0,4,"******");
	Delay_ms(1000);
	
	//连接服务器
	OLED_Show_String(0,2,"Linking Web... ");
	OLED_Show_String(0,4,"*********");
	while (! ESP8266_Link_Server( enumTCP, TCP_LEWEI_IP, TCP_LEWEI_PORT, Single_ID_0 ));
	
	//开启传透模式STA
	OLED_Show_String(0,2,"Open STA Mode... ");
	OLED_Show_String(0,4,"************");
	while (! ESP8266_Net_Mode_Choose(STA));
	
	//进入穿透模式收发
	while (! ESP8266_UnvarnishSend());
	OLED_Show_String(0,2,"Linking Web OK !");
	OLED_Show_String(0,4,"***************");
	LED_Start();
	Delay_ms(1000);
	OLED_Clear();
}

/*
** 发送登录信息保持与服务器的连接
*/
void ESP8266_Keep_Linking(void)
{
	ESP8266_SendString(ENABLE, userkey, 0, Single_ID_0 );
	Delay_ms(500);
}

/*
** 上传数据，需要的时候在上传
*/
void ESP8266_Update_Data(float _data)
{
	char data_buf[100] = {0};
	
	//OLED提示正在上传数据
	OLED_Clear();
	OLED_Show_String(0,2,"Update data...");
	OLED_Show_String(0,4,"********");
	Delay_ms(1000);
	sprintf(data_buf,"{\"method\": \"upload\",\"data\":[{\"Name\":\"balance\",\"Value\":\"%.1f\"}]}&^!", _data);
	//上传数据
	ESP8266_SendString(ENABLE, data_buf, 0, Single_ID_0 );	
	//OLED给出提示
	OLED_Show_String(0,2,"Update data OK!");
	OLED_Show_String(0,4,"***************");
	Delay_ms(1000);
	OLED_Clear();
}


//作为客户端连接服务器
void ESP8266_client_link_server(void)
{
//	uint8_t ucStatus;
	
//	char cStr [ 100 ] = { 0 };
		
   printf ( "\r\nConnecting...\r\n" );

	Delay_ms(1000);		//延时2s，让模块自动连接WiFi
	
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
//	ESP8266_AT_Test ();

//	ESP8266_Net_Mode_Choose ( STA );

   while ( ! ESP8266_JoinAP ( WIFI_NAME, WIFI_PASSWORD ) );
	
	printf("Connect success!");

//	ESP8266_Enable_MultipleId ( DISABLE );
	
	Delay_ms(2000);
	
	//连接服务器
	while (! ESP8266_Link_Server( enumTCP, TCP_LEWEI_IP, TCP_LEWEI_PORT, Single_ID_0 ));
	
	//开启传透模式STA
	while (! ESP8266_Net_Mode_Choose(STA));
	
	//进入穿透模式收发
	while (! ESP8266_UnvarnishSend());
	
	printf ( "\r\nSending...\r\n" );
	
	//先发送一次登录信息
	ESP8266_Send_Message(userkey,value_common);
	
	printf ( "Send Success!\r\n" );
	printf ("\n---------*****-----------\n");
	//此程序没有加入多次连接和断开重连功能，只能发送一次数据，随后断开连接。
}

//定时发送数据，防止终止连接
void ESP8266_Send_Message(char *_string_1, char *_string_2)
{
	Delay_ms(2000);
	
	//按照乐为物联规定的格式，向服务器发送登录注册信息，包括在网页上注册的用户编号和自定义的网关编号
	//ESP8266_SendString ( ENABLE, "{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!", 0, Single_ID_0 );   //发送数据
	ESP8266_SendString(ENABLE, _string_1, 0, Single_ID_0 );
	
	Delay_ms(2000);
	
	//规定格式发送具体数据
	//ESP8266_SendString ( ENABLE, "{\"method\": \"upload\",\"data\":[{\"Name\":\"P2\",\"Value\":\"GOOD\"}]}&^!", 0, Single_ID_0 );   //发送数据
	ESP8266_SendString(ENABLE, _string_2, 0, Single_ID_0 );
	
	Delay_ms(2000);
}

/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的两种响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	macESP8266_Usart( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据，结束函数
		return true;
	
	Delay_ms ( waittime );                 //延时等待模块应答
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	//printf( "%s", strEsp8266_Fram_Record .Data_RX_BUF );//打印出模块的响应
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
				   ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
}

/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器,这时8266作为客户端
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式，这时，8266作为服务器，可以与手机的TCP助手连接
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_UnvarnishSend ( void )
{
	if ( ! ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) )
		return false;
	
	return 
	  ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 500 );
	
}


/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
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


