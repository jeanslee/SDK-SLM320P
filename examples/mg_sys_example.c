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
 
#define EX_LOG_TAG OSI_MAKE_LOG_TAG('E', 'S', 'Y', 'S')
 
#include "osi_api.h"
#include "osi_log.h"
#include "mg_sys_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{
    int len;
    bool ret;
  
    MG_osiThreadSleep(10000);
    OSI_LOGI(0, "mg example enterline[%d]", __LINE__);

	uint8_t sn[SN_MIN_LENGTH] = {0};
	len = MG_SysReadSN(sn,SN_MIN_LENGTH);
      MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysReadSN() line[%d]: len:%d, SN:%s",__LINE__,len,sn);
	MG_osiThreadSleep(1000);
	
	uint8_t imei[IEMI_MIN_LENGTH] = {0};
	len = MG_SysReadImei(imei,IEMI_MIN_LENGTH);	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysReadImei() line[%d]: len:%d, IEMI:%s",__LINE__,len,imei);
        MG_osiThreadSleep(1000);

        ret= MG_SysReadRfCali();
        MG_osiThreadSleep(1000);
	 MG_osiTracePrintf(EX_LOG_TAG, "MG_SysReadRfCali() line[%d]: RF calibration:%d",__LINE__,ret);
        MG_osiThreadSleep(1000);

	uint8_t nbcl;
	ret= MG_SysBattGetRemainCap(&nbcl);
	MG_osiThreadSleep(1000);
    
        MG_osiTracePrintf(EX_LOG_TAG, "MG_SysBattGetRemainCap() :battery capacity(0~100):%d",nbcl); 
       MG_osiThreadSleep(1000);

	uint32_t nbcv;
	ret = MG_SysBattGetVol(&nbcv);
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysBattGetVol():battery voltage(3200~4200):%d",nbcv);
	MG_osiThreadSleep(1000);
	
	uint8_t nbcs;
	ret = MG_SysChangerGetStatus(&nbcs);
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysChangerGetStatus():battery Status(0~5):%d",nbcs);
	MG_osiThreadSleep(1000);
	
	char time[24]={0};
	len =24;
	ret = MG_SysGetModuleCompileTime(time,&len);
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysGetModuleCompileTime():time:%s",time);
	MG_osiThreadSleep(1000);
	
	char type[16]={0};
	len =16;
	ret = MG_SysGetModuleType(type,&len);
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysGetModuleType(): len:%d, type:%s",len,type);
	MG_osiThreadSleep(1000);
	
	char ver[48]={0};
	len =48;
	ret = MG_SysGetModuleVersion(ver,&len);
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysGetModuleVersion() : len:%d",len);
	MG_osiThreadSleep(1000);
       MG_osiTracePrintf(EX_LOG_TAG, "MG_SysGetModuleVersion() :ver:%s",ver);
	MG_SysSetTimeZone(-2);
	
	int timezone = MG_SysGetTimeZone();
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(EX_LOG_TAG, "MG_SysGetTimeZone(): timezone:%d",timezone);
	MG_osiThreadSleep(1000);

	//ret = MG_SysPowerOff();
	
    //ret = MG_SysReboot();
    OSI_LOGI(0, "mg example exit line[%d]" , __LINE__);
	
    MG_osiThreadExit();
}



int appimg_enter(void *param)
{
     OSI_LOGI(0, "mg example enter", __LINE__);
     osiThreadCreate("sys", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*4,1);
    //MG_osiThreadCreate("mgmqtt", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*16);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "mg example exit", __LINE__);
}





























