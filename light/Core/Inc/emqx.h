#ifndef __EMQX_H
#define __EMQX_H

#include "main.h"


_Bool EMQX_DevLink(void);													//��EMQXƽ̨��������
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt);	//��������
void EMQX_Publish(const char *topic, const char *msg);					//��������
void EMQX_RevPro(unsigned char *cmd);									//��ƽ̨���ص����ݽ��в������ȡ��Ҫ������

#endif
