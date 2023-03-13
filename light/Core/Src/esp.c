#include "esp.h"
#include "usart.h"
#include "emqx.h"
//C库
#include <string.h>
#include <stdio.h>

#define usePassword 1
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"WiFi的SSID\",\"密码\"\r\n"
#define ESP8266_EMQX_INFO		"AT+CIPSTART=\"TCP\",\"服务器IP填这里\",1883\r\n"  //EMQX的MQTT服务器地址
#define ESP8266_EMQX2_INFO		"AT+CIPSTART=\"TCP\",\"192.168.1.154\",1883\r\n"  //EMQX的MQTT服务器地址
const char *EMQX_SUBSCRIBE_TOPIC[] = {"setting/device2"};			//接收指令的话题



u8 esp8266_buf[1024];  //接收缓存，ESP8266返回的指令确认信息或从网络接收到的信息放在该缓冲区中
u16 esp8266_cnt = 0, esp8266_cntPre = 0;



//	函数功能：	清空缓存
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));  //清空缓冲区
	esp8266_cnt = 0;
}

//串口2发送一个字节
static void ESP8266_SendOneByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart2)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);
	while((((UART_HandleTypeDef *)&huart2)->Instance->SR&0X40)==0);//等待发送完成
}

//串口1发送一个字节
static void Uart_SendOneByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart1)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);
	while((((UART_HandleTypeDef *)&huart1)->Instance->SR&0X40)==0);//等待发送完成
}

void ESP8266_SendBuf(uint8_t* buf,uint16_t len)
{
	ESP8266_Clear();
	//定长发送
	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,0xFFFF);
}

//向ESP8266发送字符串
void ESP8266_SendString(char* str)
{
  ESP8266_Clear();
	//发送方法1
	while(*str)		ESP8266_SendOneByte(*str++);
	//发送法法2
//	HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),0xFFFF);
}

//	函数功能：	等待ESP8266接收数据，数据可能是ESP8266在接收到指令后返回的确认信息，也可能是从网络上接收的信息 REV_OK-接收完成		REV_WAIT-接收超时未完成	循环调用检测是否接收完成
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
		return REV_OK;								//返回接收完成标志
	}
	esp8266_cntPre = esp8266_cnt;					//置为相同
	return REV_WAIT;								//返回接收未完成标志
}


//	函数功能：	发送命令
_Bool ESP8266_SendCmd(u8* cmd, u8 *res, u16 time)
{	
	//HAL_UART_Transmit(&huart1,cmd,strlen((const char *)cmd),0xffff);
	HAL_UART_Transmit(&huart2,cmd,strlen((const char *)cmd),0xffff);
	//while(*cmd)		ESP8266_SendOneByte(*cmd++);
	while(*cmd)		Uart_SendOneByte(*cmd++);
	//ESP8266_SendString((char *)cmd);
	while(time--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			printf((const char *)esp8266_buf);
			if(strstr((const char *)esp8266_buf, (const char *)res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
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
		if(ESP8266_WaitRecive() == REV_OK){								//如果接收完成
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL){											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}else{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL){	
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;	
			}
		}
		HAL_Delay(5);
		timeOut--;		//延时等待
	} while(timeOut > 0);
	return NULL;														//超时还未找到，返回空指针
}








/*				ESP8266功能初始化设置				*/

void ESP8266_ExitTouChuan(void)
{
	 //退出透传模式
	printf("退出透传模式\r\n");
	ESP8266_SendString("+++");
	HAL_Delay(50);	
	ESP8266_SendString("+++");
	HAL_Delay(50);	
}
void ESP8266_Init_SmartConfig(void)
{	
	ESP8266_Clear();
	ESP8266_ExitTouChuan();
		/* AT测试 */
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
	 /*开启 DHCP */
	printf("3.AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK", 200))  //开启DHCP
		HAL_Delay(500);
	ESP8266_Clear();
	printf("4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP",200))
		HAL_Delay(500);

	
	ESP8266_Clear();
	/* 建立与OneNet服务器的TCP连接 */
	printf("7.CIPSTART\r\n");

	while(ESP8266_SendCmd(ESP8266_EMQX_INFO, "CONNECT", 200)) 
		HAL_Delay(500);
	printf("8.CIPMODE\r\n");
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK", 200))  //开启透传
		HAL_Delay(200);
	while(ESP8266_SendCmd("AT+CIPSEND\r\n", ">", 200))
		HAL_Delay(500);
	printf("ESP8266 Init OK\r\n"); 
	ESP8266_Clear();
	
	while(EMQX_DevLink()!=1)
		HAL_Delay(500);
	EMQX_Subscribe(EMQX_SUBSCRIBE_TOPIC, 1);
}




