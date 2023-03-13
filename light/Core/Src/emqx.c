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
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���
	unsigned char *dataPtr;
	_Bool status = 1;
	
	printf( "EMQX_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
					, EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID);
	/*���ط�0��ʾδ�ܸ�mqttPacketЭ�����ȫ�������*/
	if(MQTT_PacketConnect(EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0){
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);			//��Э�����_dataָ��ָ�������ͨ�������ϴ�ƽ̨
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK){
				switch(MQTT_UnPacketConnectAck(dataPtr)){
					case 0:printf( "Tips:	���ӳɹ�\r\n");status = 0;break;	
					case 1:printf( "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:printf( "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:printf( "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:printf( "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:printf( "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;	
					default:printf( "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	return status;
}


//==========================================================
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	/*���ط�0��ʾδ�ܸ�mqttPacketЭ�����ȫ�������*/
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================

void EMQX_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
//	printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	/*���ط�0��ʾδ�ܸ�mqttPacketЭ�����ȫ�������*/
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}


void EMQX_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	cJSON* json, *json_value;
	
	char* array[]={"open","close"};//�����ĵ����⼴��Ҫ���͹��������ݵļ�
	int i=0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				printf( "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);	
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				printf("topic: %s,  payload: %s, payload_len: %d\r\n",
																	cmdid_topic,  req_payload, req_len);
				json = cJSON_Parse(req_payload);

				if(!json)printf( "Error: before:[%s]\n", cJSON_GetErrorPtr());
				else{
					/*���磺��ȡjson�б��м�Ϊarray[i]�Ķ���*/
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
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
		
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
			
		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
			
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
		
		case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				printf( "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf( "Tips:	MQTT Subscribe OK\r\n");
			else
				printf( "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				printf( "Tips:	MQTT UnSubscribe OK\r\n");
			else
				printf( "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

