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

#include "mg_mqtt_api.h"

#define MG_MQTT_SUCCESS 0

#define MQTT_DEFAULT 1
#define MQTT_ONENET 2   //多协议
#define MQTT_ONENET_NEW  3 //MQTT 物联网套件(新版)
#define MQTT_CTWING 4

#define MQTT_TYPE MQTT_DEFAULT

extern void MG_osiThreadSleep();
extern bool MG_NwHasActivePdp();
extern void MG_osiThreadExit();

mqtt_client_context MG_mqtt_client_info_t;

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
    //OSI_LOGXI(OSI_LOGPAR_S, 0, "MG_mqtt_incoming_publish_cb():g_Mqtt_Topic:%s",g_Mqtt_Topic);
    return;
}

void MG_mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t data_len, uint8_t flags)
{
    char *urc_msg = NULL;
    //unsigned int tmplen = 0;
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
    OSI_LOGXI(OSI_LOGPAR_S, 0, "MY_mqtt_incoming_data_cb():urc_msg:%s",urc_msg);
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
#if (MQTT_TYPE == MQTT_DEFAULT)
    MG_mqtt_client_info_t.username="TEST";
    MG_mqtt_client_info_t.password="TEST";
    MG_mqtt_client_info_t.hostname="mq.tongxinmao.com";
    MG_mqtt_client_info_t.client_id="txm_1599202255";
    MG_mqtt_client_info_t.port=18831;
#endif

#if (MQTT_TYPE == MQTT_ONENET)
    MG_mqtt_client_info_t.username="369787";
    MG_mqtt_client_info_t.password="123456";
    MG_mqtt_client_info_t.hostname="183.230.40.39";
    MG_mqtt_client_info_t.client_id="625431747";
    MG_mqtt_client_info_t.port=6002;
#endif

#if (MQTT_TYPE == MQTT_ONENET_NEW)
    MG_mqtt_client_info_t.username="369774";
    MG_mqtt_client_info_t.password="version=2018-10-31&res=products%2F369774%2Fdevices%2FMEIG_MQTT&et=2536814975&method=sha1&sign=noGQz30mzRp%2F2z6PL8xsV%2FLHxe8%3D";
    MG_mqtt_client_info_t.hostname="183.230.40.96";
    MG_mqtt_client_info_t.client_id="MEIG_MQTT";
    MG_mqtt_client_info_t.port=1883;
    MG_mqtt_client_info_t.clean_session=1;  //MQTT 物联网套件(新版)必须设置为1
#endif

#if (MQTT_TYPE == MQTT_CTWING)
    MG_mqtt_client_info_t.username="meig";
    MG_mqtt_client_info_t.password="jNhXRgM6_pGAH3g2vQl7cJ9x2rqap99hIiOUa6uNDAM";
    MG_mqtt_client_info_t.hostname="mqtt.ctwing.cn";
    MG_mqtt_client_info_t.client_id="100942631";
    MG_mqtt_client_info_t.port=1883;
#endif



    iResult = MG_MqttCreate(); 
    OSI_LOGI(0, "MY_MqttCreate() line[%d]:iResult:%d", __LINE__,iResult);

    MG_mqtt_client_info_t.timeout = 300;  //ms
    MG_mqtt_client_info_t.retry_times = 20;  //max timeout : timeout*(retry_times+1)
    iResult = MG_MqttConnect(&MG_mqtt_client_info_t);
    OSI_LOGI(0, "MY_MqttConnect() line[%d]:iResult:%d", __LINE__,iResult);

#if (MQTT_TYPE == MQTT_DEFAULT)       
    iResult = MG_MqttSetCallback(MG_mqtt_incoming_publish_cb,MG_mqtt_incoming_data_cb);
    OSI_LOGI(0, "MY_MqttSetCallback() line[%d]:iResult:%d", __LINE__,iResult);

    iResult =MG_MqttUnsubscribe("/public/TEST/#");
    OSI_LOGI(0, "MY_MqttUnsubscribe() line[%d]:iResult:%d", __LINE__,iResult);
    
    iResult =MG_MqttSubscribe("/public/TEST/hes1",0);
    OSI_LOGI(0, "MY_MqttSubscribe()1 line[%d]:iResult:%d", __LINE__,iResult);

    iResult =MG_MqttSubscribe("/public/TEST/hes2",0);
    OSI_LOGI(0, "MY_MqttSubscribe()2 line[%d]:iResult:%d", __LINE__,iResult);

    iResult =MG_MqttSubscribe("/public/TEST/hes3",0);
    OSI_LOGI(0, "MY_MqttSubscribe()3 line[%d]:iResult:%d", __LINE__,iResult);

    iResult =MG_MqttSubscribe("/public/TEST/hes4",0);
    OSI_LOGI(0, "MY_MqttSubscribe()4 line[%d]:iResult:%d", __LINE__,iResult);

    iResult =MG_MqttSubscribe("/public/TEST/hes5",0);
    OSI_LOGI(0, "MY_MqttSubscribe()5 line[%d]:iResult:%d", __LINE__,iResult);

    uint32_t count_s=0;
    char  str[100]={0};
    while(1)
   {

        count_s++;
        sprintf(str,"my publish %d",count_s);
        iResult = MG_MqttPublish("/public/TEST/hes1",str,0,0);
        OSI_LOGI(0, "MY_MqttPublish() line[%d]:iResult:%d count:%d", __LINE__,iResult,count_s);
        MG_osiThreadSleep(1000);
   }
 
    while(g_mqtt_msg_count < 1)
    { 
        MG_osiThreadSleep(1000);
    }

    iResult = MG_MqttPublish("/public/TEST/windows","mqttserver and tool: http://mq.tongxinmao.com",0,0);
    OSI_LOGI(0, "MG_MqttPublish() line[%d]:iResult:%d", __LINE__,iResult);
    
    
    iResult =MG_MqttUnsubscribe("/public/TEST/#");
    OSI_LOGI(0, "MG_MqttUnsubscribe() line[%d]:iResult:%d", __LINE__,iResult);
#endif

    MG_osiThreadSleep(10000);
    iResult = MG_MqttDisconnect(); 
    OSI_LOGI(0, "MG_MqttDisconnect() line[%d]:iResult:%d", __LINE__,iResult);

    MG_MqttDestory();
    
    OSI_LOGI(0, "mg example exit line[%d]", __LINE__);
    MG_osiThreadSleep(1000);

    MG_osiThreadExit();
}



int appimg_enter(void *param)
{
     OSI_LOGI(0, "mg example enter", __LINE__);
     osiThreadCreate("mgmqtt", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*4,1);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "mg example exit", __LINE__);
}





























