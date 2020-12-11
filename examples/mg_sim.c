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

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{
    int status = 0;
    while(status == 0){
        MG_osiThreadSleep(1000);
        status = MG_cfwGetSimStatus(0);
        MG_osiTracePrintf(LOG_TAG, "sim status:%d",status);
    }
    uint8_t state=0;
    unsigned rem=0;

    if(MG_malSimGetState(0,&state,&rem)){
        MG_osiTracePrintf(LOG_TAG, "sim state %x,%d",state,rem);
    }
    else{
        MG_osiTracePrintf(LOG_TAG, "get state failed");
    }
    char*pwd = "1234";
    if(state == 0 &&MG_cfwSetSimPIN(0,pwd)){
        MG_osiTracePrintf(LOG_TAG, "set pin success");
    }
    if(state == 2 && MG_cfwUnlockSimWithPIN(0,pwd))
        MG_osiTracePrintf(LOG_TAG, "unlock sim success");
    if(state == 2 && MG_cfwClearSimPIN(0,pwd))
        MG_osiTracePrintf(LOG_TAG, "clear pin success");

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgsim", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
