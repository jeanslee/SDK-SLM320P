#include "meig_http_api.h"

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "hal_chip.h"
#include "cfw.h"
#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')


static void prvHttpThread(void *param)
{
    
     OSI_PRINTFI("MG_HTTP start");
     int i =0;
     OSI_LOGI(0, "MG_HTTP_GET start,%d",i);
     //start network
     uint8_t rat,siglevel,biterr;
    uint8_t mnc[3];
    uint8_t mcc[3];
    int status = 0;
    uint32_t AutoActPDP;
	
    OSI_LOGI(0, "meig network  application thread enter, param 0x%x", param);

    while(status == 0){
        MG_osiThreadSleep(1000);
        status = MG_cfwGetSimStatus(0);
        MG_osiTracePrintf(LOG_TAG, "sim status:%d",status);
    }

    uint8_t cid = 2;
    //if(MG_NwModifyPdpContext(0,cid,1,"cmnet")){
    //if(MG_NwModifyPdpContext(0,cid,1,"3gnet")){
    if(MG_NwModifyPdpContext(0,cid,1,"ctnet")){
        MG_osiTracePrintf(LOG_TAG, "meig modify pdp ctxt success");
        MG_osiThreadSleep(10000);
        if(MG_NwActivePdpContext(0,cid))
            MG_osiTracePrintf(LOG_TAG, "meig activate pdp ctxt success");
        else
            MG_osiTracePrintf(LOG_TAG, "meig activate pdp ctxt failure");
    }
    
    for (int n = 0; n < 10; n++)
    {
        OSI_LOGI(0, "hello world %d", n);
        osiThreadSleep(500);
    }
    

    
    //http req
    
   while(1){
    
    OSI_PRINTFI("MG_HTTP_GET start");
    // HttpInfoSet *httpinfo = (HttpInfoSet*)malloc(sizeof);
    HttpInfoSet httpinfo;
    memset(&httpinfo, 0, sizeof(HttpInfoSet));
//GET
    strcpy(httpinfo.url, "http://104.224.176.31:8080/"); 
    strcpy(httpinfo.username, "123");                                        //Optional
    strcpy(httpinfo.password, "123");                                        //Optional
    strcpy(httpinfo.HandData, "Connection:keep-alive");     //Optional,    Separated by commas
    httpinfo.cid = 1;
    
	if(MG_HTTP_GET(&httpinfo))
    {
		 OSI_PRINTFI("MG_HTTP_GET error");
    }
    else
    {
		 OSI_PRINTFI("MG_HTTP_GET req status %d",httpinfo.StatusCode);
		 OSI_PRINTFI("MG_HTTP_GET recv data %s,len %d",httpinfo.RecvData,httpinfo.recvlen);
    }
    
    MG_HTTP_ParaClear(&httpinfo);
    OSI_PRINTFI("MG_HTTP_GET end");
	osiThreadSleep(30000);
//HEAD
    memset(&httpinfo, 0, sizeof(HttpInfoSet));
    strcpy(httpinfo.url, "http://104.224.176.31:8080/"); 
    strcpy(httpinfo.username, "123");                                        //Optional
    strcpy(httpinfo.password, "123");                                        //Optional
    strcpy(httpinfo.HandData, "Connection:keep-alive");     //Optional,    Separated by commas
    httpinfo.cid = 1;
	if(MG_HTTP_HEAD(&httpinfo))
    {
		 OSI_PRINTFI("MG_HTTP_HEAD error");
    }
    else
    {
		 OSI_PRINTFI("MG_HTTP_HEAD req status %d",httpinfo.StatusCode);
		 OSI_PRINTFI("MG_HTTP_HEAD recv data %s,len %d",httpinfo.RecvData,httpinfo.recvlen);
    }
    MG_HTTP_ParaClear(&httpinfo);
    osiThreadSleep(30000);
//POST
    // memset(&httpinfo, 0, sizeof(HttpInfoSet));
    // strcpy(httpinfo.url, "http://104.224.176.31:8080"); 
    // strcpy(httpinfo.username, "123");                                        //Optional
    // strcpy(httpinfo.password, "123");                                        //Optional
    // strcpy(httpinfo.HandData, "Connection:keep-alive");     //Optional,    Separated by commas
    // httpinfo.cid = 1;
	// if(MG_HTTP_POST(&httpinfo))
    // {
    //     OSI_LOGI(0,"MG_HTTP_POST error");
    // }
    // else
    // {
    //     OSI_LOGI(0,"MG_HTTP_POST req status %d",httpinfo.StatusCode);
    //     OSI_LOGI(0,"MG_HTTP_POST recv data %s,len %d",httpinfo.RecvData,httpinfo.recvlen);
    // }
    // MG_HTTP_ParaClear(&httpinfo);
    
	
   }
   
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgaudio", prvHttpThread, NULL, OSI_PRIORITY_NORMAL, 1024*10);
	return 0;
}

void appimg_exit(void)
{
    
	MG_osiTracePrintf(LOG_TAG, "mg example exit");
}


