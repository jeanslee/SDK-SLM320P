/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _MG_OS_API_H_
#define _MG_OS_API_H_
#include "osi_api.h"
#include "osi_pipe.h"
//by hes
//This function is create a pipe.
osiPipe_t *MG_osiPipeCreate(unsigned size);
//	This function is delete a pipe.
void MG_osiPipeDelete(osiPipe_t *pipe);
//	read data from pipe
int MG_osiPipeRead(osiPipe_t *pipe, void *buf, unsigned size);
//data available in pipe for read
int MG_osiPipeReadAvail(osiPipe_t *pipe);
//	data available in pipe for write
int MG_osiPipeWriteAvail(osiPipe_t *pipe);
//This function is set reader callback.
void MG_osiPipeSetReaderCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx);
//This function is set writer callback.
void MG_osiPipeSetWriterCallback(osiPipe_t *pipe, unsigned mask, osiPipeEventCallback_t cb, void *ctx);
bool MG_osiPipeWaitReadAvail(osiPipe_t *pipe, unsigned timeout);
bool MG_osiPipeWaitWriteAvail(osiPipe_t *pipe, unsigned timeout);
int MG_osiPipeWrite(osiPipe_t *pipe, const void *buf, unsigned size);
int MG_osiPipeWriteAll(osiPipe_t *pipe, const void *buf, unsigned size, unsigned timeout);
bool MG_osiSemaphoreTryAcquire(osiSemaphore_t *sem, uint32_t timeout);
void MG_osiThreadYield(void);
int64_t MG_osiUpTimeUS(void);
bool MG_osiMessageQueueTryPut(osiMessageQueue_t *mq, const void *msg, uint32_t timeout);
bool MG_osiMutexTryLock(osiMutex_t *mutex, uint32_t timeout);
void MG_osiFree(void *ptr);
void *MG_osiMalloc(size_t size);
osiBootMode_t MG_osiGetBootMode(void);
int64_t MG_osiLocalSecond(void);
int64_t MG_osiLocalTime(void);
void MG_osiSetUpTime(int64_t ms);
uint32_t MG_osiThreadPriority(osiThread_t *thread);
void MG_osiThreadResume(osiThread_t *thread);
bool MG_osiThreadSetPriority(osiThread_t *thread, uint32_t priority);
void MG_osiThreadSuspend(osiThread_t *thread);
void MG_osiSetTimeZoneOffset(int offset);
osiThread_t *MG_osiThreadCreateE(const char *name, osiThreadEntry_t entry, void *argument,
                             uint32_t priority, uint32_t stack_size,uint32_t event_count);
//same as osiThreadCreate
osiThread_t *MG_osiThreadCreate(const char *name, osiThreadEntry_t entry, void *argument,
                             uint32_t priority, uint32_t stack_size);
osiThread_t *MG_osiThreadCurrent(void);
void MG_osiThreadSleep(uint32_t ms);
void MG_osiThreadExit(void);
/*
 * same as osiEventSend
 */
bool MG_osiEventSend(osiThread_t *thread, const osiEvent_t *event);
/*
 * same as osiEventWait
 */
bool MG_osiEventWait(osiThread_t *thread, osiEvent_t *event);
/*
 * same as osiThreadCallback
 */
bool MG_osiThreadCallback(osiThread_t *thread, osiCallback_t cb, void *cb_ctx);
/*
 * same as osiMessageQueueCreate
 */
osiMessageQueue_t *MG_osiMessageQueueCreate(uint32_t msg_count, uint32_t msg_size);
/*
 * same as osiMessageQueueDelete
 */
void MG_osiMessageQueueDelete(osiMessageQueue_t *mq);
/*
 * same as osiMessageQueuePut
 */
bool MG_osiMessageQueuePut(osiMessageQueue_t *mq, const void *msg);
/*
 * same as osiMessageQueueGet
 */
bool MG_osiMessageQueueGet(osiMessageQueue_t *mq, void *msg);
bool MG_osiMessageQueueTryGet(osiMessageQueue_t *mq, void *msg, uint32_t timeout);
/*
 * same as osiSemaphoreCreate
 */
osiSemaphore_t *MG_osiSemaphoreCreate(uint32_t max_count, uint32_t init_count);
/*
 * same as osiSemaphoreDelete
 */
void MG_osiSemaphoreDelete(osiSemaphore_t *semaphore);
/*
 * same as osiSemaphoreAcquire
 */
bool MG_osiSemaphoreAcquire(osiSemaphore_t *semaphore);//modify by zx 20201109
/*
 * same as osiSemaphoreRelease
 */
void MG_osiSemaphoreRelease(osiSemaphore_t *semaphore);
/*
 * same as osiMutexCreate
 */
osiMutex_t *MG_osiMutexCreate(void);
/*
 * same as osiMutexDelete
 */
void MG_osiMutexDelete(osiMutex_t *mutex);
/*
 * same as osiMutexLock
 */
void MG_osiMutexLock(osiMutex_t *mutex);
/*
 * same as osiMutexUnlock
 */
void MG_osiMutexUnlock(osiMutex_t *mutex);
//same as osiUpTime
int64_t MG_osiUpTime(void);
/*same as osiEpochTime*/
int64_t MG_osiEpochTime(void);
int64_t MG_osiEpochSecond(void);
//same as osiSetEpochTime
bool MG_osiSetEpochTime(int64_t ms);
void MG_osiElapsedTimerStart(osiElapsedTimer_t *timer);
uint32_t MG_osiElapsedTime(osiElapsedTimer_t *timer);
OSI_NO_RETURN void MG_osiPanic(void);

//This function is used to reboot
bool MG_Reboot(void);
//This function is used to getting random numbers
int MG_GetRandom();

/**
 * @brief£ºconvert      time stamp to "year-month-day hour:minute:sec"
 * @input: time_stamp   time stamp(UTC)
 * @output: time_string "year-month-day hour:minute:sec". The minimum length is 20
 * @return: time_string
*/
char *MG_TimeStampConverter(uint64_t time_stamp, char *time_string);

#endif
