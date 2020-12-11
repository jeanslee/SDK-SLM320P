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
#include "abup_fota.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if 0
#define _ABUP_FOTA_SERVICE_OEM             	    "NEM1"
#define _ABUP_FOTA_SERVICE_MODEL           	    "8910_MODULEv1.2"
#define _ABUP_FOTA_SERVICE_PRODUCT_ID        "1595235787"
#define _ABUP_FOTA_SERVICE_PRODUCT_SEC     "20d927d8a8654e51b9228341dddf725d"
#define _ABUP_FOTA_SERVICE_DEVICE_TYPE       "IOT"
#define _ABUP_FOTA_SERVICE_PLATFORM       	    "SC8910DM"
#else
#define _ABUP_FOTA_SERVICE_OEM             	 "8910DM"
#define _ABUP_FOTA_SERVICE_MODEL           	 "SLM320"
#define _ABUP_FOTA_SERVICE_PRODUCT_ID       "1598594387"
#define _ABUP_FOTA_SERVICE_PRODUCT_SEC     "03448d94b2ef40daa9518b8202a3e66a"
#define _ABUP_FOTA_SERVICE_DEVICE_TYPE      "robot"
#define _ABUP_FOTA_SERVICE_PLATFORM       	 "SC8910DM"

#define _ABUP_FOTA_VERSION       		 	 "app_v1.0.0"
#endif

abup_server_info_t _abup_server_info={0};

osiTimer_t *timer;
static bool periodic=true;
uint32_t    timer_out=1000*60*60*60*3;  ///3hour


void abup_updata_cb(int event)
{
	switch(event)
	{
		//update successfully
		case 1000:
			MG_osiTracePrintf(LOG_TAG, "APP uptata succeed:%d",event);
			break;
		//Invalid version
		case 2303:
			MG_osiTracePrintf(LOG_TAG, "APP uptata Invalid version:%d",event);
			break;
		//No new upgrades
		case 2101:
			MG_osiTracePrintf(LOG_TAG, "APP uptata No new upgrades:%d",event);
			break;
		//not registered
		case 2103:
			MG_osiTracePrintf(LOG_TAG, "APP uptata not registered:%d",event);
			break;
		default:
			MG_osiTracePrintf(LOG_TAG, "APP uptata unkown error:%d",event);
			break;

	}


}


void prvTimerCb(void *param)
{
     int re =0;
    MG_osiTracePrintf(LOG_TAG,"in timer cb");
    re=MG_abup_create_fota_task();
    if(re ==1)
    {
	MG_osiTracePrintf(LOG_TAG,"APP statrt updata ok");
    }
    else
    {
	MG_osiTracePrintf(LOG_TAG,"APP statrt updata error");
    }

	
    MG_osiTimerStart(timer, timer_out);
}

static void prvThreadEntry(void *param)
{

    OSI_LOGI(0, "abup: fota start");
    MG_osiThreadSleep(1000);
	
    _abup_server_info.oem			=_ABUP_FOTA_SERVICE_OEM;
    _abup_server_info.model			=_ABUP_FOTA_SERVICE_MODEL;
   _abup_server_info.product_id		=_ABUP_FOTA_SERVICE_PRODUCT_ID;
   _abup_server_info.product_secret	=_ABUP_FOTA_SERVICE_PRODUCT_SEC;
   _abup_server_info.device_type		=_ABUP_FOTA_SERVICE_DEVICE_TYPE;
   _abup_server_info.platform			=_ABUP_FOTA_SERVICE_PLATFORM;

   //register cb
   _abup_server_info.abup_cb=abup_updata_cb;

   //set abup server info
   MG_abup_set_service_info(_abup_server_info);

   //set app version
   MG_abup_set_version(_ABUP_FOTA_VERSION);

   //get abup version
   char * version=MG_abup_get_version();

   MG_osiTracePrintf(LOG_TAG, "APP device version:%s",version);
  

    timer= MG_osiTimerCreate(NULL, prvTimerCb, NULL);	//creeate timer

    if(timer == NULL)
	{
        MG_osiTracePrintf(LOG_TAG, "APP create timer failed");
       }
	else
	{
        MG_osiTracePrintf(LOG_TAG, "APP create timer success");
	 MG_osiTimerStart(timer, 2000);	
	
    }

    while(1)
    {
    	MG_osiTracePrintf(LOG_TAG, "APP run is %s",version);

       MG_osiThreadSleep(5000);

    }

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "APP mg example enter");
    MG_osiThreadCreate("mg-abup", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*2);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "APP mg example exit");
}
