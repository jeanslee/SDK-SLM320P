#include "meig_http_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "hal_chip.h"
#include "cfw.h"
#include "mg_ali_mqtt_api.h"
#include "drv_wifi.h"
#include "mg_net_api.h"

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#define MG_MQTT_SUCCESS 0


mqtt_ali_client_info mqttInfo;
static char mqTopic[64]={0};
static uint8_t mqMsgCnt=0; 
char recvdata[500];
uint8_t imei[16] = {0};
uint8_t imsi[16] = {0};
uint8_t ver[128]={0};


void mqtt_publish_cb(void *arg, const char *topic, uint32_t total_len)
{
    memset(mqTopic, 0, sizeof(mqTopic));
    if (strlen(topic) < sizeof(mqTopic) - 1)
    {
        strcpy(mqTopic, topic);
    }
    else
    {
        strncpy(mqTopic, topic, sizeof(mqTopic) - 5);
        strncpy(mqTopic+sizeof(mqTopic) - 5,"...", 3);
    }
    MG_osiTracePrintf(LOG_TAG, "mqtt_publish_cb():mqTopic:%s",mqTopic);
}

void mqtt_data_cb(void *arg, const uint8_t *data, uint16_t data_len, uint8_t flags)
{
    char *urc_msg = NULL;
    unsigned int tmplen = 0;
    unsigned int urc_len = data_len + strlen(mqTopic) + 8;

    mqMsgCnt++;
    urc_msg = (char *)malloc(urc_len);
    if (NULL == urc_msg)
    {
        MG_osiTracePrintf(LOG_TAG, "mqtt_data_cb() line[%d]:Error:Malloc false", __LINE__);
        return;
    }
    else
    {
        memset(urc_msg, 0, urc_len);
        strncpy(urc_msg, mqTopic,strlen(mqTopic));
        strncpy(urc_msg+strlen(mqTopic), ":",1);
        strncpy(urc_msg+strlen(mqTopic)+1, (char *)data,data_len);
    }
    MG_osiTracePrintf(LOG_TAG, "mqtt_data_cb():urc_msg:%s",urc_msg);
    free(urc_msg);
    memset(mqTopic, 0, sizeof(mqTopic));

}

void meig_init_net(void)
{
    int status = 0;


    while(status == 0){
        MG_osiThreadSleep(1000);
        status = MG_cfwGetSimStatus(0);
        MG_osiTracePrintf(LOG_TAG, "sim status:%d",status);
    }
    bool active = false;

    /*
    uint8_t cid = 1;
    if(MG_NwModifyPdpContext(0,cid,1,"cmnet")){
    //if(MG_NwModifyPdpContext(0,cid,1,"3gnet")){
    //if(MG_NwModifyPdpContext(0,cid,1,"ctnet")){
        MG_osiTracePrintf(LOG_TAG, "meig modify pdp ctxt success");
        MG_osiThreadSleep(10000);
        if(MG_NwActivePdpContext(0,cid))
            MG_osiTracePrintf(LOG_TAG, "meig activate pdp ctxt success");
        else
            MG_osiTracePrintf(LOG_TAG, "meig activate pdp ctxt failure");
    }
    */
    while(!active){
        MG_osiTracePrintf(LOG_TAG, "no active pdp context,check again line[%d]", __LINE__);
        MG_osiThreadSleep(1000);
        active = MG_NwHasActivePdp(0);
    }
    
    for (int n = 0; n < 10; n++)
    {
        MG_osiTracePrintf(LOG_TAG, "hello world %d", n);
        osiThreadSleep(500);
    }
}


