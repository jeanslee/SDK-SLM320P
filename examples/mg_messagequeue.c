#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void(*osiCallback_t)(void *ctx);

static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg notify messageQueue start");
	
	osiEvent_t event = {};
	osiEvent_t *msg = &event;
	
	MG_osiThreadSleep(500);
	if(msg!= NULL)
		MG_osiTracePrintf(LOG_TAG, "mg msg: %d", msg);
	else
		MG_osiTracePrintf(LOG_TAG, "mg msg is NULL");
	
	MG_osiThreadSleep(500);
	osiMessageQueue_t *msgq = MG_osiMessageQueueCreate(10, 32);
	if(msgq != NULL)
		MG_osiTracePrintf(LOG_TAG, "mg messagequeuecreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg messagequeuecreate failed");
	
	MG_osiThreadSleep(500);
	if(MG_osiMessageQueuePut(msgq, msg))
		MG_osiTracePrintf(LOG_TAG, "mg messagequeueput success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg messagequeueput failed");
	
	MG_osiThreadSleep(500);
	msg = NULL;
	if(MG_osiMessageQueueGet(msgq, msg))
		MG_osiTracePrintf(LOG_TAG, "mg messagequeueget success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg messagequeueget failed");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg messagequeuedelete start");
	MG_osiMessageQueueDelete(msgq);
	MG_osiTracePrintf(LOG_TAG, "mg messagequeuedelete end");	
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg messagequeue end");

	MG_osiThreadExit();
}
static void prvThreadEntry1(void *param){

    int i = 0;
    osiMessageQueue_t * msgq = (osiMessageQueue_t*)param;
    char buf[10]={'a','b','c','d','e','1','2','3','4','5'};
    for(;;){
        if(msgq != NULL){
            MG_osiMessageQueuePut(msgq,buf);
            MG_osiTracePrintf(LOG_TAG, "send msg");
            MG_osiThreadSleep(500);
       }
    }
}

static void prvThreadEntry2(void *param){
    int i = 0;
    osiMessageQueue_t * msgq = (osiMessageQueue_t*)param;
    char buf[10];
    for(;;){
        if(msgq != NULL){
            if(MG_osiMessageQueueGet(msgq,buf)){
            
                MG_osiTracePrintf(LOG_TAG, "get msg %s",buf);
            }
            MG_osiThreadSleep(500);
        }
    }
}
osiMessageQueue_t * gq;
int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
    gq = MG_osiMessageQueueCreate(3,10);
	MG_osiThreadCreate("msgq1", prvThreadEntry1, gq, OSI_PRIORITY_NORMAL, 1024);
	MG_osiThreadCreate("msgq2", prvThreadEntry2, gq, OSI_PRIORITY_NORMAL, 1024);
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_event_messagequeue exit");
}
