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
#include "ntp_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static  void ntpCallback(const char* str)
{
	//str =  "2020/07/06,07:56:37+00"
	MG_osiTracePrintf(LOG_TAG, "MG examales NTP time Is %d %s\n",strlen(str),str);

}

static void prvThreadEntry(void *param)
{
	#if 1
	while(1)
	{
		if(MG_SNTP_Get_Time("ntp1.aliyun.com",ntpCallback) !=-1)
		//if(MG_SNTP_Get_Time("s1a.time.edu.cn",ntpCallback) !=-1)
		{
			MG_osiTracePrintf(LOG_TAG, "ntp net ok \n");
			break;
		}
		osiThreadSleep(3000);
		MG_osiTracePrintf(LOG_TAG, "ntp net wait \n");
	}
	#endif

	// Domain Name or IP
   //MG_SNTP_Get_Time("s1a.time.edu.cn");
   //MG_SNTP_Get_Time("199.182.204.197");

   MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgntp", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
