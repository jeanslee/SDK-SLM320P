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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('E', 'G', 'P', 'S')

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "mg_gps_api.h"

uint8_t g_FindPosFlag;
#define MG_GPS_MAX_MODE_TIME 300

void MG_GetPos(uint8_t rsp, const char *lng, const char *lat)
{
	if(rsp == GPS_POS_SUCCESS)
	{	
	       char lng_str[MG_LONGITUDE_STR_SIZE+1];
              char lat_str[MG_LATITUDE_STR_SIZE+1]; 
              memset(lng_str,0,MG_LONGITUDE_STR_SIZE+1);
              memset(lat_str,0,MG_LATITUDE_STR_SIZE+1);
              memcpy(lng_str,lng,MG_LONGITUDE_STR_SIZE);
              memcpy(lat_str,lat,MG_LATITUDE_STR_SIZE);
		osiTracePrintf(OSI_LOG_TAG,"%s: lng:%s lat:%s",__func__,lng_str,lat_str);
              g_FindPosFlag = 1;
	}
	else
	{
		osiTracePrintf(OSI_LOG_TAG,"%s:Get Pos failure",__func__);		
	}	
}

static void prvGpsThread(void *param)
{
	int iret;
       uint16_t cnt;
       MG_osiThreadSleep(10000);

       //mode :DEFAULT   POS_TYPE:STANDALON
	iret = MG_GpsOpen(MG_GPS_MODE_DEFAULT,MG_GPS_POS_TYPE_STANDALON,MG_GPS_GNSS_TYPE_BDS_GPS,MG_GetPos,0);
	if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_DEFAULT failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_DEFAULT success");
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }     
       }
	iret = MG_GpsClose(GPS_CLOSE_ALL);
      osiTracePrintf(OSI_LOG_TAG,"MG_GpsClose %d",iret);
       MG_osiThreadSleep(1000);
       osiTracePrintf(OSI_LOG_TAG,"mode :DEFAULT POS_TYPE:STANDALON CLOSE ALL");
       MG_osiThreadSleep(1000);
    
	//mode :TIME   POS_TYPE:STANDALON  max_time:300s
	iret = MG_GpsOpen(MG_GPS_MODE_TIME,MG_GPS_POS_TYPE_STANDALON,MG_GPS_GNSS_TYPE_BDS,MG_GetPos,MG_GPS_MAX_MODE_TIME);
       if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME success");	
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }  
       }       
	MG_GpsClose(GPS_CLOSE_MODE);
       osiTracePrintf(OSI_LOG_TAG,"mode :TIME POS_TYPE:STANDALON CLOSE MODE");
       MG_osiThreadSleep(1000);
       
      //mode :TIME   POS_TYPE:STANDALON  max_time:300s
      	iret = MG_GpsOpen(MG_GPS_MODE_TIME,MG_GPS_POS_TYPE_STANDALON,MG_GPS_GNSS_TYPE_BDS,MG_GetPos,MG_GPS_MAX_MODE_TIME);
       if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME success");	
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }             
       }
	MG_GpsClose(GPS_CLOSE_MODE);
       osiTracePrintf(OSI_LOG_TAG,"mode :TIME POS_TYPE:STANDALON CLOSE MODE");
       MG_osiThreadSleep(1000);
       
      //mode :TIME   POS_TYPE:AGPS  max_time:300s	
	iret = MG_GpsOpen(MG_GPS_MODE_TIME,MG_GPS_POS_TYPE_AGPS,MG_GPS_GNSS_TYPE_BDS,MG_GetPos,MG_GPS_MAX_MODE_TIME);
       if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME success");	
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }   
       }
	MG_GpsClose(GPS_CLOSE_MODE);
       osiTracePrintf(OSI_LOG_TAG,"mode :TIME POS_TYPE:AGPS CLOSE MODE");
       MG_osiThreadSleep(1000);
 
	uint8_t vaild_time = MG_GpsGetEphVaildTime();
	if(vaild_time == 0)
	{
		bool active = false;
		while(!active)
		{
		       osiTracePrintf(OSI_LOG_TAG, "no active pdp context,check again");
			MG_osiThreadSleep(1000);
			active = MG_NwHasActivePdp(0);
		}
	}
    
      //mode :TIME   POS_TYPE:AGPS  max_time:300s	
	iret = MG_GpsOpen(MG_GPS_MODE_TIME,MG_GPS_POS_TYPE_AGPS,MG_GPS_GNSS_TYPE_BDS,MG_GetPos,MG_GPS_MAX_MODE_TIME);
       if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME success");	
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }               
       }

	MG_GpsClose(GPS_CLOSE_MODE);
       osiTracePrintf(OSI_LOG_TAG,"mode :TIME POS_TYPE:AGPS CLOSE MODE");
       MG_osiThreadSleep(1000);
     
      vaild_time = MG_GpsGetEphVaildTime();
      osiTracePrintf(OSI_LOG_TAG,"MG_GpsGetEphVaildTime() :%d",vaild_time);	
      MG_osiThreadSleep(1000);

      //mode :TIME   POS_TYPE:AGPS  max_time:300s	
	iret = MG_GpsOpen(MG_GPS_MODE_TIME,MG_GPS_POS_TYPE_AGPS,MG_GPS_GNSS_TYPE_BDS,MG_GetPos,MG_GPS_MAX_MODE_TIME);
       if(iret < 0)
	{
		osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME failure");	
	}
       else
       {
            osiTracePrintf(OSI_LOG_TAG,"MG_GpsOpen():MG_GPS_MODE_TIME success");	
            g_FindPosFlag = 0;
            cnt = 0;
            while(cnt < MG_GPS_MAX_MODE_TIME)
            {
                MG_osiThreadSleep(1000);
                if(g_FindPosFlag == 1)
                {
                    break;
                }
                cnt++;
            }              
       }
	MG_GpsClose(GPS_CLOSE_MODE);
       osiTracePrintf(OSI_LOG_TAG,"mode :TIME POS_TYPE:AGPS CLOSE MODE");
       MG_osiThreadSleep(1000);
       
      vaild_time = MG_GpsGetEphVaildTime();
      osiTracePrintf(OSI_LOG_TAG,"MG_GpsGetEphVaildTime() :%d",vaild_time);	
      MG_osiThreadSleep(1000);
      
      MG_GpsClose(GPS_CLOSE_ALL);
      appimg_exit();
      osiThreadExit();
}

int appimg_enter(void *param)
{
       osiTracePrintf(OSI_LOG_TAG,"gps example entry");
	osiThreadCreate("gpsthread", prvGpsThread, NULL, OSI_PRIORITY_NORMAL, 4*1024, 1);
	return 0;
}

void appimg_exit(void)
{
       osiTracePrintf(OSI_LOG_TAG,"gps example exit");
}


