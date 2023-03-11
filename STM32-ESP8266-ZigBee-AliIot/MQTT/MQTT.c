#include "mqtt.h"
#include "common.h"
#include "MQTTPacket.h"
#include "key.h"
#include "text.h"
#include "transport.h"
#include "cjson.h"


/*****************************************************************************
 *	�û�����  */

// ��������
//const char *WIFI_SSID = "Baniliy-AP";
//const char *WIFI_Password = "L3!8715c.";
const char *WIFI_SSID = "Baniliy- iPhone";
const char *WIFI_Password = "yu12345678..";
// ���ĺͷ���Topic 
#define Set_Topic "/sys/h3cyOqbsuEM/Test/thing/service/property/set"
#define Post_Topic "/sys/h3cyOqbsuEM/Test/thing/event/property/post"
// Payload����
u16 TopicId = 0;		
char payload[100];
/*****************************************************************************
 *	����3�ʹ���2���յ�����
 */
char receive_data[100];
char receive_data1[100];
unsigned char buf[200];

/*****************************************************************************
 *	�Զ���ṹ�壨�ն���Ϣ��
 */
 /*
struct End
{
	u8 No;		// �ն˱��
	u8 RAIN;	// ������Ϣ
	u8 RELAY;	// �̵���״̬
};*/
//struct End EndInfo[END_MAX_NUM];

u8 Auto_MODE = 0;	// ����ģʽ
u8 RAIN;			// ������Ϣ
u8 RELAY1;			// �̵���״̬
u8 RELAY2;			// �̵���״̬

/*----------------------------------------------------------*/
/*������������Ŀ��MQTT������									*/
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Connect()
{
    int32_t len;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;//���ò��ֿɱ�ͷ����ֵ
    
    int buflen = sizeof(buf);
	memset(buf,0,buflen); 
    data.clientID.cstring = "h3cyOqbsuEM.Test|securemode=2,signmethod=hmacsha256,timestamp=1664431040843|";//�ͻ��˱�ʶ����������ÿ���ͻ���xxxΪ�Զ��壬����Ϊ�̶���ʽ
    data.keepAliveInterval = 120;//�����ʱ���������˷������յ��ͻ�����Ϣ�����ʱ����
    data.cleansession = 1;//�ñ�־��1���������붪��֮ǰ���ֵĿͻ��˵���Ϣ������������Ϊ�������ڡ�
    data.username.cstring = "Test&h3cyOqbsuEM";//�û��� DeviceName&ProductKey
    data.password.cstring = "774496b77794e18b66cd1902e4a2183ac4fc9b9b81e17dbd1814026b7953afe4";//����
    len = MQTTSerialize_connect(buf, buflen, &data); /*1 �������ӱ���*/
    transport_sendPacketBuffer(USART3,buf,len);
	printf("\r\nMQTT���ӳɹ�");
}

/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����								*/
/*��  ����temp_buff�����汨�����ݵĻ�����					*/
/*��  ����QoS�����ĵȼ�										*/
/*��  ����topic_name������topic��������						*/
/*����ֵ�����ĳ���                                                                                                          */
/*----------------------------------------------------------*/
void MQTT_Subscribe(unsigned char *temp_buff, char *topic_name, int QoS)
{
	u8 Fixed_len = 2;                                 //SUBSCRIBE�����У��̶���ͷ����=2
	u8 Variable_len = 2;                              //SUBSCRIBE�����У��ɱ䱨ͷ����=2
	u8 Payload_len = 2 + strlen(topic_name) + 1;   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�

	temp_buff[0]=0x82;                                          //��1���ֽ� ���̶�0x82
	temp_buff[1]=Variable_len + Payload_len;                    //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���
	temp_buff[2]=0x00;                                          //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3]=0x01;		                                    //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4]=strlen(topic_name)/256;                     //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5]=strlen(topic_name)%256;		                //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�
	temp_buff[6+strlen(topic_name)]=QoS;                     //���1���ֽڣ����ĵȼ�
	
	transport_sendPacketBuffer(USART3,buf,Fixed_len + Variable_len + Payload_len);
	printf("\r\n��������ɹ�");
}

