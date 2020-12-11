#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg semaphore start");
	
	uint32_t max_count = 10;
	uint32_t init_count = 1;
	
	osiSemaphore_t *sem = MG_osiSemaphoreCreate(max_count, init_count);
	
	MG_osiThreadSleep(1000);
	if(sem != NULL)
		MG_osiTracePrintf(LOG_TAG, "mg semaphorecreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg semaphorecreate failed");

	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg semaphoreacquire strat");
	MG_osiSemaphoreAcquire(sem);
	MG_osiTracePrintf(LOG_TAG, "mg semaphoreacquire end");

	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg semaphorerelease start");
	MG_osiSemaphoreRelease(sem);
	MG_osiTracePrintf(LOG_TAG, "mg semaphorerelease end");
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg semaphoredelete start");
	MG_osiSemaphoreDelete(sem);
	MG_osiTracePrintf(LOG_TAG, "mg semaphoredelete end");
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg semaphore end");
	
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_semaphore", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_seamphore exit");
}
