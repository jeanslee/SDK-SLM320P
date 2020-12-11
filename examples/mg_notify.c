#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void(*osiCallback_t)(void *ctx);

static void prvThreadEntry(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg notify start");
	osiThread_t *thread = MG_osiThreadCurrent();
	osiEvent_t event = {};	
	osiCallback_t callback = 123;
	
	MG_osiThreadSleep(200);
	osiNotify_t *notify = MG_osiNotifyCreate(thread, callback, 456);
	
	if(notify != NULL)
		MG_osiTracePrintf(LOG_TAG, "mg notifycreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg notifycreate failed");
	
	MG_osiThreadSleep(200);
	MG_osiTracePrintf(LOG_TAG, "mg notifytrigger begain");
	MG_osiNotifyTrigger(notify);
	MG_osiTracePrintf(LOG_TAG, "mg notifytrigger end");
	
	MG_osiThreadSleep(200);
	MG_osiTracePrintf(LOG_TAG, "mg notifycancel begain");
	MG_osiNotifyCancel(notify);
	MG_osiTracePrintf(LOG_TAG, "mg notifycancel end");
	
	MG_osiThreadSleep(200);
	MG_osiTracePrintf(LOG_TAG, "mg notifydelete begain");
	MG_osiNotifyDelete(notify);
	MG_osiTracePrintf(LOG_TAG, "mg notifydelete end");
	

	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_event_notify", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_event_test exit");
}
