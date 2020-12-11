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

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "mg_os_api.h"
#include <string.h>

osiThread_t *threadid_wait = NULL;
osiThread_t *threadid_send = NULL;
osiThread_t *threadid_timer = NULL;
osiTimer_t *ptimer_t = NULL;
osiPipe_t *pipe = NULL;
osiMutex_t *mutex = NULL;
osiSemaphore_t *sema = NULL;
osiMessageQueue_t *queue = NULL;

#define MG_TIME_ZONE_SECOND (15 * 60)


typedef struct msg{
    uint32_t messageid;
    uint8_t data[8];
}msg_t;
msg_t g_msg;


#define thread_num  3

static void OSWaitEntry(void *param)
{
    
    osiEvent_t waitevent;
    char *context = NULL;
    MG_osiTracePrintf(LOG_TAG,"wait event OSWaitEntry1 enter\n");
    for(;;)
    {
        MG_osiTracePrintf(LOG_TAG,"wait event OSWaitEntry enter\n");
        
        MG_osiEventWait(MG_osiThreadCurrent(), &waitevent);
       // MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG,"wait event id1 = %d\n",waitevent.id);
        context = (char *)waitevent.param1;
        MG_osiTracePrintf(LOG_TAG,"wait event param1 = 0x%x\n",context);
        MG_osiTracePrintf(LOG_TAG,"wait event param1 = %s\n",context);
        if(context != NULL)
        {
            MG_osiFree(context);
            context = NULL;
        
        }
       #if 1
        switch(waitevent.id)
        {
            case 9:
            MG_osiTracePrintf(LOG_TAG,"wait event id 9\n");
            break;
            case 10:
            MG_osiTracePrintf(LOG_TAG,"wait event id 10\n");
            break;
            case 11:
            MG_osiTracePrintf(LOG_TAG,"wait event id 11\n");
            break;
            case 12:
            MG_osiTracePrintf(LOG_TAG,"wait event id 12\n");
            break;
            case 13:
            MG_osiTracePrintf(LOG_TAG,"wait event id 13\n");
            break;
            case 14:
            MG_osiTracePrintf(LOG_TAG,"timer event id 14\n");
            break;
            default:
            MG_osiTracePrintf(LOG_TAG,"******default*******\n");
            break;

        }
        #endif

   }
    MG_osiThreadExit();
}

static void OSSendEntry(void *param)
{
    osiEvent_t send_event;
    send_event.id = 9;
    char *p = NULL;
    MG_osiTracePrintf(LOG_TAG,"send event OSSendEntry enter\n");
    for(;;)
    {
        send_event.id ++;
        if(send_event.id >= 15)
        {
            MG_osiTracePrintf(LOG_TAG,"send event over\n");
            break;

        }
        p = (char *)MG_osiMalloc(100);
        if(!p)
        {
            MG_osiTracePrintf(LOG_TAG,"send event OSSendEntry malloc failed\n");
            MG_osiThreadExit();
        }
        sprintf((char *)p,"send event OSSendEntry event id %ld",send_event.id);
        send_event.param1 = (uint32_t)p;
        MG_osiTracePrintf(LOG_TAG,"send event id = %d,param1 = 0%x\n",send_event.id,send_event.param1);
        if(MG_osiEventSend(threadid_wait, &send_event) == 0)
        {
            MG_osiTracePrintf(LOG_TAG,"send event MG_osiEventSend failed\n");
            MG_osiThreadExit();
        }
        else
        {
            MG_osiTracePrintf(LOG_TAG,"send event MG_osiEventSend success\n");      

        }
        //MG_osiThreadSleep(5000);
    }
    MG_osiThreadExit();

}

static void prvThreadEntry(void *param)
{
    while(1)
    {
        MG_osiTracePrintf(LOG_TAG, "mg os example run");
        MG_osiThreadSleep(3000);
    }
   
   MG_osiThreadExit();
}


void Timeroutcallback(void *param)
{  
    static char n=1;  
    MG_osiTracePrintf(LOG_TAG,"timer out callback enter %d\n",n);
    
    if(ptimer_t != NULL)
    {
         n++;
         MG_osiTracePrintf(LOG_TAG,"timer start again\n");
         MG_osiTimerStart(ptimer_t, 1000);
    }
    if(n==5)
    {   
        MG_osiTracePrintf(LOG_TAG,"timer delete\n");
        
        MG_osiTimerDelete(ptimer_t);
    }
    
   

}

