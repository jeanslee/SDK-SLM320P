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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('G', 'P', 'S', 'M')

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "hal_chip.h"


#define gps_m_assert(b)  \
    do                  \
    {                   \
        if (!(b))       \
            osiPanic(); \
    } while (0)




typedef struct
{
	/* Information related to GPGGA statement */
	double latitude;                       /*!< Latitude in units of degrees */
	double longitude;                      /*!< Longitude in units of degrees */
	double altitude;                       /*!< Altitude in units of meters */
	double geo_sep;                        /*!< Geoid separation in units of meters */
	uint8_t sats_in_use;                        /*!< Number of satellites in use */
	uint8_t fix;                                /*!< Fix status. `0` = invalid, `1` = GPS fix, `2` = DGPS fix, `3` = PPS fix */
	uint8_t hours;                              /*!< Hours in UTC */
	uint8_t minutes;                            /*!< Minutes in UTC */
	uint8_t seconds;                            /*!< Seconds in UTC */
	//add by zx 20200608
	uint8_t date;                               /*!< Fix date */
    	uint8_t month;                              /*!< Fix month */
    	uint8_t year;                                /*!< Fix year */
}MG_gps_t;


 
	

static void prvGpsThread(void *param)
{
	
	MG_gps_t  Gps;
	uint8_t buffer[512]={0};
	uint32_t length=0;
       OSI_LOGI(0, "gps example entry");	
	   
	MG_gps_open();
	osiThreadSleep(3000);
	
	for (;;)
	{
		OSI_LOGI(0, "gps example get location");
		MG_get_gps_location(&Gps,10);
		osiThreadSleep(1000);
		osiTracePrintf(OSI_LOG_TAG,"lat:%f,lon:%f,alti:%f,Geid:%f",Gps.latitude,Gps.longitude,Gps.altitude,Gps.geo_sep);
       	osiTracePrintf(OSI_LOG_TAG,"hour:%x,minutes:%x,second:%x,date:%x,month:%x,year:%x,Stas_use:%x,fix:%x",Gps.hours,Gps.minutes,Gps.seconds,Gps.date,Gps.month,Gps.year,Gps.sats_in_use,Gps.fix);

		length=MG_get_gps_nmea(buffer,512);
		/*print NMEA data*/
		OSI_LOGI(0,"=====gps nmea data=====");
		OSI_PRINTFI("%s",buffer);
		OSI_LOGI(0,"=====gps nmea data size is %ld=====",length);
	}
	MG_gps_close();
	
	osiThreadExit();
}

int appimg_enter(void *param)
{
	OSI_LOGI(0, "gps example entry");

	osiThreadCreate("gpsthread", prvGpsThread, NULL, OSI_PRIORITY_NORMAL, 16*1024, 0);
	return 0;
}

void appimg_exit(void)
{
	OSI_LOGI(0, "gps example exit");
}


