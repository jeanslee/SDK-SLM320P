/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */
 
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Q', 'T', 'T')
 
#include "osi_api.h"
#include "osi_log.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mg_ali_mqtt_api.h"

#define MG_MQTT_SUCCESS 0


mqtt_ali_client_info MG_mqtt_ali_client_info_t;

static char g_Mqtt_Topic[64];
static uint8_t g_mqtt_msg_count=0;    

void MG_mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t total_len)
{
    memset(g_Mqtt_Topic, 0, sizeof(g_Mqtt_Topic));
    if (strlen(topic) < sizeof(g_Mqtt_Topic) - 1)
    {
        strcpy(g_Mqtt_Topic, topic);
    }
    else
    {
        strncpy(g_Mqtt_Topic, topic, sizeof(g_Mqtt_Topic) - 5);
        strncpy(g_Mqtt_Topic+sizeof(g_Mqtt_Topic) - 5,"...", 3);
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0, "MG_mqtt_incoming_publish_cb():g_Mqtt_Topic:%s",g_Mqtt_Topic);
    return;
}

void MG_mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t data_len, uint8_t flags)
{
    char *urc_msg = NULL;
    unsigned int tmplen = 0;
    unsigned int urc_len = data_len + strlen(g_Mqtt_Topic) + 8;

    g_mqtt_msg_count++;
    urc_msg = (char *)malloc(urc_len);
    if (NULL == urc_msg)
    {
        OSI_LOGI(0, "MG_mqtt_incoming_data_cb() line[%d]:Error:Malloc false", __LINE__);
        return;
    }
    else
    {
        memset(urc_msg, 0, urc_len);
        strncpy(urc_msg, g_Mqtt_Topic,strlen(g_Mqtt_Topic));
        strncpy(urc_msg+strlen(g_Mqtt_Topic), ":",1);
        strncpy(urc_msg+strlen(g_Mqtt_Topic)+1, (char *)data,data_len);
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0, "MG_mqtt_incoming_data_cb():urc_msg:%s",urc_msg);
    free(urc_msg);
    memset(g_Mqtt_Topic, 0, sizeof(g_Mqtt_Topic));

    return;
}



static void prvThreadEntry(void *param)
{

    bool active = false;
    while(!active){
        OSI_LOGI(0, "no active pdp context,check again line[%d]", __LINE__);
        MG_osiThreadSleep(1000);
        active = MG_NwHasActivePdp(0);
    }
    int iResult;
    MG_mqtt_ali_client_info_t.ali_device_name="MgName";
    MG_mqtt_ali_client_info_t.ali_device_secret="569c0ad14f20c07064e4ee8d4a5f5416";
    MG_mqtt_ali_client_info_t.ali_product_key="a1A6f5k6fLm";    
    iResult = MG_AliMqttCreate(&MG_mqtt_ali_client_info_t); 
    OSI_LOGI(0, "MG_AliMqttCreate() line[%d]:iResult:%d", __LINE__,iResult);
  
    MG_mqtt_ali_client_info_t.hostname="iot-as-mqtt.cn-shanghai.aliyuncs.com";
    MG_mqtt_ali_client_info_t.port=1883;
    iResult = MG_AliMqttOpen(&MG_mqtt_ali_client_info_t);
    OSI_LOGI(0, "MG_AliMqttOpen() line[%d]:iResult:%d", __LINE__,iResult);
   
    MG_mqtt_ali_client_info_t.client_info.client_id="mattclientExample";
    MG_mqtt_ali_client_info_t.username=NULL;
    MG_mqtt_ali_client_info_t.password=NULL;
    iResult = MG_AliMqttConnect(&MG_mqtt_ali_client_info_t,0,0);
    OSI_LOGI(0, "MG_AliMqttConnect() line[%d]:iResult:%d", __LINE__,iResult);
    
    iResult = MG_AliMqttSetCallback(MG_mqtt_incoming_publish_cb,MG_mqtt_incoming_data_cb);
    OSI_LOGI(0, "MG_AliMqttSetCallback() line[%d]:iResult:%d", __LINE__,iResult);
    
    iResult =MG_AliMqttSubscribe("/a1A6f5k6fLm/MgName/user/get",0);
    OSI_LOGI(0, "MG_AliMqttSubscribe() line[%d]:iResult:%d", __LINE__,iResult);
    
    while(g_mqtt_msg_count < 1)
    { 
        MG_osiThreadSleep(1000);
    }

    iResult = MG_AliMqttPublish("/a1A6f5k6fLm/MgName/user/update","This is test data, hello MQTT",0,0);
    OSI_LOGI(0, "MG_AliMqttPublish() line[%d]:iResult:%d", __LINE__,iResult);
    
    
    iResult =MG_AliMqttUnsubscribe("/a1A6f5k6fLm/MgName/user/get");
    OSI_LOGI(0, "MG_AliMqttUnsubscribe() line[%d]:iResult:%d", __LINE__,iResult);

    iResult = MG_AliMqttDisconnect(); 
    OSI_LOGI(0, "MG_AliMqttDisconnect() line[%d]:iResult:%d", __LINE__,iResult);
    
    MG_AliMqttClose();
    MG_AliMqttDestory();
    
    OSI_LOGI(0, "mg example exit line[%d]", __LINE__);
    MG_osiThreadSleep(1000);

    MG_osiThreadExit();
}



int appimg_enter(void *param)
{
     OSI_LOGI(0, "mg example enter", __LINE__);
     osiThreadCreate("mgmqtt", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*4,1);
    //MG_osiThreadCreate("mgmqtt", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*16);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "mg example exit", __LINE__);
}





























