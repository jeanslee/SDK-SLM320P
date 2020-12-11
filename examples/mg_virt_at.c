/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "osi_pipe.h"
#include <string.h>

extern bool MG_getModelInfo(uint8_t*buf,int*len);

static void prvVirtAtRespCallback(void *param, unsigned event)
{
    osiPipe_t *pipe = (osiPipe_t *)param;
    char buf[256];
    for (;;)
    {
        int64_t ms = MG_osiEpochTime();
        int bytes = osiPipeRead(pipe, buf, 255);
        if (bytes <= 0)
            break;

        buf[bytes] = '\0';
        MG_osiTracePrintf(LOG_TAG, "VAT <--(%d): %s", bytes, buf);

        MG_osiTracePrintf(LOG_TAG, "VAT <--(ms:%ld)", ms);
        MG_osiThreadSleep(2000);
    }

}

static void prvVirtAtEntry(void *param)
{
    unsigned name = OSI_MAKE_TAG('V', 'A', 'T', '1');
    osiPipe_t *at_rx_pipe = MG_atDeviceVirtCreate(name,prvVirtAtRespCallback);

    const char *cmd = "AT+GMI\r\n";
    MG_osiTracePrintf(LOG_TAG, "VAT -->: %s", cmd);
    while(true){
        if(!MG_atDeviceVirtSend(at_rx_pipe,cmd))
            break;
        MG_osiThreadSleep(2000);
    }
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "application image enter, param 0x%x", param);
    osiThreadCreate("vat", prvVirtAtEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 4);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "application image exit");
}
