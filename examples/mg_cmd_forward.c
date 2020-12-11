/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "osi_log.h"
#include "osi_api.h"
#include "osi_pipe.h"
#include "cfw_event.h"

osiPipe_t * pipe;
#define EV_MMI_EV_BASE 0x00100000
#define EV_USER_UART_RESP (EV_MMI_EV_BASE + 2)
#define EV_USER_FORWARD_CMD (EV_MMI_EV_BASE + 4)

void at_cmd_cb(char*buf,int len){
    OSI_LOGI(0, "custom cmd callback");
    MG_sendUart(buf);
}
static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "application thread enter, param 0x%x", param);

    int rev = 0;
    osiThread_t * eng = MG_regEventThread(osiThreadCurrent());
    MG_regCmdCallback(at_cmd_cb);
    uint8_t buf[256]={0};
    for (;;)
    {

        pipe = MG_getPipe();
        osiEvent_t event;
        event.id = OSI_EVENT_ID_NONE;

        osiEventTryWait(osiThreadCurrent(), &event,1000);
        if(event.id != 0)
            OSI_LOGI(0, "receive event %d", event.id);
        if (event.id == OSI_EVENT_ID_QUIT)
            break;
        else if(event.id == EV_USER_FORWARD_CMD){
            /*
            int len = event.param1;
            int received = 0;
            int bytes = osiPipeRead(pipe, buf, sizeof(buf));         
            if(bytes <len)
                MG_osiTracePrintf(OSI_LOG_TAG, "err:received cmd %s", buf);
            else{
                MG_osiTracePrintf(OSI_LOG_TAG, "received cmd %s", buf);
                //todo process custom command
            }
                
            //send response 
            osiEvent_t evt;
            evt.id = EV_USER_UART_RESP;
            evt.param1 = 0;
            evt.param2 = 0;
            evt.param3 = 0;
            osiEventSend(eng, &evt);
            */
        }
    }

    MG_unregEventThread(osiThreadCurrent());

    osiThreadExit();
}

int appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);

    osiThread_t * thread = osiThreadCreate("mythread", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 2);

    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
