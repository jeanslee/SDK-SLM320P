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
#include "drv_efuse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RDA_EFUSE_UNIQUE_ID_LOW_INDEX (24)
#define RDA_EFUSE_UNIQUE_ID_HIGH_INDEX (26)

static void prvThreadEntry(void *param)
{
    uint32_t id_low, id_high;
    bool low = false, high = false;

    low = drvEfuseRead(false,RDA_EFUSE_UNIQUE_ID_LOW_INDEX, &id_low);
    high = drvEfuseRead(false,RDA_EFUSE_UNIQUE_ID_HIGH_INDEX, &id_high);
    if (low && high)
    {
        OSI_LOGI(0, "get CPU serial number succeed");
        OSI_LOGE(0, "EFUSE Read id_low = 0x%08x", id_low);
        OSI_LOGE(0, "EFUSE Read id_high = 0x%08x", id_high);
    }
    else
    { 
        OSI_PRINTFI("read CPU serial number failed");
        MG_osiThreadExit();
    }

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-serailnum", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
