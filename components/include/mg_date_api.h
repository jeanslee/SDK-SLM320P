#ifndef _MG_DATA_API_H_
#define _MG_DATA_API_H_


#include "osi_log.h"
#include"mg_date_api.h"
#include <string.h>
#include <stdlib.h>
#include "osi_api.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>



typedef struct
{
    uint16_t nYear;
    uint8_t  nMonth;
    uint8_t  nDay;
    uint8_t  nHour;
    uint8_t  nMin;
    uint8_t  nSec;
    uint8_t  DayIndex; /* 0=Sunday */
} MG_date;


//static int64_t MG_timerSwitch(MG_date  *date,int offset);

int  MG_GetCurDate(MG_date * date);

int MG_SetCurDate(MG_date  *date,  int offset);

int  MG_Utc_to_Rtc(MG_date  *utc_date,MG_date  *rtc_date,int offset);

int  MG_Rtc_to_Utc(MG_date  *rtc_date,MG_date  *utc_date,int offset);

int MG_UtcToSec(MG_date  *utc_date);

int MG_RtcToSec(MG_date  *rtc_date);

int  MG_SecToUtc(MG_date  *utc_date, int sec);

int  MG_SecToRtc(MG_date  *rtc_date, int sec);

int MG_LocalToUtc(int sec,int offset);

int MG_UtcToLocal(int sec,int offset);


#endif