void TimerEntry(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg os timer enter");
    ptimer_t = MG_osiTimerCreate(MG_osiThreadCurrent(), Timeroutcallback, NULL);
    if(NULL == ptimer_t)
    {
        MG_osiTracePrintf(LOG_TAG,"timer create failed\n");
        MG_osiThreadExit();

    }
    MG_osiThreadSleep(1000);
    if(MG_osiTimerStart(ptimer_t, 3000) == 0)
    {
        MG_osiTracePrintf(LOG_TAG,"timer start failed\n");
        MG_osiThreadExit();

    }
    else
    {
         MG_osiTracePrintf(LOG_TAG,"timer start success\n");  

    }
    MG_osiThreadSleep(1000);
    MG_osiTimerStop(ptimer_t);
    MG_osiTracePrintf(LOG_TAG,"timer stop\n");  
    MG_osiThreadSleep(3000);
    MG_osiTracePrintf(LOG_TAG,"timer start\n");  
    MG_osiTimerStart(ptimer_t, 1000);
    MG_osiThreadSleep(2000);
    osiEvent_t event;
    while(1)
    {
        MG_osiEventWait(MG_osiThreadCurrent(), &event);
        MG_osiTracePrintf(LOG_TAG,"timer task run\n");  
        //MG_osiThreadSleep(10000);
    }
    MG_osiThreadExit();
  
}

void timer_demo(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg os timer example enter");
    threadid_timer = MG_osiThreadCreateE("ostimer", TimerEntry,NULL,OSI_PRIORITY_NORMAL,1024,1);
}

void entry_demo(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg os entry example enter");
    threadid_wait = MG_osiThreadCreate("oswait", OSWaitEntry,NULL,OSI_PRIORITY_NORMAL,1024);
    threadid_send = MG_osiThreadCreate("ossend", OSSendEntry,NULL,OSI_PRIORITY_NORMAL,1024);
}

void recvformqueEntry(void *param)
{
    msg_t *recvmsg;
    MG_osiTracePrintf(LOG_TAG,"os_example -- recvformqueEntry enter %x  %x\n",recvmsg,&recvmsg);
    while(1)
    {
       if(MG_osiMessageQueueGet(queue,(void *)&recvmsg) == 0)
       {
            MG_osiTracePrintf(LOG_TAG,"os_example -- MG_osiMessageQueueTryGet failed or timeout\n");

       }
       MG_osiTracePrintf(LOG_TAG,"os_example recvmsg->id = %d,recvmsg->data = %s  addr = %x\n",recvmsg->messageid,recvmsg->data,recvmsg);
       MG_osiFree(recvmsg);
    }

    #if 0
    for(;;)
   {
        MG_osiEventWait(MG_osiThreadCurrent(), &event);

    }
    #endif
    MG_osiThreadExit();
  
}

void sendtoqueEntry(void *param)
{

    
    msg_t *sendmsg;
    
    int count;
    MG_osiTracePrintf(LOG_TAG,"os_example -- sendtoqueEntry enter \n");
    for(count = 0;count < 5;count++)
    {
        sendmsg =MG_osiMalloc(sizeof(msg_t));
        sendmsg->messageid = count;  
        sprintf((char *)sendmsg->data,"hello %d",count);
        MG_osiTracePrintf(LOG_TAG,"os_example -- sendmsg->data = %s,sendmsg->messageid = %d  addr = %x\n",sendmsg->data,sendmsg->messageid,sendmsg);
        if(queue == NULL)
        {
             MG_osiTracePrintf(LOG_TAG,"os_example -- queue == NULL\n");
             break;

        }
        if(MG_osiMessageQueuePut(queue,(void *)&sendmsg) == 0)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- MessageQueuePut failed\n"); 
            MG_osiThreadExit();
        }
        else
       {
            MG_osiTracePrintf(LOG_TAG,"os_example -- MessageQueuePut success\n"); 
        }
        MG_osiThreadSleep(500);

    }
    MG_osiThreadExit();

}

void messagequeue_demo(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg messagequeue example enter");
    
    queue = MG_osiMessageQueueCreate(10,50);
    if(NULL == queue)
    {
        MG_osiTracePrintf(LOG_TAG,"os_example -- MG_osiMessageQueueCreate faield\n");
        return;

    }
    else
    {
        MG_osiTracePrintf(LOG_TAG,"os_example -- MG_osiMessageQueueCreate success\n");
    }
    MG_osiThreadCreate("sendtoqueue", sendtoqueEntry,NULL,OSI_PRIORITY_NORMAL,1024);
    MG_osiThreadSleep(5000);
    MG_osiThreadCreate("recvfromqueue", recvformqueEntry, NULL, OSI_PRIORITY_NORMAL, 1024);

}

