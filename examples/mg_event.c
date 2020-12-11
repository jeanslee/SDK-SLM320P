#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

//#include "audio_player.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void(*osiCallback_t)(void *ctx);

void callBack_test()
{
	MG_osiTracePrintf(LOG_TAG, "callback");
}

static void prvThreadEntry(void *param)
{
	osiThread_t *thread = MG_osiThreadCurrent();
	osiEvent_t event = {};
	
	MG_osiThreadSleep(1000);
	if(MG_osiEventSend(thread, &event))
		MG_osiTracePrintf(LOG_TAG, "mg_eventsent success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg_eventsent failed");
	
	MG_osiThreadSleep(1000);
	if(MG_osiEventWait(thread, &event))
		MG_osiTracePrintf(LOG_TAG, "mg_eventwait success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg_eventsent failed");

	MG_osiThreadSleep(1000);
	if(MG_osiThreadCallback(thread, callBack_test, NULL))
		MG_osiTracePrintf(LOG_TAG, "mg_threadcallback success");
	else
		MG_osiTracePrintf(LOG_TAG, "mg_threadcallback failed");
	
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_event_test", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_event_test exit");
}

/*
bool osiEventSend(osiThread_t *thread, const osiEvent_t *event)

	send an event to a thread
	At send, the body of event will be copied to event queue rather then send the pointer of event.
	When event queue of the target thread is full, 
	the caller thread will be block until there are rooms in target thread event queue.

	Return
	• true on success
	• false on invalid parameter
	Parameters
	• thread: thread pointer, can’t be NULL
	• event: event to be sent

bool osiEventWait(osiThread_t *thread, osiEvent_t *event)

	wait an event
	Wait an event from current thread event queue. When current thread event queue is empty, it will be blocked
	forever until there are event in event queue.
	The event body will be copied to event.
	There are some event ID used by system. 
	After system event is received and process, this will return an event with ID of 0. 
	Application can ignore event ID 0 safely.

	Return
	• true on success
	• false on invalid parameter
	Parameters
	• thread: thread pointer, can’t be NULL
	• event: event pointer
	
	
bool osiThreadCallback(osiThread_t *thread, osiCallback_t cb, void *cb_ctx)
	set callback to be executed on thread
	Thread callback is implemented by osiEvent_t.
	This can be called in ISR. In ISR, the callback event will be lost when the event queue of target thread is full.

	Return
	• true on success
	• false on invalid parameter, or event queue is full in ISR
	Parameters
	• thread: thread pointer, can’t be NULL
	• cb: callback to be executed
	• cb_ctx: callback context
	
*/