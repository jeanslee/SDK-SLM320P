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

#define LOG_TAG OSI_MAKE_LOG_TAG('F', 'T', 'P', 'P')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfw.h"

#include "mg_ftp_api.h"
#include "mg_net_api.h"
extern void MG_osiThreadSleep(uint32_t ms);
extern uint8_t MG_cfwGetSimStatus(uint8_t nSimID);
extern bool MG_NwModifyPdpContext(uint8_t nSim,uint8_t cid,uint8_t pdptype,const char*apn);
extern  osiThread_t *MG_osiThreadCreate(const char *name, osiThreadEntry_t entry, void *argument,
            uint32_t priority, uint32_t stack_size);
extern void MG_osiThreadExit(void);
extern uint32_t MG_GetAutoActPDP(uint8_t nSim);
extern bool MG_NwGetNeighborCellInfo(NeighborCellInfo *cellinf, uint32_t timeout);
extern bool MG_NwGetMccMnc(uint8_t*mnc,uint8_t*mcc);
extern bool MG_NwGetSignalQuality(uint8_t*rat,uint8_t*siglevel,uint8_t*biterr);
extern void MG_NwGetPdpContextList(CFW_GPRS_PDPCONT_INFO_V2 *pdp,uint8_t *len,uint8_t nSim);
extern bool MG_NwDeactivePdpContext(uint8_t sim,uint8_t cid);
extern bool MG_NwGetCurrentCellInfo(CurrentCellInfo *cellinf);
extern bool MG_NwActivePdpContext(uint8_t sim,uint8_t cid);

int32_t read(uint8_t *buf, uint32_t buflen){
    MG_osiTracePrintf(LOG_TAG, "read buf:%s", buf);
    return 0;
}

static void prvThreadEntry(void *param)
{
    uint8_t rat,siglevel,biterr;
    uint8_t mnc[3];
    uint8_t mcc[3];
    int status = 0;
    uint32_t AutoActPDP;
	
    MG_osiTracePrintf(LOG_TAG, "meig network  application thread enter, param 0x%x", param);

    while(status == 0){
        MG_osiThreadSleep(1000);
        status = MG_cfwGetSimStatus(0);
        MG_osiTracePrintf(LOG_TAG, "sim status:%d",status);
    }

    uint8_t cid = 2;
    //if(MG_NwModifyPdpContext(0,cid,1,"cmnet")){
    //if(MG_NwModifyPdpContext(0,cid,1,"3gnet")){
    if(MG_NwModifyPdpContext(0,cid,1,"ctnet")){
        MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt success");
        MG_osiThreadSleep(10000);
        if(MG_NwActivePdpContext(0,cid)){
            MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt success");
			const char* url = "104.224.176.31:3101";
			const char* user = "user2";
			const char* passwd = "123456";
			MG_osiThreadSleep(2000);
			int ret = MG_ftpLogin(url,user,passwd);
            MG_osiTracePrintf(LOG_TAG, "MG_ftpLogin,ret=%d", ret);
            ret = MG_ftpPut("/1.test","/nvm/1.bin");
            MG_osiTracePrintf(LOG_TAG, "MG_ftpPut,ret=%d", ret);
            ret = MG_ftpFileSize("/1.test");
            MG_osiTracePrintf(LOG_TAG, "MG_ftpFileSize,ret=%d", ret);
            ret = MG_ftpGet("/1.test",read);
            MG_osiTracePrintf(LOG_TAG, "MG_ftpGet,ret=%d", ret);
			ret = MG_ftpLogout();
            MG_osiTracePrintf(LOG_TAG, "MG_ftpLogout,ret=%d", ret);
		}
        else
            MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt failure");
        MG_osiThreadSleep(20000);

        if(MG_NwDeactivePdpContext(0,cid))
            MG_osiTracePrintf(LOG_TAG, "deactivate pdp ctxt success");
        else
            MG_osiTracePrintf(LOG_TAG, "deactivate pdp ctxt failure");
    }
    else
        MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt failure");

    CFW_GPRS_PDPCONT_INFO_V2 pdpctx[8]; 
    uint8_t len = 8;
    MG_NwGetPdpContextList(pdpctx,&len,0);

    bool got = MG_NwGetSignalQuality(&rat,&siglevel,&biterr);
    if(got)
        MG_osiTracePrintf(LOG_TAG, "rat:%x,siglevel:%x,biterr:%x",rat,siglevel,biterr);
    else
        MG_osiTracePrintf(LOG_TAG, "get net signal quality err");

    got = MG_NwGetMccMnc(mnc,mcc);
    if(got)
        MG_osiTracePrintf(LOG_TAG, "mnc:%s,mcc:%x",(char*)mnc,(char*)mcc);
    else
        MG_osiTracePrintf(LOG_TAG, "get mnc/mcc err");

    CurrentCellInfo cellinf;
    NeighborCellInfo ncellinf;
    MG_NwGetCurrentCellInfo(&cellinf);
    MG_osiTracePrintf(LOG_TAG,"lac:%s",cellinf.lac);
    MG_NwGetNeighborCellInfo(&ncellinf, 1000);
    MG_osiTracePrintf(LOG_TAG,"neighbor cell num:%d",ncellinf.nCellNum);

    osiThreadSleep(5000);
    // Enable auto activate PDP:
//    MG_SetAutoActPDP(0, 1);

    osiThreadSleep(5000);
    AutoActPDP = MG_GetAutoActPDP(0);
    OSI_LOGI(0, "#MG_GetAutoActPDP# AutoActPDP %d", AutoActPDP);

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "ftp example enter");
    MG_osiThreadCreate("FTPP", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*8);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "ftp example exit");
}

