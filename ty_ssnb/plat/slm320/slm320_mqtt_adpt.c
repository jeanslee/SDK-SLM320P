/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */

#include "MQTTClient.h" //paho mqtt接口
#include "ty_mqtt_adpt.h"



#define MQTT_CMD_TIMIOUT 30000 //推送时如果下行报文量大会导致QoS1的PUBACK收到不到导致PUB失败，MQTTSubscribe中的回调尽量快速处理完

static MQTTClient client = {0};
static Network network = {0};
static bool g_mqttConnected = false;
static MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
static unsigned char sendbuf[MQTT_REQ_BUF_SIZE] = {0};
static unsigned char readbuf[MQTT_RSP_BUF_SIZE] = {0};
static ty_mqtt_recv_submsg_cb_t _submsg_cb = NULL;

#define MQTT_MSG_DEBUG

void nb12messageArrived(MessageData* data)
{
    #ifdef MQTT_MSG_DEBUG
    LOG("Message arrived\n");
//	LOG("Message arrived on topic %.*s: %.*s\r\n", data->topicName->lenstring.len, data->topicName->lenstring.data,\
//		data->message->payloadlen, data->message->payload);
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
    int rc = 0;

    if(!info || !callback || !handle )
        return -1;
    
    _submsg_cb = callback;

    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = info->ClientID;
    connectData.username.cstring = info->user;
    connectData.password.cstring = info->passwd;
    connectData.cleansession = info->CleanSession;
    connectData.keepAliveInterval = info->keepalive;
    
    if(info->usetls)
    {
//        LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        network.tls = true;
        network.psk_or_cert = 1;
        network.auth.tls_ca.ca_cert = info->tls_ca.ca_cert;
        network.auth.tls_ca.cert_len = info->tls_ca.cert_len;

    }
    else
    {
        network.tls = false;
        LOG("%s %d no use tls \r\n",__FUNCTION__,__LINE__);
    }
    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = info->ClientID;
    connectData.username.cstring = info->user;
    connectData.password.cstring = info->passwd;
    
    NetworkInit(&network);
    MQTTClientInit(&client, &network, MQTT_CMD_TIMIOUT, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
    if(g_mqttConnected)
    {
        LOG("disconnect last socket\r\n");
        NetworkDisconnect (&network);
        g_mqttConnected = false;
    }
//    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    LOG("MQTT %s %d\r\n", info->url,info->port);
    if ((rc = NetworkConnect(&network, info->url, info->port)) != 0)
    {
        LOG("Return code from network connect is %d\r\n", rc);
        //需要释放TLS资源
        goto disconnect_socket;
    }
    
    if ((rc = MQTTConnect(&client, &connectData)) != 0)
    {
        LOG("MQTT Connect Fail ret=%d\r\n", rc);
        goto disconnect_socket;
    }
    else
    {
        g_mqttConnected = true;
        LOG("MQTT Connected\r\n");
        if(handle)
            *handle = (void*)&network;
        return rc;
    }
    
disconnect_socket:
    NetworkDisconnect (&network);
    g_mqttConnected = false;
    return rc;
}

/*
int ty_mqtt_will(void *handle,uint8_t retained,uint8_t qos,const char *topic,const char* msg)
{
    connectData.willFlag = 1;
    connectData.will.message.cstring = msg;
    connectData.will.qos = qos;
    connectData.will.retained = retained;
    connectData.will.topicName.cstring = topic;
    
    return 0;
}
*/
int ty_mqtt_sub(void *handle,const char *topic,uint8_t qos)
{   
    LOG("MQTTSubscribe %s\r\n",topic);
    //int rc;
    //if ((rc = MQTTSubscribe(&client, "FreeRTOS/sample/#", 2, messageArrived)) != 0)
	//	LOG("Return code from MQTT subscribe is %d\n", rc);
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
    LOG("MQTTDisconnect\r\n");
    MQTTDisconnect (&client);
    NetworkDisconnect (&network);
    LOG("NetworkDisconnect\r\n");
    g_mqttConnected = false;
    *handle = NULL;
}

int ty_mqtt_status (void *handle)
{
    int s =  MQTTIsConnected(&client);
    //LOG("MQTTIsConnected=%d\r\n",s);
    return s;
}

int ty_mqtt_yield (void *handle,int timeout_ms)
{
    return MQTTYield(&client, timeout_ms);
}

