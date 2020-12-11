#ifndef __TY_MQTT_ADPT_H__
#define __TY_MQTT_ADPT_H__

#include "stdint.h"
#include "stdbool.h"

#define MQTT_PUB_QOS            1
#define MQTT_SUB_QOS            0

#define MQTT_REQ_BUF_SIZE 2048
#define MQTT_RSP_BUF_SIZE 2048


typedef struct 
{
	const char *url; 
	uint16_t port;
	bool usetls;
    char user[64] ;
    char passwd[65] ;
    char ClientID[150];
	bool CleanSession;
	uint16_t keepalive;
    struct
    {
		const unsigned char *ca_cert;
		uint32_t cert_len;
    }tls_ca;
}ty_mqtt_cnt_info;

//mqtt  ≈‰Ω”ø⁄
typedef int (* ty_mqtt_recv_submsg_cb_t)(int msgID,const char* topic,int topicLen,int msgLen,const char *message);

void ty_mqtt_sub_msg_cb(char *data);

int ty_mqtt_connect(ty_mqtt_cnt_info *info, ty_mqtt_recv_submsg_cb_t callback,void **handle);

//int ty_mqtt_will(void *handle,uint8_t retained,uint8_t qos,const char *topic,const char* msg);

int ty_mqtt_sub(void *handle,const char *topic,uint8_t qos);

int ty_mqtt_unsub(void *handle,const char *topic);

int ty_mqtt_pub(void *handle,char *topic,uint8_t qos,bool retain,const char * message);

void ty_mqtt_close(void **handle);

int ty_mqtt_status (void *handle);

int ty_mqtt_yield (void *handle,int timeout_ms);



#endif