void meig_parse_deviceinfo(char * deviceinfo, int length)
{
	MG_osiTracePrintf(LOG_TAG, "meig_parse_deviceinfo start");
	char * p;
	char * p_end;
	int len=0;
	
	p = strstr(deviceinfo, "domain");
	p = p + 9;
	p_end=strstr(p,":");
	len=p_end-p;
	
	mqttInfo.hostname = malloc(len+1);
	memset(mqttInfo.hostname,0,len+1);
	memcpy(mqttInfo.hostname, p , len);
	
	char rsp[128];
	memset(rsp,0,128);
	memcpy(rsp, p , len);
	MG_osiTracePrintf(LOG_TAG,"hostname %s",rsp);
	osiThreadSleep(1000);
	
	p = p_end + 1;
	p_end=strstr(p, ",");
	len=p_end-p-1;
	char port[8];
	memset(port,0,8);
	memcpy(port,p,len);
	mqttInfo.port = atoi(port);
	
	
	MG_osiTracePrintf(LOG_TAG,"port %d",mqttInfo.port);
	osiThreadSleep(1000);
	
	p = strstr(deviceinfo, "customerId");
	p=p+13;
	p_end=strstr(p,",");
	len=p_end-p-1;
	
	mqttInfo.client_info.client_id = malloc(len+1);
	memset(mqttInfo.client_info.client_id,0,len+1);
	memcpy(mqttInfo.client_info.client_id, p , len);
	
	memset(rsp,0,128);
	memcpy(rsp, p , len);
	MG_osiTracePrintf(LOG_TAG,"client_id %s",rsp);
	osiThreadSleep(1000);
	
	p = strstr(deviceinfo, "imei");
	p=p+7;
	p_end=strstr(p, ",");
	len=p_end-p-1;
	
	mqttInfo.ali_device_name = malloc(len+1);
	memset(mqttInfo.ali_device_name,0,len+1);
	memcpy(mqttInfo.ali_device_name, p , len);
	
	memset(rsp,0,128);
	memcpy(rsp, p , len);
	MG_osiTracePrintf(LOG_TAG,"ali_device_name %s",rsp);
	osiThreadSleep(1000);
	
	p = strstr(deviceinfo, "productKey");
	p=p+13;
	p_end=strstr(p, ",");
	len=p_end-p-1;
	mqttInfo.ali_product_key = malloc(len+1);
	memset(mqttInfo.ali_product_key,0,len+1);
	memcpy(mqttInfo.ali_product_key, p , len);
   	
		memset(rsp,0,128);
	memcpy(rsp, p , len);
	MG_osiTracePrintf(LOG_TAG,"ali_product_key %s",rsp);
	osiThreadSleep(1000);
	
	p = strstr(deviceinfo, "deviceSecret");
	p=p+15;
	p_end=strstr(p, "}");
	len=p_end-p-1;
	mqttInfo.ali_device_secret = malloc(len+1);
	memset(mqttInfo.ali_device_secret,0,len+1);
	memcpy(mqttInfo.ali_device_secret, p , len);
		memset(rsp,0,128);
	memcpy(rsp, p , len);
	MG_osiTracePrintf(LOG_TAG,"ali_device_secret %s",rsp);
	osiThreadSleep(1000);
	
}

void meig_http_get_deviceinfo(void)
{
    HttpInfoSet httpinfo;
    memset(&httpinfo, 0, sizeof(HttpInfoSet));

    sprintf(httpinfo.url, "http://iot.mengniu.com.cn/iot/device/registration?imei=%s&customerId=e631972317ae412d84",(char*)imei); 
    MG_osiTracePrintf(LOG_TAG,"url:%s",httpinfo.url);
    httpinfo.cid = 1;
    //strcpy(httpinfo.username, "123");                                        //Optional
    //strcpy(httpinfo.password, "123");                                        //Optional
    //strcpy(httpinfo.HandData, "Connection:keep-alive");     //Optional,    Separated by commas
	
   //while(1)
   {
		MG_osiTracePrintf(LOG_TAG,"MG_HTTP_GET start");
		//GET    
		if(MG_HTTP_GET(&httpinfo))
		{
			MG_osiTracePrintf(LOG_TAG,"MG_HTTP_GET error");
		}
		else
		{	
	        
			MG_osiTracePrintf(LOG_TAG,"MG_HTTP_GET req status %d",httpinfo.StatusCode);
			MG_osiTracePrintf(LOG_TAG,"MG_HTTP_GET recv data %s,len %d",httpinfo.RecvData,httpinfo.recvlen);
			MG_osiTracePrintf(LOG_TAG,"MG_HTTP_GET end");
			//break;
		}		
		osiThreadSleep(3000);
   }
   
   
    meig_parse_deviceinfo(httpinfo.RecvData, httpinfo.recvlen);
	
    MG_HTTP_ParaClear(&httpinfo);
	
/*
char *RecvData1="{\"status\":\"1\",\"message\":\"success\",\"data\":{\"domain\":\"iot-as-mqtt.cn-shanghai.aliyuncs.com:1883\",\"customerId\":\"hairong\",\"imei\":\"862485035897099\",\"productKey\":\"a14XBNg6pCp\",\"deviceSecret\":\"f5183a47057820dd5fd289173dcc5040\"}}";
meig_parse_deviceinfo(RecvData1, strlen(RecvData1));
*/
}

static int prvWifiApInfoCompare(const void *ctx1, const void *ctx2)
{
    const wifiApInfo_t *w1 = (const wifiApInfo_t *)ctx1;
    const wifiApInfo_t *w2 = (const wifiApInfo_t *)ctx2;
    return (w1->rssival - w2->rssival);
}

