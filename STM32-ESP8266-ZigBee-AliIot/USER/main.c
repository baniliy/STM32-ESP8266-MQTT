#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"	
#include "touch.h"	
#include "usart2.h" 
#include "usart3.h"
#include "common.h" 
#include "MQTTPacket.h"
#include "mqtt.h"
#include "cjson.h"


/************************************************

 �������ۺ�ʵѵ
 AUTHOR: BANILIY			CREATE DATE��2022-09-26
 
 �����壺					ALIENTEK STM32F103 
 WIFI��						ESP8266 
 �ն�Э������				ZigBee 
 ��������ƽ̨��				Ali-IOT
 �����ĵ���https://www.yuque.com/docs/share/3488aa6b-e126-4e8a-a9c6-a47a6ebee61b?# ���������ۺ���Ŀʵѵ��


************************************************/


 int main(void)
 {	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200	
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	usart2_init(115200);		//��ʼ������2
	usart3_init(115200);		//��ʼ������3 
	
	printf("\r\n---------------------------------------------------\r\n");
	printf("\r\n>>>    BANILIY  �������ۺ���Ŀʵѵ    <<<\r\n");
	printf("\r\n---------------------------------------------------\r\n");
	printf("\r\n����ģ��...\r\n"); 
	 while( atk_8266_send_cmd("AT","OK",20))			//���WIFIģ���Ƿ�����
	{
		atk_8266_send_cmd("AT+RST","OK",20);		//����ģ�� 
		delay_ms(1000);								//��ʱ2S�ȴ������ɹ�
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		atk_8266_quit_trans();						//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);	//�ر�͸��ģʽ	
		printf("\r\nδ��⵽ģ��!!!\r\n"); 
		delay_ms(800);
		Show_Str(40,55,200,16,"��������ģ��...",16,0);
		printf("\r\n��������ģ��...\r\n"); 
	} 
	// ����������
	while(1)
	{
		MQTT_Process();
	}
	
}


















