#ifndef __MG_ALARM_API__
#define __MG_ALARM_API__

#include<stdint.h>
#include "drv_rtc.h"
#include "cfw.h"
#include "at_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_ATS_ALARM_TEXT_LEN_ 32
#define ALARM_API  998

extern atSetting_t gAtSetting;

typedef struct
{
    char text[CONFIG_ATS_ALARM_TEXT_LEN_ + 1];
    uint8_t has_timezone;
    int8_t timezone;
    uint8_t silent;
} atAlarmInfo_tt;

//function
uint8_t  MG_SetAlarm(uint32_t owner,const char *alarmTime ,uint8_t index ,uint8_t type ,const char  *alarmText,const char *alarmRecurr, uint8_t silent);
uint32_t MG_GetRtcGetAlarmCount(uint32_t owner);
uint32_t MG_GetRtcGetAlarms(uint32_t owner, drvRtcAlarm_t *alarms, uint32_t max_count);
bool     MG_GelRtcRemoveAlarm(uint32_t owner, uint32_t name);
uint8_t  MG_SetAlarmCallback(uint32_t owner,drvRtcAlarmCB_t cb);

#ifdef __cplusplus
}
#endif

#endif //__MG_ALARM_API__