/*----------------------------------------------------------*/
/*��������������Ϣ                 			                */
/*��  ����pTopic������Topic									*/
/*��  ����pMessage��������Ϣ									*/
/*��  ����QoS�����ĵȼ�			                            */
/*����ֵ����Ϣ�����Ƿ�ɹ�									*/
/*----------------------------------------------------------*/
//������Ϣ
int MQTT_Publish(char *pTopic,char *pMessage,int QoS)
{
    int32_t len;
    MQTTString topicString = MQTTString_initializer;
    int msglen = strlen(pMessage);//���㷢����Ϣ�ĳ���
    int buflen = sizeof(buf);

    memset(buf,0,buflen);
    topicString.cstring = pTopic;//"�豸topic�б��еĵ�һ��";
    len = MQTTSerialize_publish(buf, buflen, 0, QoS, 0, 0, topicString, (unsigned char*)pMessage, msglen); /*���췢����Ϣ�ı���*/

    transport_sendPacketBuffer(USART3,buf,len);//������Ϣ
	free(pMessage);
    return 0;
}

/*----------------------------------------------------------*/
/*��������PING���ģ�������									*/
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void MQTT_PingREQ()
{
	while((USART3->SR&0x0080)==0) ;
	USART3->DR=0xC0;
	while((USART3->SR&0x0080)==0) ;
	USART3->DR=0x00;
}

/*----------------------------------------------------------*/
/*��������JSON����		                					*/
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void Json_Create()
{   
	sprintf(payload,"{\"id\": %013d,\"params\":{\"RAIN\":%d,\"RELAY1\":%d,\"RELAY2\":%d,\"MODE\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}", 
		++TopicId,RAIN,RELAY1,RELAY2,Auto_MODE);		// ����JSON�ַ���		
	printf("\r\n>>> BANILIY ���� %s \r\n", payload);
	MQTT_Publish(Post_Topic,payload,1);					// ������Ϣ
}

/*----------------------------------------------------------*/
/*��������JSON������������Э��������������					*/
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void Json_Parse_ZigBee(char *buffer)
{
	int i=0,j=0,k=0;
	sscanf( buffer, "{\"RAIN\":%d,\"RELAY1\":%d,\"RELAY2\":%d}", &i,&j,&k); // ����JSON�ַ���
	RAIN = i;
	RELAY1 = j;
	RELAY2 = k;
	printf("\r\n>ZIGBEE  %s",buffer);
}

/*----------------------------------------------------------*/
/*��������JSON�������������ƶ˷���������					*/
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void Json_Parse_Aliyun()
{
	//�жϽ��յ���������û��Ŀ������
	if(strstr((const char*)receive_data ,(const char*)"params")) 	
	{
		printf("\r\n >ALIYUN  %s",receive_data);
	}
	if(strstr((const char*)receive_data ,(const char*)"\"RELAY1\":1")) 	
	{
		u2_printf("21\n\r");	//��������
	}
	if(strstr((const char*)receive_data ,(const char*)"\"RELAY1\":0")) 	
	{
		u2_printf("20\n\r");	//��������
	}
	if(strstr((const char*)receive_data ,(const char*)"\"RELAY2\":1")) 	
	{
		u2_printf("31\n\r");	//��������
	}
	if(strstr((const char*)receive_data ,(const char*)"\"RELAY2\":0")) 	
	{
		u2_printf("30\n\r");	//��������
	}
	if(strstr((const char*)receive_data ,(const char*)"\"MODE\":1")) 	
	{
		Auto_MODE = 1;
	}
	if(strstr((const char*)receive_data ,(const char*)"\"MODE\":0")) 	
	{
		Auto_MODE = 0;
	}	
}

