
#include "ty_os_adpt.h"
#include "softap_api.h"


typedef struct 
{
    UINT32 handle;
    UINT32 msg_size;
}QueWrapper;

#if MEMLEAK_CHECK
//do nothing
#else
void * lpa_malloc( size_t size)
{
    return xy_malloc(size);
}

void lpa_free(void * ptr)
{
    if(ptr)
    xy_free(ptr);
}


void *lpa_calloc(size_t nitems, size_t size)
{
    void *temp = xy_malloc(nitems*size);
    if(!temp)
        return temp;
    memset(temp,0,nitems*size);
    return temp;
}

void* lpa_realloc(void *ptr, size_t size)
{
    void* newBuff = xy_malloc(size);
    memcpy(newBuff , ptr, size);
    lpa_free(ptr);
    return newBuff;
}
#endif

void * ty_create_mutex()
{
    //LOG("%s in",__FUNCTION__);
    UINT32 *handle =  (UINT32 *)lpa_malloc(sizeof(UINT32));
    if(!handle)
        return handle;
    xy_Mutex_Create(handle);
    //LOG("%s %p",__FUNCTION__,handle);
    return (void *)handle;
}

void ty_mutex_lock(void *mutex,uint32_t timeout)
{
    if(!mutex)
        return ;
    xy_Mutex_Lock((UINT32*)mutex,timeout);
}

void ty_mutex_unlock(void *mutex)
{
    if(!mutex)
            return ;
    xy_Mutex_Unlock((UINT32*)mutex);
}

void ty_mutex_destroy(void *mutex)
{
    //LOG("%s in",__FUNCTION__);
    if(!mutex)
            return ;
    xy_Mutex_Destroy((UINT32*)mutex);
    lpa_free(mutex);
    //LOG("%s in",__FUNCTION__);
    
}

void *ty_create_msg_Que(uint32_t msg_count, uint32_t msg_size)
{
    QueWrapper *wrapper = lpa_malloc(sizeof(QueWrapper));
    wrapper->handle = -1;
    wrapper->msg_size = msg_size;
    xy_QueueCreate(&wrapper->handle, msg_count, msg_size);
    xy_printf("%s %p",__FUNCTION__,wrapper->handle);
    return (void*)wrapper;
}

void ty_delete_msg_que(void *mq)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
    if(wrapper)
    {
        xy_QueueClear((UINT32)wrapper->handle);
        xy_QueueDelete(&wrapper->handle);
        lpa_free(wrapper);
        wrapper = NULL;
    }
}

bool ty_put_msg_que(void *mq, const void *msg, uint32_t timeout)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
    int ret;
    void *temp = lpa_malloc(wrapper->msg_size);
    if(temp == NULL)
        return false;
    memcpy(temp,msg,wrapper->msg_size);
    ret = xy_QueueWrite((UINT32)wrapper->handle,(void*)&temp, sizeof(void *), K_FOREVER);
    //xy_printf("%s event=%d sz=%d  ret=%d %p %p\r\n",__FUNCTION__,t->event,t->size,ret,t,t->data);
    return true;
}
bool ty_get_msg_que(void *mq,void *msg, uint32_t timeout)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
    UINT32 queuelen = -1;
    void *temp = NULL;
    int ret ;
    
    ret = xy_QueueRead((UINT32)wrapper->handle, (void*)&temp, &queuelen,timeout);
    if(ret == 0 && temp != NULL)
    {
        //xy_printf("%s event=%d %d %p %p\r\n",__FUNCTION__,t->event,t->size,t,t->data);
        if(msg != NULL)
            memcpy(msg,temp,wrapper->msg_size);
        lpa_free(temp);
        temp = NULL;
        return true;
    }
    return false;
}
uint32_t ty_get_tick_ms()
{
    return xy_GetTickCount ();
}

void ty_sleep(int ms)
{
    xy_sleep (ms);
}

