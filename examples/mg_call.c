
/*
** FILE NAME: 
**   mg_call.c
** 
** FILE DESCRIPTION: 
**   MEIG Proprietary Commands.
** 
** Copyright (C) 2020 MeiG Smart Technology Co., Ltd (and/or its affiliates "MEIG").
** All rights reserved.
** MEIG Confidential and Proprietary.
*/


/*===========================================================================
                        EDIT HISTORY

  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.

When             Who              What, Where, Why
----------   ----------   ---------------------------------------------------

2020-05-30   LiWeihua        Added example to hold/active call.
2020-05-29   LiWeihua        Added example to answer call.
2020-05-28   LiWeihua        Added example to handup call.
2020-05-21   LiWeihua        Added example to dial a voice call.
2020-05-21   LiWeihua        Created initial version.
===========================================================================*/


/*===========================================================================
                            INCLUDE FILES
===========================================================================*/
#include "osi_log.h"
#include "osi_api.h"

#include "cfw_event.h"
#include "cfw.h"

/*===========================================================================

                                                   LOCAL DEFINITIONS AND DECLARATIONS

  This section contains local definitions for constants, macros, types,  variables and other items needed by this module.

===========================================================================*/
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'C', 'C')


/*===========================================================================

                                                     FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================
   
                                                      FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
DESCRIPTION:
  prvThreadEntry.

PARAMETER:
  
RETURN:
  
REMARK:
   
===========================================================================*/
static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "meigcall application thread enter, param 0x%x", param);

    osiThread_t * current = osiThreadCurrent();
    MG_regEventThread(current);

    osiThreadSleep(10000);

    // enable VoLTE:
    MG_setVOLTE(0, 1);
    osiThreadSleep(5000);

    // dial a voice call (you can change the number to try):
    MG_VoiceCallDial(0,"12121");
    OSI_LOGI(0, "MEIG/LiWeihua: hold/active call test");
    osiThreadSleep(20000);
    // hold the call:
    MG_VoiceCallHoldActive(0);

    osiThreadSleep(20000);
    // active the holding call:
    MG_VoiceCallHoldActive(0);

    osiThreadSleep(10000);
    // release the call:
    MG_VoiceCallHangup(0);

    OSI_LOGI(0, "MEIG/LiWeihua: try to call the device for test in 20 seconds");
    osiThreadSleep(20000);
    // suppose there is an incoming call (please try to call the device for test):
    // answer the call:
    MG_VoiceCallAnswer(0);

    // event handler
    for(;;)
    {
        osiEvent_t event = {};		

        osiEventTryWait(current, &event,-1);//500
        OSI_LOGI(0,"[MGCC]got event:%d",event.id);

        switch(event.id)
        {
            case EV_CFW_CC_SPEECH_CALL_IND:
                {
                    const CFW_EVENT *cfw_event = (const CFW_EVENT *)&event;
                    CFW_SPEECH_CALL_IND *call_info = (CFW_SPEECH_CALL_IND *)cfw_event->nParam1;
                    char num_str[TEL_NUMBER_MAX_LEN + 2];

                    MG_cfwBcdToDialString(call_info->TelNumber.nTelNumber, call_info->TelNumber.nSize, num_str);

                    MG_osiTracePrintf(OSI_LOG_TAG, "[MGCC] %d got incoming call %s, to answer...", event.id, num_str);
                    MG_VoiceCallAnswer(0);	
                }
                break;				

	     default:
		 break;
        }
    }

    MG_unregEventThread(current);

    osiThreadExit();
}

/*===========================================================================
DESCRIPTION:
  appimg_enter.

PARAMETER:
  
RETURN:
  
REMARK:
   
===========================================================================*/
int appimg_enter(void *param)
{
    OSI_LOGI(0, "meigcall application image enter, param 0x%x", param);

    osiThreadCreate("meigcall", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 6);
    return 0;
}

/*===========================================================================
DESCRIPTION:
  appimg_exit.

PARAMETER:
  
RETURN:
  
REMARK:
   
===========================================================================*/
void appimg_exit(void)
{
    OSI_LOGI(0, "meigcall application image exit");
}

