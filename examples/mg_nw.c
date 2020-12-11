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

/*===========================================================================
                        EDIT HISTORY

  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.

When             Who              What, Where, Why
----------   ----------   ---------------------------------------------------

2020-06-01   LiWeihua        Added example to get auto-activate-PDP.
2020-06-01   LiWeihua        Added example to set auto-activate-PDP.
===========================================================================*/

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'N', 'W')

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{

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
        MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt success");
        MG_osiThreadSleep(10000);
        if(MG_NwActivePdpContext(0,cid))
            MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt success");
        else
            MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt failure");
    }
    else
        MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt failure");

    MG_osiTracePrintf(LOG_TAG, "mg now is activate");


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

    osiThreadSleep(5000);

    osiThreadSleep(5000);
    AutoActPDP = MG_GetAutoActPDP(0);
    OSI_LOGI(0, "#MG_GetAutoActPDP# AutoActPDP %d", AutoActPDP);

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgnw", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
