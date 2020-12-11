#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg mutex start");
	
	MG_osiThreadSleep(500);
	osiMutex_t *mutex = NULL;
	mutex = MG_osiMutexCreate();
	if(mutex != NULL)
		MG_osiTracePrintf(LOG_TAG, "mg mutexcreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg mutexcreate failed");

	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg mutexlock start");
	MG_osiMutexUnlock(mutex);
	MG_osiTracePrintf(LOG_TAG, "mg mutexlock end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg mutexunlcok start");
	MG_osiMutexLock(mutex);
	MG_osiTracePrintf(LOG_TAG, "mg mutexunlcok end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg mutexdelete start");
	MG_osiMutexDelete(mutex);
	MG_osiTracePrintf(LOG_TAG, "mg mutexdelete end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg mutex end");

	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_event_mutex", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_event_mutex exit");
}
