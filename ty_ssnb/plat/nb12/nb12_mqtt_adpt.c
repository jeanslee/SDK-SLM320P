/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */

#include "MQTTClient.h"
#include "ty_mqtt_basic_info.h"
#include "softap_api.h"
#include "ty_ss_cloud.h"


#define MQTT_CMD_TIMIOUT 20000 //推送时如果下行报文量大会导致QoS1的PUBACK收到不到导致PUB失败，MQTTSubscribe中的回调尽量快速处理完

static MQTTClient client = {0};
static Network network = {0};
static bool g_mqttConnected = false;
static MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
static unsigned char sendbuf[MQTT_REQ_BUF_SIZE] = {0};
static unsigned char readbuf[MQTT_RSP_BUF_SIZE] = {0};
static ty_mqtt_recv_submsg_cb_t _submsg_cb = NULL;

//#define MQTT_MSG_DEBUG

void nb12messageArrived(MessageData* data)
{
    #ifdef MQTT_MSG_DEBUG
    xy_printf("Message arrived\n");
	//xy_printf("Message arrived on topic %.*s: %.*s\r\n", data->topicName->lenstring.len, data->topicName->lenstring.data,
	//	data->message->payloadlen, data->message->payload);
    #endif
    if(_submsg_cb != NULL)
    {
        _submsg_cb(0,data->topicName->lenstring.data,data->topicName->lenstring.len,data->message->payloadlen,data->message->payload);
    }
}
/*
int ty_mqtt_connect(const char *user,const char *passwd,const char *ClientID,
const char *DestAddr,uint16_t  DestPort,uint8_t CLeanSession,
uint16_t keepalive,uint8_t UseTls)
*/
int ty_mqtt_connect(ty_mqtt_cnt_info *info, ty_mqtt_recv_submsg_cb_t callback,void **handle)
{
    
    int rc = 0, 
    count = 0;
    static bool init_flag = false;
    static bool task_init_flag = false;

    _submsg_cb = callback;

    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = info->ClientID;
    connectData.username.cstring = info->user;
    connectData.password.cstring = info->passwd;
    connectData.cleansession = info->CleanSession;
    connectData.keepAliveInterval = info->keepalive;
    
    if(info->usetls)
    {

        network.tls = true;
        network.psk_or_cert = 1;
        network.auth.tls_ca.ca_cert = info->tls_ca.ca_cert;
        network.auth.tls_ca.cert_len = info->tls_ca.cert_len;
        connectData.MQTTVersion = 3;
        connectData.clientID.cstring = info->ClientID;
        connectData.username.cstring = info->user;
        connectData.password.cstring = info->passwd;

     }
    
    if(!init_flag)
    {
        NetworkInit(&network);
        MQTTClientInit(&client, &network, MQTT_CMD_TIMIOUT, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
        init_flag = true;
    }
    if(g_mqttConnected)
    {
        xy_printf("disconnect last socket\r\n");
        NetworkDisconnect (&network);
        g_mqttConnected = false;
    }
    xy_printf("MQTT %s %d\r\n", info->url,info->port);
    if ((rc = NetworkConnect(&network, info->url, info->port)) != 0)
    {
        xy_printf("Return code from network connect is %d\r\n", rc);
        //需要释放TLS资源
        goto disconnect_socket;
    }
    
    if ((rc = MQTTConnect(&client, &connectData)) != 0)
    {
        xy_printf("MQTT Connect Fail ret=%d\r\n", rc);
        goto disconnect_socket;
    }
    else
    {
        g_mqttConnected = true;
        xy_printf("MQTT Connected\r\n");
        if(!task_init_flag)
        {
#if defined(MQTT_TASK)
            if ((rc = MQTTStartTask(&client)) != 0)
            {
                xy_printf("Return code from start tasks is %d\r\n", rc);
                goto disconnect_socket;
            }
            task_init_flag = true;
#endif
        }
        if(handle)
            *handle = (void*)&network;
        return rc;
    }
    
disconnect_socket:
    NetworkDisconnect (&network);
    g_mqttConnected = false;
    return rc;
}

int ty_mqtt_will(void *handle,uint8_t retained,uint8_t qos,const char *topic,const char* msg)
{
    connectData.willFlag = 1;
    connectData.will.message.cstring = msg;
    connectData.will.qos = qos;
    connectData.will.retained = retained;
    connectData.will.topicName.cstring = topic;
    
    return 0;
}

int ty_mqtt_sub(void *handle,const char *topic,uint8_t qos)
{   
    xy_printf("MQTTSubscribe %s\r\n",topic);
    //int rc;
    //if ((rc = MQTTSubscribe(&client, "FreeRTOS/sample/#", 2, messageArrived)) != 0)
	//	xy_printf("Return code from MQTT subscribe is %d\n", rc);
    return MQTTSubscribe(&client, topic, qos, nb12messageArrived);
}

int ty_mqtt_unsub(void *handle,const char *topic)
{
    return MQTTUnsubscribe (&client, topic);
}

int ty_mqtt_pub(void *handle,char *topic,uint8_t qos,bool retain,const char * msg )
{
    MQTTMessage message = {0};
    message.qos = qos;
    message.retained = retain;
    message.payload = msg;
    //sprintf(payload, "message number %d", count);
    message.payloadlen = strlen(msg);

    return MQTTPublish(&client, topic, &message);
}

void ty_mqtt_close(void **handle)
{
    if(!(handle && *handle))
    {
        LOG("invalid mqtt handle!\r\n");
        return ;
    }
    xy_printf("MQTTDisconnect\r\n");
    MQTTDisconnect (&client);
    NetworkDisconnect (&network);
    xy_printf("NetworkDisconnect\r\n");
    g_mqttConnected = false;
    *handle = NULL;
}

int ty_mqtt_status (void *handle)
{
    int s =  MQTTIsConnected(&client);
    //xy_printf("MQTTIsConnected=%d\r\n",s);
    return s;
}

int ty_mqtt_yield (void *handle,int timeout_ms)
{
    return MQTTYield(&client, timeout_ms);
}



