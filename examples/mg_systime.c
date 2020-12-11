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


#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')
osiTimer_t *timer;
volatile bool timercb = false;
void prvTimerCb(void *param)
{
    timercb = true;
    MG_osiTracePrintf(LOG_TAG,"in timer cb");
}

static void prvThreadEntry(void *param)
{

    int64_t ms = MG_osiEpochTime();
    int64_t upms = MG_osiUpTime();
    for(int i=0;i<10;i++){
        MG_osiTracePrintf(LOG_TAG, "current ms:%ld,up ms:%ld",ms,upms);
        MG_osiThreadSleep(400);
        ms = MG_osiEpochTime();
        upms = MG_osiUpTime();
    }

    timer= MG_osiTimerCreate(NULL, prvTimerCb, NULL);
    if(timer == NULL){
        MG_osiTracePrintf(LOG_TAG, "create timer failed");
    }else{
        MG_osiTracePrintf(LOG_TAG, "create timer success");
        MG_osiTimerStart(timer, 20);
    }

    while(!timercb){
        MG_osiTracePrintf(LOG_TAG, "wait for timer");
        MG_osiThreadSleep(10);
    }

    
    MG_osiThreadSleep(2000);
    bool ret = MG_osiSetEpochTime(ms+2000);
    if(ret)
        MG_osiTracePrintf(LOG_TAG, "set epoch time success");
    else
        MG_osiTracePrintf(LOG_TAG, "set epoch time err");


    MG_osiTimerStop(timer);
    MG_osiTimerDelete(timer);

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgtime", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
