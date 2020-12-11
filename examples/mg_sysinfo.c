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
    uint8_t buf[64]={'\0'};
    uint8_t buf2[64]={'\0'};
    uint8_t len =16;
    int status = 0;
    while(status != 1){
        MG_osiThreadSleep(1000);
        status = MG_cfwGetSimStatus(0);
        MG_osiTracePrintf(LOG_TAG, "sim status:%d",status);
    }
    if(MG_getSimImei(0,buf,&len))
        MG_osiTracePrintf(LOG_TAG, "imei:%s",buf);
    else
        MG_osiTracePrintf(LOG_TAG, "get imei err");

    len = 16;
    if(MG_getSimImsi(0,buf,&len))
        MG_osiTracePrintf(LOG_TAG, "imsi:%s",buf);
    else
        MG_osiTracePrintf(LOG_TAG, "get imsi err");

    len = 20;
    if(MG_getSimIccid(0,buf,&len))
        MG_osiTracePrintf(LOG_TAG, "iccid:%s",buf);
    else
        MG_osiTracePrintf(LOG_TAG, "get iccid err");
    len = 32;
    MG_getFirmwareVersion(buf2,&len);
    MG_osiTracePrintf(LOG_TAG, "fw ver:%s",buf2);
    len = 32;
    MG_getHardwareVersion(buf2,&len);
    MG_osiTracePrintf(LOG_TAG, "hw ver:%s",buf2);
    len = 32;
    MG_getManufactureInfo(buf2,&len);
    MG_osiTracePrintf(LOG_TAG, "manufacture info:%s",buf2);
    len = 32;
    MG_getModelInfo(buf2,&len);
    MG_osiTracePrintf(LOG_TAG, "model info:%s",buf2);
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgsysinfo", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