static char*  prvWifiScanAllChannel(drvWifi_t *w, uint32_t scan_round)
{
    MG_osiTracePrintf(LOG_TAG, "wifi scan all channel %u rounds", scan_round);
    wifiApInfo_t *aps = (wifiApInfo_t *)malloc(25 * sizeof(wifiApInfo_t));
    wifiScanRequest_t req = {};
    req.aps = aps;
    req.max = 25;
    req.found = 0;
    req.maxtimeout = 300;
    char *buf = (char*)malloc(128*sizeof(char));
    memset(buf,0,128*sizeof(char));

    bool r = MG_drvWifiScanAllChannel(w, &req, scan_round);
    if (!r)
    {
        MG_osiTracePrintf(LOG_TAG, "wifi scan all channel fail");
    }
    else if (req.found != 0)
    {
        qsort(&req.aps[0], req.found, sizeof(wifiApInfo_t), prvWifiApInfoCompare);
        for (uint32_t i = 0; i < req.found; i++)
        {
            wifiApInfo_t *w = &req.aps[i];
            MG_osiTracePrintf(LOG_TAG, "found ap - {mac address: %x%lx, rssival: %i dBm, channel: %u}",
                     w->bssid_high, w->bssid_low, w->rssival, w->channel);
            sprintf(buf+strlen(buf),"%x%lx,%i",w->bssid_high, w->bssid_low, w->rssival);
            MG_osiTracePrintf(LOG_TAG, "%s",buf);
            if(strlen(buf)>110)
                break;
        }
    }
    free(aps);
    return buf;
}

void post_info(){

    HttpInfoSet httpinfo;
    memset(&httpinfo, 0, sizeof(HttpInfoSet));

    strcpy(httpinfo.url, "http://iot.zhiquyun.net/dev-server/platform/retainsDeviceName"); 
    httpinfo.cid = 1;
    sprintf(httpinfo.SendData,"imei=%s&imsi=%s&swVers=%s",(char*)imei,(char*)imsi,(char*)ver);
    MG_osiTracePrintf(LOG_TAG,"url:%s,data:%s",httpinfo.url,httpinfo.SendData);
    MG_HTTP_POST(&httpinfo);
    MG_osiTracePrintf(LOG_TAG,"post req status %d",httpinfo.StatusCode);
    MG_osiTracePrintf(LOG_TAG,"postrecv data %s,len %d",httpinfo.RecvData,httpinfo.recvlen);

    MG_HTTP_ParaClear(&httpinfo);
}


