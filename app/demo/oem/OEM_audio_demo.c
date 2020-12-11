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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "mg_audio_api.h"
#include "drv_keypad.h"

#include "oem.h"

#define TIME_20MS (20)
#define RECORD_TIME_MS (3000)
#define PCM_8K_16BIT_20MS_BYTES (320)
#define STREAM_DEMO_BUFFER_SIZE   ((RECORD_TIME_MS/TIME_20MS)*PCM_8K_16BIT_20MS_BYTES)    //(1024*500)

#define PLAY_DEMO_BUFFER_SIZE    PCM_8K_16BIT_20MS_BYTES
#define PTT_KEY_RELEASE 2
#define PTT_KEY_PRESSED 1
uint8_t g_PttKeyStastus;

#define AUDIO_IDLE 0
#define AUDIO_BUSY_RECORD_START 1
#define AUDIO_BUSY_RECORD_END 2
#define AUDIO_BUSY_PLAY_START 3
#define AUDIO_BUSY_PLAY_RUN 4
#define AUDIO_BUSY_PLAY_END 5

static uint8_t g_AudioStastus;

static uint8_t *g_recordbuf = NULL;
static uint32_t g_recordrevcount;
static uint8_t g_recordrevflag;

int capture_callback(uint8_t *data, uint32_t len)
{		
	if(g_recordrevflag == 1)
      {
           osiTracePrintf(OSI_LOG_TAG,"capture_callback():g_recordrevflag %d",g_recordrevflag);
	    return 0;
	}
	if(g_recordrevcount >= STREAM_DEMO_BUFFER_SIZE)
	{
		osiTracePrintf(OSI_LOG_TAG,"capture_callback():not have enough buf to save, then stop record");				
        //MG_AudioStreamRecordStop();
		g_recordrevflag =1;
		return 0;
	}
	memcpy(g_recordbuf + g_recordrevcount,data,len); 
	g_recordrevcount += len;
	osiTracePrintf(OSI_LOG_TAG,"capture_callback():len: %d,sum len: %d",len,g_recordrevcount);
	return 0;
}


static int play_stream_frame(uint8_t *buf ,uint32_t size)
{
	if(buf == NULL)
	{
		return -1;
	}
	int iret;
	uint32_t pos= 0;
	uint32_t len = 0;
	char rsp[64];
	for(;;)
	{
		if(pos >= size)
		{
           break;
		}
		
		len = size - pos; 
		if(len < PLAY_DEMO_BUFFER_SIZE)
		{
			iret = MG_AudioStreamPlay(MG_AUSTREAM_FORMAT_PCM,buf+pos,len);			
			if(iret < 0)
			{
				return -1;				
			}
			pos = pos + len;
		}
        else
        {
			iret = MG_AudioStreamPlay(MG_AUSTREAM_FORMAT_PCM,buf+pos,PLAY_DEMO_BUFFER_SIZE);			
			if(iret < 0)
			{				
				return -1;
			}	
			pos = pos + PLAY_DEMO_BUFFER_SIZE;		
		}		        				
	}
	//MG_osiThreadSleep(200);
	return 0;
}

void OEM_RrcordThread(void *param)
{
	int iret;
	char rsp[64];
	
	g_recordbuf = malloc(STREAM_DEMO_BUFFER_SIZE);
	if(g_recordbuf == NULL)
	{
		osiTracePrintf(OSI_LOG_TAG,"malloc():malloc failure");
		osiThreadExit();
		return;
	}
	g_PttKeyStastus = (uint8_t)PTT_KEY_RELEASE;
	g_AudioStastus = (uint8_t) AUDIO_IDLE;
	for(;;)
	{
		if(g_AudioStastus == (uint8_t)AUDIO_BUSY_RECORD_START)
		{
			if((g_PttKeyStastus == (uint8_t)PTT_KEY_RELEASE)&&
				(g_recordrevflag == 1))
			{				
				g_AudioStastus = (uint8_t)AUDIO_BUSY_RECORD_END;
			}
		}
		else if(g_AudioStastus == (uint8_t)AUDIO_BUSY_RECORD_END)
		{
			MG_AudioStreamRecordStop();
			g_recordrevflag =0;
			memset(rsp,0,64);	
			sprintf(rsp,"record end");
			MG_SendMessage_to_AT_Serial(rsp,strlen(rsp));
			osiTracePrintf(OSI_LOG_TAG,"record end");	
			g_AudioStastus = (uint8_t)AUDIO_BUSY_PLAY_START;
		}	
        else if(g_AudioStastus == (uint8_t)AUDIO_BUSY_PLAY_START)	
	    {
			iret = MG_AudioPlayStart();
			if(iret < 0)
			{
				osiTracePrintf(OSI_LOG_TAG,"MG_AudioPlayStart():start failure");
				continue;
			}
			memset(rsp,0,64);	
			sprintf(rsp,"play start");
			MG_SendMessage_to_AT_Serial(rsp,strlen(rsp));
			g_AudioStastus = (uint8_t)AUDIO_BUSY_PLAY_RUN;
		}	
		else if(g_AudioStastus == (uint8_t)AUDIO_BUSY_PLAY_RUN)
		{				
			iret = play_stream_frame(g_recordbuf,STREAM_DEMO_BUFFER_SIZE);
			if(iret < 0)
			{
				osiTracePrintf(OSI_LOG_TAG,"MG_AudioPlayStart():start failure");
			}			
			g_AudioStastus = (uint8_t)AUDIO_BUSY_PLAY_END;			
		}
		else if(g_AudioStastus == (uint8_t)AUDIO_BUSY_PLAY_END)
		{
			MG_AudioPlayEnd();			
			memset(rsp,0,64);	
			sprintf(rsp,"play end");
			MG_SendMessage_to_AT_Serial(rsp,strlen(rsp));
			g_AudioStastus = (uint8_t) AUDIO_IDLE;	
		}		
		else if((g_PttKeyStastus == (uint8_t)PTT_KEY_PRESSED) &&
			(g_AudioStastus == (uint8_t)AUDIO_IDLE))
		{	
		
			g_recordrevcount = 0;
			g_recordrevflag = 0;		
			memset(g_recordbuf,0,STREAM_DEMO_BUFFER_SIZE);
			iret = MG_AudioStreamRecordStart(capture_callback,MG_AUSTREAM_FORMAT_PCM,PCM_8K_16BIT_20MS_BYTES);
			if(iret < 0)
			{
				osiTracePrintf(OSI_LOG_TAG,"MG_AudioStreamRecordStart():start failure");
				continue;		
			}
			g_AudioStastus = (uint8_t)AUDIO_BUSY_RECORD_START;
			
			memset(rsp,0,64);	
			sprintf(rsp,"record start");
			MG_SendMessage_to_AT_Serial(rsp,strlen(rsp));
			osiTracePrintf(OSI_LOG_TAG,"record start"); 						
		}		
		else if(g_AudioStastus == (uint8_t)AUDIO_IDLE)
		{

			MG_osiThreadSleep(1000);	
			//g_PttKeyStastus = (uint8_t)PTT_KEY_PRESSED;
		}
	}
	
    free(g_recordbuf);
    MG_osiThreadSleep(1000);
    osiThreadExit();
}

