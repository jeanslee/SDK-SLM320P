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
#include "mg_os_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "drv_adc.h"

static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(5000);
	bool ret;
	ret = MG_Reboot();
	if(ret==true)
	{
		MG_osiTracePrintf(LOG_TAG, "mg reboot success");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "mg reboot failed");
	}

    osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg reboot example enter");
    MG_osiThreadCreate("mgreboot", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg reboot example exit");
}
