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
static bool periodic=true;
static osiTimer_t *timer;


void prvTimerCb(void *param)
{
    MG_osiTracePrintf(LOG_TAG,"in timer cb");
}

static void prvThreadEntry(void *param)
{
	    MG_osiThreadSleep(2000);
	    timer= MG_osiTimerCreate(NULL, (osiCallback_t)prvTimerCb, NULL);	//create timer
	    if(timer == NULL)
	   {
	        MG_osiTracePrintf(LOG_TAG, "create timer failed");
	    }
	   else
	   {
	        MG_osiTracePrintf(LOG_TAG, "create timer success");
	   }
	   
	    if (MG_osiTimerSetPeriod(timer, 3000,periodic))
	    	{
			MG_osiTracePrintf(LOG_TAG, "MG set period success");
		}
	    else
	    	{
			MG_osiTracePrintf(LOG_TAG, "MG set period failed");
		}
	    MG_osiTimerStartLast(timer);

	    while(1)
	    {
	        MG_osiThreadSleep(1000);
	    }
		
		MG_osiTimerStop(timer);
		MG_osiTracePrintf(LOG_TAG, "stop timer success");
		MG_osiTimerDelete(timer);
		MG_osiTracePrintf(LOG_TAG, "delete timer success");

		MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg timer example enter");
    MG_osiThreadCreate("mgtimer", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg timer example exit");
}




