/*----------------------------------------------------------*/
/*����������ʼ��ESP8266 �������绷��		                */
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void MQTT_Init()
{
	char Connect_CMD[50];
	while(atk_8266_send_cmd("ATE0","OK",20));				//�رջ���
	while(atk_8266_send_cmd("AT+CWMODE=1","OK",20));		//��Ϊ�ͻ���ģʽ
	printf("\r\n���ڳ�������WIFI  >>>%s<<< ",WIFI_SSID);
	sprintf(Connect_CMD,"AT+CWJAP=\"%s\",\"%s\"",WIFI_SSID,WIFI_Password);
	printf("\r\n %s <<< \r\n",Connect_CMD);
	while(atk_8266_send_cmd((u8 *)Connect_CMD,"OK",18));			//����WIFI
	printf("\r\nWIFI���ӳɹ�");
	while(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"iot-06z00i5jz65jb3w.mqtt.iothub.aliyuncs.com\",1883","OK",20));	// ���ӷ�����
	delay_ms (1000);
	while(atk_8266_send_cmd("AT+CIPMODE=1","OK",20));		//��͸��ģʽ
	delay_ms (1000);
	while(atk_8266_send_cmd("AT+CIPSEND",">",20));			//��ʼ��������
	printf("\r\n��ʼ��������");
	delay_ms (1000);
	free(Connect_CMD);
}

/*----------------------------------------------------------*/
/*����������������		                    				*/
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void MQTT_HandleKeys()
{
	u8 key;		// ��Ӧ�İ���
	key=KEY_Scan(0); 
	if(key)
	{
		printf("\r\n��⵽������Ӧ��%d						",key);
		switch(key)
		{
			case 1://KEY0
				u2_printf("21\n\r");	//��������
				printf("\r\n����ͳɹ�!\r\n");
				break;
			case 2://KEY1
				u2_printf("31\n\r");	//��������
				printf("\r\n����ͳɹ�!\r\n");
				break;
			case 4://WK_UP
				Json_Create();
				printf("\r\n �����ɹ�! ");
				break;
		}
	}
}

/*----------------------------------------------------------*/
/*����������������ģʽ1ʱ���Զ������������Ƽ̵���״̬		 */
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void RAIN_Auto_Process()
{
	if(Auto_MODE == 1)
	{
		if(RAIN == 1)
		{
			u2_printf("21\n");	//��������
			u2_printf("31\n");
			RELAY1 = 1;
			RELAY2 = 1;
		}
		else
		{
			u2_printf("20\n");	//��������
			u2_printf("30\n");
			RELAY1 = 0;
			RELAY2 = 0;
		}
	}
}

/*----------------------------------------------------------*/
/*��������������ͨ��		                    			*/
/*��  ������												*/
/*����ֵ����												*/
/*----------------------------------------------------------*/
void MQTT_Process()
{
	u16 k = 0;
	MQTT_Init();						// ��ʼ��MQTT
	MQTT_Connect();						// ����Ŀ��MQTT������
	delay_ms(1000);
	MQTT_Subscribe(buf,Set_Topic,1);	// ��������
	
	while(1)
	{
		LED0=0;
		LED1=0;
		delay_ms(10);							
		if(k%15){LED0 = 1;Json_Parse_Aliyun();}				// �����ƶ�����
		if(USART2_RX_STA&0X8000)							// ����2���յ�����
		{ 		
			USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;			// ��ӽ�����
			Json_Parse_ZigBee((char *)USART2_RX_BUF);		// �����ն�����
			USART2_RX_STA=0;	
			if(k%10){LED1 = 1;}
		}
		RAIN_Auto_Process();						// ���������Զ�����
		MQTT_HandleKeys();							// ������Ӧ
		k++;  
		if(k%90==0){Json_Create();k=0;}				// ���ڷ�����Ϣ
	}
}

