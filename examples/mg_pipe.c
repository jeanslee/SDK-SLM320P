#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "osi_pipe.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg pipe start");
	
	MG_osiThreadSleep(500);
	
	unsigned size = 4;
	void *buf = NULL;
	unsigned timeout = 100;
	osiPipeEventCallback_t cb = NULL;
	
	osiPipe_t *pipe = MG_osiPipeCreate(size);
	if(pipe != NULL)
		MG_osiTracePrintf(LOG_TAG, "mg pipecreate success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg pipecreate failed");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipesetwritercallback start");
	MG_osiPipeSetWriterCallback(pipe, size, cb, buf);
	MG_osiTracePrintf(LOG_TAG, "mg pipesetwritercallback end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipesetreadercallback start");
	MG_osiPipeSetReaderCallback(pipe, size, cb, buf);
	MG_osiTracePrintf(LOG_TAG, "mg pipesetreadercallback end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg piperead start");
	MG_osiPipeRead(pipe, buf, size);
	MG_osiTracePrintf(LOG_TAG, "mg piperead end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipewrite start");
	MG_osiPipeWrite(pipe, buf, size);
	MG_osiTracePrintf(LOG_TAG, "mg pipewrite end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipewriteall start");
	MG_osiPipeWriteAll(pipe, buf, size, timeout);
	MG_osiTracePrintf(LOG_TAG, "mg pipewriteall end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipereadavail start");
	MG_osiPipeReadAvail(pipe);
	MG_osiTracePrintf(LOG_TAG, "mg pipereadavail end");
	
	MG_osiThreadSleep(500);
	MG_osiTracePrintf(LOG_TAG, "mg pipewriteavail start");
	MG_osiPipeWriteAvail(pipe);
	MG_osiTracePrintf(LOG_TAG, "mg pipewriteavail end");
	
	MG_osiThreadSleep(1500);
	if(MG_osiPipeWaitReadAvail(pipe, timeout))
		MG_osiTracePrintf(LOG_TAG, "mg pipewaitreadavail true");
	else
		MG_osiTracePrintf(LOG_TAG, "mg pipewaitreadavail false");
	
	MG_osiThreadSleep(1500);
	if(MG_osiPipeWaitWriteAvail(pipe, timeout))
		MG_osiTracePrintf(LOG_TAG, "mg pipewaitwriteavail true");
	else
		MG_osiTracePrintf(LOG_TAG, "mg pipewaitwriteavail false");
	
	MG_osiThreadSleep(1500);
	MG_osiTracePrintf(LOG_TAG, "mg pipeseteof start");
	MG_osiPipeSetEof(pipe);
	MG_osiTracePrintf(LOG_TAG, "mg pipeseteof end");
	
	MG_osiThreadSleep(1500);
	if(MG_osiPipeIsEof(pipe))
		MG_osiTracePrintf(LOG_TAG, "mg pipeiseof ");
	else
		MG_osiTracePrintf(LOG_TAG, "mg pipenoteof ");
	
	MG_osiThreadSleep(1500);
	MG_osiTracePrintf(LOG_TAG, "mg pipestop start");
	MG_osiPipeStop(pipe);
	MG_osiTracePrintf(LOG_TAG, "mg pipestop end");
	
	MG_osiThreadSleep(1500);
	if(MG_osiPipeIsStopped(pipe))
		MG_osiTracePrintf(LOG_TAG, "mg pipeisstopped ");
	else
		MG_osiTracePrintf(LOG_TAG, "mg pipenotstopped ");
		
	MG_osiThreadSleep(1500);
	MG_osiTracePrintf(LOG_TAG, "mg pipedelete start");
	MG_osiPipeDelete(pipe);
	MG_osiTracePrintf(LOG_TAG, "mg pipedelete end");
	
	MG_osiThreadSleep(1500);
	MG_osiTracePrintf(LOG_TAG, "mg pipe end");

	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_pipe", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_pipe exit");
}