static void meig_mqtt_thread_main(void *param)
{
    
    MG_osiTracePrintf(LOG_TAG,"MG_HTTP start");

    int len = 128;
    MG_getFirmwareVersion(ver,&len);


 	
    MG_osiTracePrintf(LOG_TAG, "meig network  application thread enter, param 0x%x", param);
	meig_init_net();
    len = 16;
    MG_getSimImsi(0,imsi,&len);
    len = 16;
    MG_getSimImei(0,imei,&len);
	meig_http_get_deviceinfo(); 

    post_info();
	int result;
	result = MG_AliMqttCreate(&mqttInfo); 
    MG_osiTracePrintf(LOG_TAG, "MG_AliMqttCreate() line[%d]:result:%d", __LINE__,result);
  

    result = MG_AliMqttOpen(&mqttInfo);
    MG_osiTracePrintf(LOG_TAG, "MG_AliMqttOpen() line[%d]:result:%d", __LINE__,result);
   
    mqttInfo.username=NULL;
    mqttInfo.password=NULL;
    result = MG_AliMqttConnect(&mqttInfo,0,0);
    MG_osiTracePrintf(LOG_TAG, "MG_AliMqttConnect() line[%d]:result:%d", __LINE__,result);

    result = MG_AliMqttSetCallback(mqtt_publish_cb,mqtt_data_cb);
    MG_osiTracePrintf(LOG_TAG,"%d--result:%d",__LINE__,result);

    char topic[64]={0};
    sprintf(topic,"/sys/%s/%s/rrpc/request",mqttInfo.ali_product_key,mqttInfo.ali_device_secret);
    result = MG_AliMqttSubscribe(topic,0);
    MG_osiTracePrintf(LOG_TAG,"%d--result:%d",__LINE__,result);

    char topic2[64]={0};
    sprintf(topic2,"/sys/%s/%s/rrpc/reponse",mqttInfo.ali_product_key,mqttInfo.ali_device_secret);
    result = MG_AliMqttSubscribe(topic,0);
    MG_osiTracePrintf(LOG_TAG,"%d--result:%d",__LINE__,result);

    char topic3[64]={0};
    char topic4[64]={0};
    sprintf(topic3,"/%s/%s/user/Rpt/DevInfo",mqttInfo.ali_product_key,mqttInfo.ali_device_secret);
    sprintf(topic4,"/%s/%s/user/Rpt/AttrRpt",mqttInfo.ali_product_key,mqttInfo.ali_device_secret);
    char msg[256]={0};
    sprintf(msg,"{\"devType\":\"00\",\"swVers\":\"%s\",\"mType\":\"4G\",\"elec\":\"00\",\"imsi\":\"%s\"}",ver,imsi);

    MG_osiTracePrintf(LOG_TAG,"msg:%s",msg);
    result = MG_AliMqttPublish(topic3,msg,0,0);
    MG_osiTracePrintf(LOG_TAG,"%d--result:%d",__LINE__,result);

    CurrentCellInfo cellinfo;
    memset(&cellinfo,0,sizeof(cellinfo));
    MG_NwGetCurrentCellInfo(&cellinfo);
    uint8_t mcc[3],mnc[3];
    mcc[0] = cellinfo.mcc[0];
    mcc[1] = cellinfo.mcc[1];
    mcc[2] = cellinfo.mcc[2];
    mnc[0] = cellinfo.mnc[0];
    mnc[1] = cellinfo.mnc[1];
    mnc[2] = cellinfo.mnc[2];


    int lac = cellinfo.lac;
    int cellid = cellinfo.cid;

    drvWifi_t *w = MG_drvWifiOpen();
    if (w == NULL)
    {
        MG_osiTracePrintf(LOG_TAG, "wifi scan example create wifi context fail");
    }

    for (;;)
    {
        char smnc[6]={0};
        char smcc[6]={0};
        sprintf(smnc,"%d%d%d",mnc[0],mnc[1],mnc[2]);
        sprintf(smcc,"%d%d%d",mcc[0],mcc[1],mcc[2]);

        uint8_t rat,sig,biterr;
        MG_NwGetSignalQuality(&rat,&sig,&biterr);
        CFW_TSM_ALL_NEBCELL_INFO nebcell;
        MG_NwGetNeighborCellInfo(&nebcell, 5000);
        char cells[128]={0};
        memset(cells,0,sizeof(cells));
        int copy = 0;
        for(uint8_t i=0;i<nebcell.nTSM_NebCellNUM;i++){
        
            char cl[24]={0};
            if(i >5)
                break;
            CFW_TSM_NEIGHBOR_CELL_INFO cinfo =nebcell.nTSM_NebCell[i];

            int lac2 = (cinfo.nTSM_LAI[3] << 8) | (cinfo.nTSM_LAI[4]);
            int cellid2 = (cinfo.nTSM_CellID[0] << 8) | (cinfo.nTSM_CellID[1]);
            sprintf(cl,"%s,%s,%d,%d,%d|",smcc,smnc,lac2,cellid2,cinfo.nTSM_AvRxLevel);
            int clen = strlen(cl);
            memcpy(cells+copy,cl,clen);
            MG_osiTracePrintf(LOG_TAG,"cells:%s",cells);
            copy+=clen;
        }

        char *buf = NULL;
        if(w != NULL)
            buf = prvWifiScanAllChannel(w,1);

        memset(msg,0,sizeof(msg));
        sprintf(msg,"{\"103\":\"%s,%s,%d,%d,%d\",\"104\":%s\"\",\"105\":\"%s\"}",smcc,smnc,lac,cellid,(int)sig,cells,buf);
        MG_osiTracePrintf(LOG_TAG,"msg:%s",msg);
        result = MG_AliMqttPublish(topic4,msg,0,0);
        MG_osiTracePrintf(LOG_TAG,"%d--result:%d",__LINE__,result);

        osiEvent_t event;
        event.id = OSI_EVENT_ID_NONE;

        osiEventTryWait(MG_osiThreadCurrent(), &event,300*1000);
        OSI_LOGI(0, "receive event %d", event.id);
        if (event.id == OSI_EVENT_ID_QUIT)
            break;
    }

    
	
	result = MG_AliMqttDisconnect(); 
	while(result != 0)
    {		
		MG_osiTracePrintf(LOG_TAG, "MG_AliMqttDisconnect() line[%d]:result:%d", __LINE__,result);
		osiThreadSleep(1000);	
		result = MG_AliMqttDisconnect(); 
	}
    
    
    MG_AliMqttClose();
    MG_AliMqttDestory();
	
	free(mqttInfo.hostname);
	free(mqttInfo.client_info.client_id);
	free(mqttInfo.ali_device_name);
	free(mqttInfo.ali_product_key);
	free(mqttInfo.ali_device_secret);
	
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiThreadCreate("MEIG_MQTT", meig_mqtt_thread_main, NULL, OSI_PRIORITY_NORMAL, 1024*10);
	return 0;
}

void appimg_exit(void)
{
    
	MG_osiTracePrintf(LOG_TAG, "mg example exit");
}