void sematest1(void *param)
{
    MG_osiTracePrintf(LOG_TAG,"os_example -- sematest1 enter\n");
   
    int32_t count = 0;
    MG_osiTracePrintf(LOG_TAG,"os_example -- sem wait now\n");
    if(MG_osiSemaphoreTryAcquire(sema,100)==0)
    {
        MG_osiTracePrintf(LOG_TAG,"os_example -- sem wait timeout\n");  
    }
    while(count < 10)
    {
        MG_osiSemaphoreAcquire(sema);
        MG_osiTracePrintf(LOG_TAG,"os_example -- sematest1\n");
        count++;
    }
    MG_osiThreadExit();
   
}


void sematest2(void *param)
{
     MG_osiTracePrintf(LOG_TAG,"os_example -- sematest2 enter\n");
     int32_t num = 0;
     MG_osiTracePrintf(LOG_TAG,"os_example -- sem release now\n");
     while(num < 10)
     {
        MG_osiThreadSleep(1000);
        MG_osiSemaphoreRelease(sema);
        MG_osiTracePrintf(LOG_TAG,"os_example -- sematest2\n");
        num++;
     }
     MG_osiThreadExit();
     
    

}

void sem_demo(void)
{

    MG_osiTracePrintf(LOG_TAG,"os_example -- sem_demo enter\n");    
    sema = MG_osiSemaphoreCreate(1,0);
    MG_osiThreadCreate("semawait", sematest1,NULL,OSI_PRIORITY_NORMAL,1024);
    MG_osiThreadSleep(3000);
    MG_osiThreadCreate("semapost", sematest2,NULL,OSI_PRIORITY_NORMAL,1024);
    
}

void threadfunc(void *param)
{
    MG_osiMutexLock(mutex);
    int32_t num ;
    osiThread_t *pthread = MG_osiThreadCurrent();
    for(num = 0;num < 3; num++)
    {
        MG_osiThreadSleep(2000);
        MG_osiTracePrintf(LOG_TAG,"os--example mutex --pthread = 0x%x,num = %d\n",pthread,num);
    }
    MG_osiMutexUnlock(mutex);
    MG_osiThreadExit();
   
}

void mutex_demo(void)
{
    MG_osiTracePrintf(LOG_TAG,"os_example -- mutex_demo enter\n");
    mutex = MG_osiMutexCreate();
    int count;
    char threadname[100] = {0x00};
    for(count = 0;count < thread_num;count++)
    {
        memset(threadname,0x00,sizeof(threadname));
        sprintf(threadname,"thread-%d",count);
        MG_osiThreadCreate(threadname,threadfunc,NULL,OSI_PRIORITY_NORMAL, 1024);
        
    }
}

void ReadCallback(void *param,unsigned event)
{
    char buf[256];
   
    for (;;)
    {
        int re=0;
        re =MG_osiPipeWaitReadAvail(pipe,100);
        if(re)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe have data\n");
        }

        re =MG_osiPipeReadAvail(pipe);
        if(re>0)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe have data %d\n",re);
        }
        int bytes = MG_osiPipeRead(pipe, buf, 255);
        if (bytes <= 0)
            break;

        buf[bytes] = '\0';
        MG_osiTracePrintf(LOG_TAG,"os_example -- pipe read (%d): %s", bytes, buf);
    }    



}

void WriterCallback(void *param,unsigned event)
{
     MG_osiTracePrintf(LOG_TAG,"os_example -- pipe WriterCallback\n");
  


}

