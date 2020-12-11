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
#include "mg_alarm_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "time.h"

atSetting_t gAtSetting;


static void alarmCallback(drvRtcAlarm_t *alarm, void *unused)
{
    atAlarmInfo_tt *info = (atAlarmInfo_tt *)alarm->info;
    char rsp[80];

    if (info->silent)
        sprintf(rsp, "alarmCallback: %ld", alarm->name);
    else
        sprintf(rsp, "alarmCallback: %ld, \"%s\"", alarm->name, info->text);
    MG_osiTracePrintf(LOG_TAG,"%s",rsp);
}

OSI_UNUSED static int _outputTimeWithZone(char *rsp, struct tm *tm, int timezone)
{
    static const char *fmt1 = "%02d/%02d/%02d,%02d:%02d:%02d%+03d";
    static const char *fmt2 = "%04d/%02d/%02d,%02d:%02d:%02d%+03d";
    if (gAtSetting.csdf_auxmode == 1)
    {
        return sprintf(rsp, fmt1, (tm->tm_year + 1900) % 100, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec, timezone);
    }
    else
    {
        return sprintf(rsp, fmt2, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec, timezone);
    }
}

OSI_UNUSED static int _outputTime(char *rsp, struct tm *tm)
{
    static const char *fmt1 = "%02d/%02d/%02d,%02d:%02d:%02d";
    static const char *fmt2 = "%04d/%02d/%02d,%02d:%02d:%02d";
    if (gAtSetting.csdf_auxmode == 1)
    {
        return sprintf(rsp, fmt1, (tm->tm_year + 1900) % 100, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    else
    {
        return sprintf(rsp, fmt2, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}

static void prvThreadEntry(void *param)
{
	osiThreadSleep(3000);
	uint8_t re=0;
	uint32_t alarm_count=0;
	
	//set callback func
	MG_SetAlarmCallback(ALARM_API,alarmCallback);
	//set one alarm
	re =MG_SetAlarm(ALARM_API,"00/01/01,07:20:00",1,0,"alram1","0",0);
	if(re ==1)
	{
		MG_osiTracePrintf(LOG_TAG, "mgalarm add ok \n");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "mgalarm add error \n");
	}
	re =MG_SetAlarm(ALARM_API,"00/01/01,08:30:00",2,0,"alram1","0",0);
	if(re ==1)
	{
		MG_osiTracePrintf(LOG_TAG, "mgalarm add ok \n");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "mgalarm add error \n");
	}
	//read all alarm count
	alarm_count=MG_GetRtcGetAlarmCount(ALARM_API);
	MG_osiTracePrintf(LOG_TAG, "mgalarm count %d \n",alarm_count);

	drvRtcAlarm_t *alarms = malloc(alarm_count * sizeof(drvRtcAlarm_t));
    if (alarms == NULL)
    {
		 MG_osiTracePrintf(LOG_TAG, "mgalarm malloc error\n");
		 osiThreadSleep(100);
		 MG_osiThreadExit();
	}   
	char rsp[256];
	MG_GetRtcGetAlarms(ALARM_API, alarms, alarm_count);
    for (int i = 0; i < alarm_count; i++)
	{
		
		char *prsp = rsp;
		atAlarmInfo_tt *info = (atAlarmInfo_tt *)alarms[i].info;
		//MG_osiTracePrintf(LOG_TAG, "mgalarm type %d\n",alarms[i].type);
		
            if (alarms[i].type == DRV_RTC_ALARM_WDAY_REPEATED)
            {
                uint32_t sec_in_day = alarms[i].wday_repeated.sec_in_day;
                uint32_t hour = sec_in_day / 3600;
                uint32_t min = (sec_in_day % 3600) / 60;
                uint32_t sec = sec_in_day % 60;
                if (info->has_timezone)
                    sprintf(prsp, "%s\"%02d:%02d:%02d%+03d\"",prsp, hour, min, sec, info->timezone);
                else
                    sprintf(prsp, "%s\"%02d:%02d:%02d\"",prsp, hour, min, sec);
            }
            else
            {
                sprintf(prsp, "%s\"",prsp);
                struct tm tm;
                if (info->has_timezone)
                {
                    time_t t = alarms[i].expire_sec + info->timezone * CFW_TIME_ZONE_SECOND;
                    gmtime_r(&t, &tm);
                    _outputTimeWithZone(prsp, &tm, info->timezone);
                }
                else
                {
                    time_t t = alarms[i].expire_sec + gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
                    gmtime_r(&t, &tm);
                    _outputTime(prsp, &tm);
                }
                sprintf(prsp, "%s\"",prsp);
				
            }

            sprintf(prsp, "%s,%ld,0,\"%s\"",prsp, alarms[i].name, info->text);

            if (alarms[i].type == DRV_RTC_ALARM_WDAY_REPEATED &&
                alarms[i].wday_repeated.wday_mask != 0)
            {
                sprintf(prsp, "%s,\"",prsp);
                uint8_t wday_mask = alarms[i].wday_repeated.wday_mask;

                // It is tricky for the loop. Mask (0~6) is Sunday, Monday, ..
                // and what we need is (1~7) Monday, ...
                for (int n = 1; n < 8; n++)
                {
                    uint8_t mask = (1 << (n % 7));
                    if ((wday_mask & mask) == 0)
                        continue;

                    wday_mask &= ~mask;
                    if (wday_mask == 0) // last one
                        sprintf(prsp, "%s%d\"",prsp, n);
                    else
                        sprintf(prsp, "%s%d,",prsp, n);
                }
            }
            sprintf(prsp, "%s,%d", prsp,info->silent);
            MG_osiTracePrintf(LOG_TAG, "alarm %s\n",prsp);
	}
	free(alarms);
	//delelte one alarm
	MG_GelRtcRemoveAlarm(ALARM_API,1);
	//read all alarm count
	alarm_count=MG_GetRtcGetAlarmCount(ALARM_API);
	MG_osiTracePrintf(LOG_TAG, "mgalarm count %d \n",alarm_count);

	//re =MG_SetAlarm("12:18:50",3,0,"alram2","0",0);

	

	#if 1
	while(1)
	{
		
	
		osiThreadSleep(3000);
		MG_osiTracePrintf(LOG_TAG, "mgalarm is run \n");
	}
	#endif



   MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgalarm", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 2048);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
