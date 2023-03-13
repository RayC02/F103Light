#include "esp.h"
#include "usart.h"
#include "emqx.h"
//C��
#include <string.h>
#include <stdio.h>

#define usePassword 1
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"WiFi��SSID\",\"����\"\r\n"
#define ESP8266_EMQX_INFO		"AT+CIPSTART=\"TCP\",\"������IP������\",1883\r\n"  //EMQX��MQTT��������ַ
#define ESP8266_EMQX2_INFO		"AT+CIPSTART=\"TCP\",\"192.168.1.154\",1883\r\n"  //EMQX��MQTT��������ַ
const char *EMQX_SUBSCRIBE_TOPIC[] = {"setting/device2"};			//����ָ��Ļ���



u8 esp8266_buf[1024];  //���ջ��棬ESP8266���ص�ָ��ȷ����Ϣ���������յ�����Ϣ���ڸû�������
u16 esp8266_cnt = 0, esp8266_cntPre = 0;



//	�������ܣ�	��ջ���
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));  //��ջ�����
	esp8266_cnt = 0;
}

//����2����һ���ֽ�
static void ESP8266_SendOneByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart2)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);
	while((((UART_HandleTypeDef *)&huart2)->Instance->SR&0X40)==0);//�ȴ��������
}

//����1����һ���ֽ�
static void Uart_SendOneByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart1)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);
	while((((UART_HandleTypeDef *)&huart1)->Instance->SR&0X40)==0);//�ȴ��������
}

void ESP8266_SendBuf(uint8_t* buf,uint16_t len)
{
	ESP8266_Clear();
	//��������
	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,0xFFFF);
}

//��ESP8266�����ַ���
void ESP8266_SendString(char* str)
{
  ESP8266_Clear();
	//���ͷ���1
	while(*str)		ESP8266_SendOneByte(*str++);
	//���ͷ���2
//	HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),0xFFFF);
}

//	�������ܣ�	�ȴ�ESP8266�������ݣ����ݿ�����ESP8266�ڽ��յ�ָ��󷵻ص�ȷ����Ϣ��Ҳ�����Ǵ������Ͻ��յ���Ϣ REV_OK-�������		REV_WAIT-���ճ�ʱδ���	ѭ�����ü���Ƿ�������
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
		return REV_OK;								//���ؽ�����ɱ�־
	}
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	return REV_WAIT;								//���ؽ���δ��ɱ�־
}


//	�������ܣ�	��������
_Bool ESP8266_SendCmd(u8* cmd, u8 *res, u16 time)
{	
	//HAL_UART_Transmit(&huart1,cmd,strlen((const char *)cmd),0xffff);
	HAL_UART_Transmit(&huart2,cmd,strlen((const char *)cmd),0xffff);
	//while(*cmd)		ESP8266_SendOneByte(*cmd++);
	while(*cmd)		Uart_SendOneByte(*cmd++);
	//ESP8266_SendString((char *)cmd);
	while(time--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			printf((const char *)esp8266_buf);
			if(strstr((const char *)esp8266_buf, (const char *)res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				return 0;
			}
		}
		
		HAL_Delay(10);
	}
	return 1;
}

unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;
	do{
		if(ESP8266_WaitRecive() == REV_OK){								//����������
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL){											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}else{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL){	
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;	
			}
		}
		HAL_Delay(5);
		timeOut--;		//��ʱ�ȴ�
	} while(timeOut > 0);
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��
}








/*				ESP8266���ܳ�ʼ������				*/

void ESP8266_ExitTouChuan(void)
{
	 //�˳�͸��ģʽ
	printf("�˳�͸��ģʽ\r\n");
	ESP8266_SendString("+++");
	HAL_Delay(50);	
	ESP8266_SendString("+++");
	HAL_Delay(50);	
}
void ESP8266_Init_SmartConfig(void)
{	
	ESP8266_Clear();
	ESP8266_ExitTouChuan();
		/* AT���� */
	printf("1. AT\r\n");
	ESP8266_SendCmd("AT\r\n","OK",200);
	HAL_Delay(500);
	printf("1.5 RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n","OK",200);
	HAL_Delay(500);
	ESP8266_Clear();
	printf("2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n","OK",200))
		HAL_Delay(500);
	 /*���� DHCP */
	printf("3.AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK", 200))  //����DHCP
		HAL_Delay(500);
	ESP8266_Clear();
	printf("4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP",200))
		HAL_Delay(500);

	
	ESP8266_Clear();
	/* ������OneNet��������TCP���� */
	printf("7.CIPSTART\r\n");

	while(ESP8266_SendCmd(ESP8266_EMQX_INFO, "CONNECT", 200)) 
		HAL_Delay(500);
	printf("8.CIPMODE\r\n");
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK", 200))  //����͸��
		HAL_Delay(200);
	while(ESP8266_SendCmd("AT+CIPSEND\r\n", ">", 200))
		HAL_Delay(500);
	printf("ESP8266 Init OK\r\n"); 
	ESP8266_Clear();
	
	while(EMQX_DevLink()!=1)
		HAL_Delay(500);
	EMQX_Subscribe(EMQX_SUBSCRIBE_TOPIC, 1);
}




