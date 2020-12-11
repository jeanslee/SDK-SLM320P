#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "osi_fifo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg fifo start");
	
	MG_osiThreadSleep(500);
	osiFifo_t *fifo = (osiFifo_t *)malloc(sizeof(osiFifo_t));
	osiEvent_t event = {};
	osiEvent_t *data = &event;
	size_t size = 4;
	if(MG_osiFifoInit(fifo, data, size))
		MG_osiTracePrintf(LOG_TAG, "mg fifocreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg fifocreate failed");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifoput start");
	MG_osiFifoPut(fifo, data, size);
	MG_osiTracePrintf(LOG_TAG, "mg fifoput end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifoget start");
	MG_osiFifoGet(fifo, data, size);
	MG_osiTracePrintf(LOG_TAG, "mg fifoget end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifo peek start");
	MG_osiFifoPeek(fifo, data, size);
	MG_osiTracePrintf(LOG_TAG, "mg fifo peek end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifo skipbytes start");
	MG_osiFifoSkipBytes(fifo, size);
	MG_osiTracePrintf(LOG_TAG, "mg fifo skipbytes end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifo reset start");
	MG_osiFifoReset(fifo);
	MG_osiTracePrintf(LOG_TAG, "mg fifo reset end");
	
	if(MG_osiFifoSearch(fifo, 4, true))
		MG_osiTracePrintf(LOG_TAG, "mg fifo search true");
	else
		MG_osiTracePrintf(LOG_TAG, "mg fifo search false");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg fifo end");
	
	free(fifo);

	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_fifo", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_fifo exit");
}
