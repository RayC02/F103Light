#ifndef __ESP_H_
#define __ESP_H_

#include "main.h"


#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

extern unsigned char esp8266_buf[1024];
extern u16 esp8266_cnt;
void ESP8266_Clear(void);
static void ESP8266_SendOneByte(uint8_t val);
void ESP8266_SendBuf(uint8_t* buf,uint16_t len);
void ESP8266_SendString(char* str);
void ESP8266_Init_SmartConfig(void);
void ESP8266_Init_after_SmartConfig(void);
unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void Net_Init(void);
#endif
