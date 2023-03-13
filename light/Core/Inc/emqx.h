#ifndef __EMQX_H
#define __EMQX_H

#include "main.h"


_Bool EMQX_DevLink(void);													//与EMQX平台建立连接
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt);	//订阅主题
void EMQX_Publish(const char *topic, const char *msg);					//发布主题
void EMQX_RevPro(unsigned char *cmd);									//将平台返回的数据进行拆包，获取想要的数据

#endif
