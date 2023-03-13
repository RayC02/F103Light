#include "emqx.h"
#include "main.h"
#include "esp.h"
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "cJSON.h"
#include "tim.h"
#include "mqttkit.h"


extern unsigned char esp8266_buf[1024];

#define EMQX_USARNAME "RayC02"
#define EMQX_CLIENTID "device2"
#define EMQX_PASSWD "587FF020E80FC95D1EBE0864A6E734D45F6D6D8E"
#define	EMQX_PUBLISH_MSG "message/device2"
char mqtt_message[300];
int ret=0;
u8 TEXT_Buffer[] = {"hello"};

_Bool EMQX_DevLink(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包
	unsigned char *dataPtr;
	_Bool status = 1;
	
	printf( "EMQX_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
					, EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketConnect(EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0){
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);			//将协议包中_data指针指向的数据通过串口上传平台
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK){
				switch(MQTT_UnPacketConnectAck(dataPtr)){
					case 0:printf( "Tips:	连接成功\r\n");status = 0;break;	
					case 1:printf( "WARN:	连接失败：协议错误\r\n");break;
					case 2:printf( "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf( "WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf( "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf( "WARN:	连接失败：非法链接(比如token非法)\r\n");break;	
					default:printf( "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	return status;
}


//==========================================================
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================

void EMQX_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
//	printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}


void EMQX_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	cJSON* json, *json_value;
	
	char* array[]={"open","close"};//所订阅的主题即将要发送过来的数据的键
	int i=0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				printf( "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);	
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				printf("topic: %s,  payload: %s, payload_len: %d\r\n",
																	cmdid_topic,  req_payload, req_len);
				json = cJSON_Parse(req_payload);

				if(!json)printf( "Error: before:[%s]\n", cJSON_GetErrorPtr());
				else{
					/*例如：获取json列表中键为array[i]的对象*/
					for(;i<25;i++){
						json_value = cJSON_GetObjectItem(json, array[i]);

						if(json_value!=NULL)
							break;
					}
					if(strstr(json_value->string, "open") != NULL){
						if(json_value->valueint==1){
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,1500);  
							ret=1;
						}else{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,800);  
							ret=1;
						}	
						sprintf(mqtt_message,"{\"SetState\": %d}",ret);
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
						ret=0;
					}
	
					if(json_value!=NULL){
						printf( "json_string = %s,json_int = %d\r\n", json_value->string,json_value->valueint);
					}	
				}
				cJSON_Delete(json);
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				printf( "Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					printf( "Tips:	Send PublishRel\r\n");
					ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				printf( "Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					printf( "Tips:	Send PublishComp\r\n");
					ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				printf( "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf( "Tips:	MQTT Subscribe OK\r\n");
			else
				printf( "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				printf( "Tips:	MQTT UnSubscribe OK\r\n");
			else
				printf( "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

