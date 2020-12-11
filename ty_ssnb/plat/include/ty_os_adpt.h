#ifndef __TY_OS_ADPT_H__
#define __TY_OS_ADPT_H__

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"


#define TY_WAIT_FOREVER                            0xFFFFFFFF
#define TY_TIMER_AUTO_RELOAD 1                      /**< Periodic software timer */
#define TY_TIMER_NO_AUTO_RELOAD 2                   /**< One-off software timer, but not self-delete */


void LOG(const char *format, ... );
void ty_printf_hexbuf(char *message,int len);

#if MEMLEAK_CHECK

#include "memleak.h"

#else

void * lpa_malloc( size_t size);

void lpa_free(void *);

void *lpa_calloc(size_t nitems, size_t size);


void* lpa_realloc(void *__ptr, size_t __size);

#endif

void *ty_create_mutex();

void ty_mutex_lock(void*mutex, uint32_t timeout);

void ty_mutex_unlock(void*mutex);

void ty_mutex_destroy(void *mutex);


void *ty_create_msg_Que(uint32_t msg_count, uint32_t msg_size);

void ty_delete_msg_que(void *mq);

bool ty_put_msg_que(void *mq, const void *msg, uint32_t timeout);

bool ty_get_msg_que(void *mq,void *msg, uint32_t timeout);



uint32_t ty_get_tick_ms();

void ty_sleep(int ms);


typedef void (*ty_task_func_t)( void *arg);

int ty_task_create(ty_task_func_t task_entry,char * name,int StackSize,void *pvParameters,int Priority,void * TCB_hanndler);

void ty_task_delete( void *TCB_hanndler );


uint32_t ty_semaphore_create(uint32_t *pSemaphoreId);

uint32_t ty_semaphore_take(uint32_t *pSemaphoreId,uint32_t xTicksToWait_ms);

uint32_t ty_semaphore_give(uint32_t *pSemaphoreId);

uint32_t ty_semaphore_destroy(uint32_t *pSemaphoreId);



typedef void (*ty_timer_func_t)(void* arg);

uint32_t ty_timer_create(char* name, uint32_t *pTimerId,uint32_t ulTimerTick_ms,const uint32_t uxAutoReload, ty_timer_func_t callback_func );

uint32_t ty_timer_start(uint32_t *pTimerId);

uint32_t ty_timer_stop(uint32_t *pTimerId);

uint32_t ty_timer_reset(uint32_t *pTimerId);

uint32_t ty_timer_delete(uint32_t *pTimerId);


void ty_work_lock(int lock_dsp);

void ty_work_unlock();

int ty_wait_tcpip_ok(int timeout);

int ty_pdp_active(int timeout);

int ty_pdp_deactive();



int ty_rtc_get_sec (int zone_sec);

void ty_assert_proc(int value,char* filename, int lineno);

#define ty_assert(a)	{ty_assert_proc(!(a), __FILE__, __LINE__);}

void ty_get_timestamp(char* timeBuff);

void ty_plat_os_init();

#endif
