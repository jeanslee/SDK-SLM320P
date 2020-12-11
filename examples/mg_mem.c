#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "osi_mem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg mem start");
	
	int *p = NULL;
	int *p1 = NULL;
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg allocsize start");
	MG_osiMemAllocSize(p);
	MG_osiTracePrintf(LOG_TAG, "mg allocsize end");
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg ref start");
	MG_osiMemRef(p);
	MG_osiTracePrintf(LOG_TAG, "mg ref end");
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg refcount start");
	MG_osiMemRefCount(p);
	MG_osiTracePrintf(LOG_TAG, "mg refcount end");
	
	MG_osiTracePrintf(LOG_TAG, "mg setcaller start");
	MG_osiThreadSleep(1000);	
	MG_osiMemSetCaller(p, p1);
	MG_osiTracePrintf(LOG_TAG, "mg setcaller end");
    
    int *a = (int *)osiMalloc(sizeof(int));
    int *b = (int *)osiCalloc(1, sizeof(int));
    osiFree(a);
    osiFree(b);
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg mem end");
	
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_mem", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_mem exit");
}
