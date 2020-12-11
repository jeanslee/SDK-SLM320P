/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('E', 'A', 'U', 'R')

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "mg_audio_api.h"


#define STREAM_DEMO_BUFFER_SIZE  (1024*500)
#define PCM_8K_16BIT_20MS_BYTES (320)
#define RECORD_COUNT (2)

static uint8_t *g_recordbuf = NULL;
static uint32_t g_recordrevcount;
static uint8_t g_recordrevflag;
static uint8_t g_playfinishflag;

int capture_callback(uint8_t *data, uint32_t len)
{		
	if(g_recordrevflag == 1)
      {
           osiTracePrintf(OSI_LOG_TAG,"capture_callback():g_recordrevflag %d",g_recordrevflag);
	    return;
	}
	if(g_recordrevcount >= STREAM_DEMO_BUFFER_SIZE)
	{
		osiTracePrintf(OSI_LOG_TAG,"capture_callback():not have enough buf to save, then stop record");		
        MG_AudioStreamRecordStop();
		g_recordrevflag =1;	 
		return;
	}
	memcpy(g_recordbuf + g_recordrevcount,data,len); 
	g_recordrevcount += len;
	osiTracePrintf(OSI_LOG_TAG,"capture_callback():len: %d,sum len: %d",len,g_recordrevcount);
}

int play_callback(uint8_t rsp)
{
    if(rsp == AUDIO_RSP_PLAY_FINISH)
    {
        g_playfinishflag = 1;
    }
}

static void prvRrcordThread(void *param)
{
	int iret;
	g_recordbuf = malloc(STREAM_DEMO_BUFFER_SIZE);
	if(g_recordbuf == NULL)
	{
            osiTracePrintf(OSI_LOG_TAG,"malloc():malloc failure");
            osiThreadExit();
            return;
	}
	else
	{
		MG_osiThreadSleep(10000);
		g_recordrevcount = 0;
		g_recordrevflag = 0;
		memset(g_recordbuf,0,STREAM_DEMO_BUFFER_SIZE);
		iret = MG_AudioStreamRecordStart(capture_callback,MG_AUSTREAM_FORMAT_PCM,PCM_8K_16BIT_20MS_BYTES);
		if(iret < 0)
		{
			osiTracePrintf(OSI_LOG_TAG,"MG_AudioStreamRecordStart():start failure");	
		}	
	}

	 for(;;)
	{
            if(g_recordrevflag == 1)               
	    {	
	        break;
           }
           MG_osiThreadSleep(1000);
       }
     /*
       int i = 0;
       while(i < RECORD_COUNT)
	{	
	    osiTracePrintf(OSI_LOG_TAG,"recorder : %d",g_recordrevflag);    
	    if(g_recordrevflag == 1)
	    {	         
                i++;
                osiTracePrintf(OSI_LOG_TAG,"RECORD_COUNT : %d",i);
                MG_osiThreadSleep(1000);
                g_recordrevcount = 0;
                g_recordrevflag = 0;
                memset(g_recordbuf,0,STREAM_DEMO_BUFFER_SIZE);
                iret = MG_AudioStreamRecordStart(capture_callback,PCM_8K_16BIT_20MS_BYTES);
                if(iret < 0)
                {
                    osiTracePrintf(OSI_LOG_TAG,"MG_AudioStreamRecordStart():start failure");
                    break;
                }
                
	    }	
           MG_osiThreadSleep(1000);
           
	}
     MG_osiThreadSleep(600000);
 */
    g_playfinishflag = 0; 
    iret = MG_AudioPlayStart();
    if(iret < 0)
    {
        osiTracePrintf(OSI_LOG_TAG,"MG_AudioPlayStart():start failure");
    }
    else
    {
        iret = MG_AudioPlayMem(play_callback,MG_AUSTREAM_FORMAT_PCM,g_recordbuf,STREAM_DEMO_BUFFER_SIZE);
        if(iret < 0)
        {
            osiTracePrintf(OSI_LOG_TAG,"MG_AudioPlayMem():start failure");
        }
    }
    
    for(;;)
    {
        if(g_playfinishflag == 1)               
        {	
            MG_AudioPlayEnd();
            break;
       }
       MG_osiThreadSleep(1000);
    }
   
  
    free(g_recordbuf);
    MG_osiThreadSleep(1000);
    appimg_exit();
    osiThreadExit();
}

int appimg_enter(void *param)
{
    osiTracePrintf(OSI_LOG_TAG,"record example entry");
	osiThreadCreate("record", prvRrcordThread, NULL, OSI_PRIORITY_NORMAL, 4*1024, 4);
	return 0;
}

void appimg_exit(void)
{
       osiTracePrintf(OSI_LOG_TAG,"record example exit");
}