void PipeWriteEntry()
{
    int count = 0;
    int write_len = 0;
    char content[100] = {0x00};
    MG_osiPipeSetReaderCallback(pipe, OSI_PIPE_EVENT_RX_ARRIVED,ReadCallback,NULL);
    MG_osiPipeSetWriterCallback(pipe, OSI_PIPE_EVENT_TX_COMPLETE,WriterCallback,NULL);
    while(1)
   {
       int re =0;

        sprintf(content,"pipe send string %d",count++);
        MG_osiTracePrintf(LOG_TAG,"os_example -- pipe strlen(content) = %d\n",strlen(content));
        re =MG_osiPipeWaitWriteAvail(pipe,100);
        if(re)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe Avail write data\n");
        }
        write_len = MG_osiPipeWriteAll(pipe,content,strlen(content),OSI_WAIT_FOREVER);
        if(write_len < strlen(content))
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe space full\n");
        }
        if(write_len < 0)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe space has been full write_len = %d\n",write_len);
            break;
        }
        MG_osiThreadSleep(1000);
        re =MG_osiPipeWriteAvail(pipe);
        if(re >0)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe Avail write data %d\n",re);
        }
        MG_osiPipeWrite(pipe,content,strlen(content));
        MG_osiThreadSleep(1000);

        re =MG_osiPipeWriteAvail(pipe);
        if(re ==1024)
        {
            MG_osiTracePrintf(LOG_TAG,"os_example -- pipe delete %d\n",re);
            MG_osiPipeDelete(pipe);
            MG_osiThreadExit();
        }
   }

MG_osiThreadExit();


}

void TimeStamp_demo(void)
{
    MG_osiTracePrintf(LOG_TAG,"os_example -- TimeStamp_demo enter\n");
    //2020-10-15 10:58:50 对应时间戳 1602730730
    uint64_t timestamp_utc = 1602730730;
    MG_osiTracePrintf(LOG_TAG,"timestamp of 2020-10-15 10:58:50 in second is 1602730730\n");
    char time_string[20] = {0};
    MG_TimeStampConverter(timestamp_utc, time_string);
    MG_osiTracePrintf(LOG_TAG,"UTC %s\n", time_string);
    bzero(time_string, sizeof(time_string));
    MG_TimeStampConverter(timestamp_utc+28800, time_string);
    MG_osiTracePrintf(LOG_TAG,"BeiJing %s\n", time_string);
}

void pipe_demo(void)
{
    MG_osiTracePrintf(LOG_TAG,"os_example -- pipe_demo enter\n");
    pipe = MG_osiPipeCreate(1024);
    MG_osiThreadCreate("pipe-write", PipeWriteEntry, NULL, OSI_PRIORITY_NORMAL, 1024);

}


osiThread_t *threadid_1 = NULL;

void task1Entry()
{
    MG_osiThreadSleep(1000);
    
    uint32_t priority;
    threadid_1 =MG_osiThreadCurrent();
    priority=MG_osiThreadPriority(threadid_1);
    MG_osiTracePrintf(LOG_TAG,"os_example -- Priority %d\n",priority);
    MG_osiThreadSetPriority(threadid_1,10);
    priority=MG_osiThreadPriority(threadid_1);
    MG_osiTracePrintf(LOG_TAG,"os_example -- SetPriority %d\n",priority);

    while(1)
    {
        MG_osiTracePrintf(LOG_TAG,"os_example -- Priority %d\n",priority);
        MG_osiThreadSleep(1000);
    }
    MG_osiThreadExit();
}
void task2Entry()
{
    MG_osiThreadSleep(5000);
    MG_osiThreadSuspend(threadid_1);
    MG_osiTracePrintf(LOG_TAG,"os_example -- threadid_1 Suspend\n");

    MG_osiThreadSleep(5000);
    MG_osiThreadResume(threadid_1);
    MG_osiTracePrintf(LOG_TAG,"os_example -- threadid_1 Resume\n");
    MG_osiThreadSleep(5000);
    MG_osiTracePrintf(LOG_TAG,"os_example -- OS STOP \n");
    MG_osiThreadSleep(1000);
    MG_osiPanic();
    while(1)
    {
        MG_osiTracePrintf(LOG_TAG,"os_example -- task2Entry\n");
        MG_osiThreadSleep(1000);
    }
    MG_osiThreadExit();
   
}
void thread_demo(void)
{
    MG_osiTracePrintf(LOG_TAG,"os_example -- thread_demo enter\n");

    MG_osiThreadCreate("task1", task1Entry, NULL, OSI_PRIORITY_NORMAL, 1024);
    MG_osiThreadCreate("task2", task2Entry, NULL, OSI_PRIORITY_NORMAL, 1024);

}

int appimg_enter(void *param)
{
    MG_osiThreadSleep(3000);
    MG_osiTracePrintf(LOG_TAG, "mg os example enter");
    //MG_osiThreadCreate("mgOsExample", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
   
    //timer_demo();
    //entry_demo();
    //messagequeue_demo();
    //sem_demo();
    //mutex_demo();
    //pipe_demo();
    //thread_demo();
    TimeStamp_demo();
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg os example exit");
}
