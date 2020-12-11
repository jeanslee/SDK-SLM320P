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
#include "mg_sys_api.h"
#include "mg_date_api.h"
#include <string.h>
#include<time.h>


static void GetCurDate_test(void)
{
	MG_osiTracePrintf(LOG_TAG, "enter GetCurDate_test");
	MG_date date;
	MG_osiThreadSleep(2000);
	
	MG_GetCurDate(&date);
	MG_osiThreadSleep(2000);
       MG_osiTracePrintf(LOG_TAG, "current date : %04d-%02d-%02d-%02d-%02d-%02d" ,
	  	date.nYear,
	  	date.nMonth ,
	  	date.nDay,
	  	date.nHour,
	  	date.nMin,
	  	date.nSec );
	   
	MG_osiThreadSleep(2000);
	MG_osiTracePrintf(LOG_TAG, "get cur date success" );
	  
}



static void SetCurDate_test(void)
{
	MG_osiTracePrintf(LOG_TAG, "enter setCurDate_test");
	MG_date  date;
	MG_osiThreadSleep(2000);
	
	int offset = 8;
	date.nYear=2022;
	date.nMonth=8;
	date.nDay=1;
	date.nHour=10;
	date.nMin=45;
	date.nSec=45;
	MG_SetCurDate(&date,offset);
	MG_osiTracePrintf(LOG_TAG, "set cur date success" );
	
	MG_GetCurDate(&date);
       MG_osiTracePrintf(LOG_TAG, "get set  date : %04d-%02d-%02d-%02d-%02d-%02d" ,
	  		date.nYear,
	  		date.nMonth ,
	  		date.nDay,
	  		date.nHour,
	  		date.nMin,
	  		date.nSec );

}

static void Date_test(void)
{
	MG_osiTracePrintf(LOG_TAG, "enter Date_test");
	MG_osiThreadSleep(2000);
	MG_date  date;
	int64_t utc_sec, rtc_sec;

	date.nYear=2020;
	date.nMonth=8;
	date.nDay=1;
	date.nHour=10;
	date.nMin=45;
	date.nSec=45;
	MG_osiThreadSleep(2000);
	
	utc_sec = MG_UtcToSec(&date);
	MG_osiTracePrintf(LOG_TAG, "utc to sec : %d ",utc_sec);
	MG_osiThreadSleep(2000);
	
	rtc_sec = MG_RtcToSec(&date);
	MG_osiTracePrintf(LOG_TAG, "rtc to sec : %d ",rtc_sec);
	MG_osiThreadSleep(2000);

	int  offset = 8;
	int  local_sec = osiEpochSecond();
	int64_t  utc_sec1 = 123456;
	
	int  utcsec=MG_LocalToUtc(local_sec,offset);
	MG_osiTracePrintf(LOG_TAG, "local to utc  : %d ",utcsec);
	MG_osiThreadSleep(2000);
	
	int localsec=MG_UtcToLocal(utc_sec1,offset);
	MG_osiTracePrintf(LOG_TAG, "utc to local  : %d ",localsec);
	MG_osiThreadSleep(2000);

	MG_date  utc_date;
	MG_date  rtc_date;
	int sec_utc=MG_SecToUtc(&utc_date,local_sec);
	MG_osiTracePrintf(LOG_TAG, "sec to utc date: %04d-%02d-%02d-%02d-%02d-%02d" ,
	  		utc_date.nYear,
	  		utc_date.nMonth ,
	  		utc_date.nDay,
	  		utc_date.nHour,
	  		utc_date.nMin,
	  		utc_date.nSec );
	
	MG_osiThreadSleep(2000);
	int sec_rtc=MG_SecToRtc(&rtc_date,local_sec);
	MG_osiTracePrintf(LOG_TAG, "sec to rtc date: %04d-%02d-%02d-%02d-%02d-%02d" ,
	  		rtc_date.nYear,
	  		rtc_date.nMonth ,
	  		rtc_date.nDay,
	  		rtc_date.nHour,
	  		rtc_date.nMin,
	  		rtc_date.nSec );
	MG_osiThreadSleep(2000);
	
	
	
}

static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(2000);
	GetCurDate_test();
       //SetCurDate_test();
	Date_test();
	while(1)
	{
		MG_osiThreadSleep(2000);
	}
      MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg date example enter");
    MG_osiThreadCreate("mgdate", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg date example exit");
}






















