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
#include "drv_watchdog.h"
#include "mg_os_api.h"
#include "osi_log.h"

static void prvThreadEntry(void *param)
{
    bool ret = false;

    ret = MG_drvWatchDogStart(0.5, 1);
    if (ret == false)
    {
        MG_osiTracePrintf(LOG_TAG, "watch dog start failed");
        MG_osiThreadExit();
    }

    MG_osiThreadSleep(10000);
    MG_drvWatchDogStop();

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-watchdog", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
