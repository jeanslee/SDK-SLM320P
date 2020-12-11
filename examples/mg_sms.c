
/*
** FILE NAME: 
**   mg_sms.c
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

2020-07-04   LiWeihua        Added event handler and sms receiver.
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
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'S', 'M')


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
    OSI_LOGI(0, "meigsms application thread enter, param 0x%x", param);

    osiThread_t * current = osiThreadCurrent();
    MG_regEventThread(current);

    osiThreadSleep(10000);

    MG_setVOLTE(0, 1);
    osiThreadSleep(5000);

    MG_SendSmsText(0, "13818475936", "SMS¶ÌÏûÏ¢");

    osiThreadSleep(10000);

    // event handler
    for(;;)
    {
        osiEvent_t event = {};		

        osiEventTryWait(current, &event,500);
        OSI_LOGI(0,"[MGSM]got event:%d",event.id);

        switch(event.id)
        {
            case EV_CFW_NEW_SMS_IND:
                {
                    const CFW_EVENT *cfw_event = (const CFW_EVENT *)&event;
                    CFW_NEW_SMS_NODE_EX *pNewMsgNode = (CFW_NEW_SMS_NODE_EX *)cfw_event->nParam1;
                    char rsp[128];

                    /* check the msg node type */
                    switch (pNewMsgNode->node.nType)
                    {
                        case 0x21: // 1. Text Message Content with header
                        case 0x22: // 2. Text Message Content without header
                        case 0x5:  // 3. MMS PUSH header
                            {
                                CFW_SMS_TXT_HRD_IND_V2 *pTextWithHead = (CFW_SMS_TXT_HRD_IND_V2 *)&pNewMsgNode->info;							
                                char oaNumber[TEL_NUMBER_MAX_LEN + 2];
                                MG_smsNumberStr(pTextWithHead->oa, pTextWithHead->oa_size, pTextWithHead->tooa, oaNumber);

                                MG_osiTracePrintf(OSI_LOG_TAG, "[MGSM] %d got new SMS from %s", event.id, oaNumber);
					
                                sprintf(rsp, "+CMT: \"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\"",
                                oaNumber, pTextWithHead->scts.uYear, pTextWithHead->scts.uMonth,
                                pTextWithHead->scts.uDay, pTextWithHead->scts.uHour, pTextWithHead->scts.uMinute,
                                pTextWithHead->scts.uSecond, pTextWithHead->scts.iZone);
                                MG_osiTracePrintf(OSI_LOG_TAG, "%s", rsp);
										
                               char *data = MG_smsNodeDataStr(pNewMsgNode->node.nType, &pNewMsgNode->info, 0);     
				   MG_osiTracePrintf(OSI_LOG_TAG, "%s", data);
                            }
                            break;

                        default:
                            break;			  
                    }
                }
                break;				

	     default:
		 break;
        }
    }	
	
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
    OSI_LOGI(0, "meigsms application image enter, param 0x%x", param);

    osiThreadCreate("meigsms", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 6);
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
    OSI_LOGI(0, "meigsms application image exit");
}

