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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('E', 'L', 'B', 'S')

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "mg_gps_api.h"

static void prvGpsThread(void *param)
{
	bool active = false;
    while(!active){
        OSI_LOGI(0, "no active pdp context,check again line[%d]", __LINE__);
        MG_osiThreadSleep(1000);
        active = MG_NwHasActivePdp(0);
    }
	
    uint8_t lat[MG_LATITUDE_STR_SIZE];
    uint8_t lng[MG_LONGITUDE_STR_SIZE];
    for (;;)
    {
        memset(lng,0,MG_LONGITUDE_STR_SIZE);
        memset(lat,0,MG_LATITUDE_STR_SIZE); 

        MG_GetLbsLocation(lng,lat);

        osiTracePrintf(OSI_LOG_TAG, "MG_GetLbsLocation() : lng:%s lat:%s",lng,lat);
        osiThreadSleep(1000);
    }

	
	osiThreadExit();
}

int appimg_enter(void *param)
{
	OSI_LOGI(0, "gps example entry");

	osiThreadCreate("gpsthread", prvGpsThread, NULL, OSI_PRIORITY_NORMAL, 4*1024, 1);
	return 0;
}

void appimg_exit(void)
{
	OSI_LOGI(0, "gps example exit");
}


