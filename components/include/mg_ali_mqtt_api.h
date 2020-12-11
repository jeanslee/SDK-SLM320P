
#ifndef _MG_ALI_MQTT_API_H_
#define _MG_ALI_MQTT_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lwip_mqtt_api.h"

#define MQTT_ALI_CLIENT_ID_SIZE 64
#define MQTT_ALI_DEVICE_NAME_SIZE 64
#define MQTT_ALI_PRODUCT_KEY_SIZE 64
#define MQTT_ALI_DEVICE_SECRET_SIZE MQTT_STRING_SIZE
#define MQTT_ALI_TOPIC_SIZE 128

typedef struct
{
   struct mqtt_connect_client_info_t client_info;
   const char *ali_product_key;
   const char* ali_device_name;
   const char* ali_device_secret;
   u8_t client_idx;
   u16_t port;
   const char *hostname;
   u16_t msg_id;
   
   u8_t vsn;
   u8_t cid;
   u8_t will_flag;
   u8_t pkt_timeout;
   u8_t retry_times;
   u8_t timeout_notice;
   u8_t ssl_enable;
   u8_t sslctx_idx;
   u8_t msg_recv_mode;
   u8_t msg_len_enable;
   const char* username;
   const char* password;
   const char* topic;
   u16_t msg_len;
   
}mqtt_ali_client_info;

#ifdef __cplusplus
extern "C" {
#endif



int MG_AliMqttCreate(void *mqtt_client_info);
void MG_AliMqttDestory(void);
int MG_AliMqttOpen(void *mqtt_client_info);
void MG_AliMqttClose(void);

int MG_AliMqttConnect(void *mqtt_client_info,uint8_t securemode,uint8_t signmethod);
int MG_AliMqttDisconnect(void);
int MG_AliMqttSetCallback(void *mqtt_incoming_publish_cb_t,void *mqtt_incoming_data_cb_t);
int MG_AliMqttSubscribe(const char *topic,uint8_t qos);
int MG_AliMqttUnsubscribe(const char *topic);
int MG_AliMqttPublish(const char *topic,const char *message,uint8_t qos,uint8_t remain);

#ifdef __cplusplus
}
#endif

#endif