void LOG(const char *format, ... )
{
    va_list argp;
    char str[256] = {0};
    int ret;
    
    va_start(argp, format );
    ret = vsnprintf( str, sizeof(str), format, argp );
    va_end( argp );

    /*
    if( ret >= 0 && ret < sizeof(str) - 1 )
    {
        str[ret]     = '\n';
        str[ret + 1] = '\0';
    }
    */
    //xy_printf("%s",str);
	//直接输出到AT串口终端
    ty_cloud_ext_notify(str);
}

void ty_printf_hexbuf(char *message,int len)
{
    int i;
    for(i = 0;i < strlen(message);i+=64)
    {
        char debug[64+3] = {0};
        memcpy(debug,message+i,(len-i >= 64) ? 64:(len-i));
        LOG("%i,[%s]\r\n",i,debug);
    }
}


int ty_task_create(ty_task_func_t task_entry,char * name,int StackSize,void *pvParameters,int Priority,void * TCB_hanndler)
{
    xy_TaskCreate ( task_entry,  name,  StackSize,pvParameters,  Priority, TCB_hanndler);
}

void ty_task_delete( void *TCB_hanndler )
{
    xy_TaskDelete (TCB_hanndler);
}

uint32_t ty_semaphore_create(uint32_t *pSemaphoreId)
{
    return xy_Semaphore_Create (pSemaphoreId);
}

uint32_t ty_semaphore_take(uint32_t *pSemaphoreId,uint32_t xTicksToWait_ms)
{
    return xy_Semaphore_Take (pSemaphoreId,  xTicksToWait_ms);
}

uint32_t ty_semaphore_give(uint32_t *pSemaphoreId)
{
    return xy_Semaphore_Give (pSemaphoreId);
}

uint32_t ty_semaphore_destroy(uint32_t *pSemaphoreId)
{
    return xy_Semaphore_Destroy (pSemaphoreId);
}

uint32_t ty_timer_create(char* name, uint32_t *pTimerId,uint32_t ulTimerTick_ms,const uint32_t uxAutoReload, ty_timer_func_t callback_func )
{
   return xy_Timer_Create ( name, pTimerId,ulTimerTick_ms,   uxAutoReload,  callback_func);
}

uint32_t ty_timer_start(uint32_t *pTimerId)
{
    return xy_Timer_Start(pTimerId);
}

uint32_t ty_timer_stop(uint32_t *pTimerId)
{
    return xy_Timer_Stop(pTimerId);
}

uint32_t ty_timer_reset(uint32_t *pTimerId)
{
    return xy_Timer_Reset(pTimerId);
}

uint32_t ty_timer_delete(uint32_t *pTimerId)
{
    return xy_Timer_Delete (pTimerId);
}

void ty_work_lock(int lock_dsp)
{
    xy_work_lock(lock_dsp);   
}

void ty_work_unlock()
{
    xy_work_unlock();
}

int ty_wait_tcpip_ok(int timeout)
{
    return  xy_wait_tcpip_ok( timeout);
}

int ty_rtc_get_sec (int zone_sec)
{
    return  xy_rtc_get_sec ( zone_sec);
}

void ty_assert_proc(int value,char* filename, int lineno)
{
    xy_assert_proc(value, filename, lineno);
}


void ty_get_timestamp(char* timeBuff)
{
    struct xy_wall_clock julian_time={0};
	xy_rtc_get_UT_offset(&julian_time,0-8*60*60);
    sprintf(timeBuff,"%04d%02d%02d%02d%02d%02d",julian_time.tm_year,julian_time.tm_mon,julian_time.tm_wday,julian_time.tm_hour,julian_time.tm_min,julian_time.tm_sec);
}

static inline void*  nb12_plat_malloc(uint32_t ulSize)
{
    return xy_malloc (ulSize);
}

static inline void nb12_plat_xy_free(void* pMem)
{
    xy_free (pMem);
}

void ty_plat_os_init()
{
    #if MEMLEAK_CHECK
    memleak_set_plat_malloc_free (nb12_plat_malloc, nb12_plat_xy_free);
    dbg_init(1000);
    dbg_catch_sigsegv();
    #endif
}


