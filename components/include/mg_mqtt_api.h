
#ifndef _MG_MQTT_API_H_
#define _MG_MQTT_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{   
   const char *client_id;
   const char *hostname;
   uint16_t port;  
   uint16_t keep_alive;
   uint8_t clean_session;
   const char* username;
   const char* password;
   uint16_t timeout;
   uint8_t retry_times;	
}mqtt_client_context;

#ifdef __cplusplus
extern "C" {
#endif

//MG_MQTT_E
int MG_MqttCreate(void);  		
void MG_MqttDestory(void); 						
int MG_MqttConnect(void *mqtt_client_context);
int MG_MqttDisconnect(void);
int MG_MqttSetCallback(void *mqtt_incoming_publish_cb_t,void *mqtt_incoming_data_cb_t);
int MG_MqttSubscribe(const char *topic,uint8_t qos);
int MG_MqttUnsubscribe(const char *topic);
int MG_MqttPublish(const char *topic,const char *message,uint8_t qos,uint8_t remain);


#ifdef __cplusplus
}
#endif

#endif


