
#include "ty_os_adpt.h"
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include "opencpu_rtos.h"
//以下字段需要在Makefile中定义
#ifndef SS11_PRODUCT_TYPE
#error "no define macro SS11_PRODUCT_TYPE"
#endif

#ifndef SS11_FIRMWARE_VERSION
#error "no define macro SS11_FIRMWARE_VERSION"
#endif

#ifndef SS11_MQTT_PRODUCT_KEY
#error "no define macro SS11_MQTT_PRODUCT_KEY"
#endif

#ifndef SS11_MQTT_PRODUCT_SECRET
#error "no define macro SS11_MQTT_PRODUCT_SECRET"
#endif

#ifndef SS11_TY_CLOUD_MQTT_SERVER
#error "no define macro SS11_TY_CLOUD_MQTT_SERVER"
#endif


#ifndef SS11_TY_CLOUD_PORT
#error "no define macro SS11_TY_CLOUD_PORT"
#endif

#ifndef SS11_TY_CLOUD_HTTP_DEVREG_URL
#error "no define macro SS11_TY_CLOUD_HTTP_DEVREG_URL"
#endif


//产品名称
const char *PRODUCT_TYPE = SS11_PRODUCT_TYPE;
//固件版本(FOTA升级时需要用于匹配校验)
const char *FIRMWARE_VERSION = SS11_FIRMWARE_VERSION;
//(用于三元组动态注册和认证)
const char *MQTT_PRODUCT_KEY = SS11_MQTT_PRODUCT_KEY;
//(用于三元组动态注册)
const char *MQTT_PRODUCT_SECRET = SS11_MQTT_PRODUCT_SECRET;
//MQTT平台地址
const char *TY_CLOUD_MQTT_SERVER = SS11_TY_CLOUD_MQTT_SERVER;
//MQTT端口号
int TY_CLOUD_PORT = SS11_TY_CLOUD_PORT;
//设备动态注册地址
const char *TY_CLOUD_HTTP_DEVREG_URL = SS11_TY_CLOUD_HTTP_DEVREG_URL;




#if MEMLEAK_CHECK
//do nothing
#else
void * lpa_malloc( size_t size)
{
    return opencpu_malloc(size);
}

void lpa_free(void * ptr)
{
    if(ptr)
    opencpu_free(ptr);
}


void *lpa_calloc(size_t nitems, size_t size)
{
    void *temp = opencpu_malloc(nitems*size);
    if(!temp)
        return temp;
    memset(temp,0,nitems*size);
    return temp;
}

void* lpa_realloc(void *ptr, size_t size)
{
    void* newBuff = opencpu_malloc(size);
    memcpy(newBuff , ptr, size);
    lpa_free(ptr);
    return newBuff;
}
#endif

void * ty_create_mutex()
{
    HANDLE  *handle =  (HANDLE *)lpa_malloc(sizeof(HANDLE));
    if(!handle)
        return NULL;
    
    *handle = opencpu_mutex_create();
    return (void *)handle;
}


void ty_mutex_lock(void *mutex,uint32_t timeout)
{
	if(!mutex)
        return ;
    
	opencpu_mutex_take(*(HANDLE*)mutex); 
       ty_sleep(timeout);
}

void ty_mutex_unlock(void *mutex)
{
	if(!mutex)
            return ;
    
	opencpu_mutex_give(*(HANDLE*)mutex);
}


void ty_mutex_destroy(void *mutex)
{
	if(!mutex)
            return ;

    opencpu_mutex_destory(*((HANDLE*)mutex));
    lpa_free(mutex);
    mutex = NULL;
}





 void *ty_create_msg_Que(uint32_t msg_count, uint32_t msg_size)
{

    Queue *cmd_queue = Queue_create(msg_count, msg_size, NULL);

    return (void*)cmd_queue;

}

void ty_delete_msg_que(void *mq)
{

    Queue *cmd_queue_tmp = (Queue*)mq;
    if(cmd_queue_tmp)
    {
        Queue_Free(cmd_queue_tmp);
        cmd_queue_tmp = NULL;
    }
}
bool ty_put_msg_que(void *mq, const void *msg, uint32_t timeout)
{
    Queue *cmd_queue_tmp = (Queue*)mq;
    if(cmd_queue_tmp == NULL)
        {
            opencpu_printf("put msg que error!\r\n");
            return false;
            
        }
    void *temp = lpa_malloc(cmd_queue_tmp->ItemSize);
    if(temp == NULL)
        return false;
    
    memcpy(temp, msg, cmd_queue_tmp->ItemSize);



    Queue_AddToHead(cmd_queue_tmp, temp);

    return true;

}
bool ty_get_msg_que(void *mq,void *msg, uint32_t timeout)
{

    Queue *cmd_queue_tmp = (Queue*)mq;

    if(cmd_queue_tmp == NULL && mq == NULL)
        {
            opencpu_printf("get msg que error!\r\n");
            return false;
        }
 
    
    void *temp = Queue_GetFromTail(cmd_queue_tmp);

    
    if(temp != NULL)
    {
        if(msg != NULL)
    
            memcpy(msg, temp, cmd_queue_tmp->ItemSize);

            lpa_free(temp);
            temp = NULL;
      
       
        return true;
    }
//    else {
//            printf("temp = NULL!\r\n");
//            msg = NULL;
//    }
    ty_sleep(100);
    return false;

}



uint32_t ty_get_tick_ms()
{
    UINT32 tick=0;
    tick = opencpu_TimGetUpTime ();

    return opencpu_Tick2Msec (tick);
}


void ty_sleep(int ms)
{

    opencpu_sleep(ms);
    
} 

void LOG(const char *format, ... )
{
    va_list argp;
    char str[256] = {0};
    int ret = 0;
    
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
    opencpu_printf ("%s",str);
	//直接输出到AT串口终端

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
    HANDLE* ret =(HANDLE*)TCB_hanndler;
		if(ret == NULL)
			{
				opencpu_printf("create failed\r\n");
				return -1;
				}
    *ret = opencpu_create_task  (task_entry, pvParameters, StackSize, Priority, name)  ; 
    return 0;
   
}

void ty_task_delete( void *TCB_hanndler )
{
     opencpu_del_task(*((HANDLE*)TCB_hanndler));
}

uint32_t ty_semaphore_create(uint32_t *pSemaphoreId)
{
     return -1;
}

uint32_t ty_semaphore_take(uint32_t *pSemaphoreId,uint32_t xTicksToWait_ms)
{
     return -1;
}

uint32_t ty_semaphore_give(uint32_t *pSemaphoreId)
{
     return -1;
}

uint32_t ty_semaphore_destroy(uint32_t *pSemaphoreId)
{
     return -1;
}

uint32_t ty_timer_create(char* name, uint32_t *pTimerId,uint32_t ulTimerTick_ms,const uint32_t uxAutoReload, ty_timer_func_t callback_func )
{
      ST_TIMER_CONFIG *pTimer =  (ST_TIMER_CONFIG*)lpa_malloc(sizeof(ST_TIMER_CONFIG));

       if(pTimer == NULL)
        {
            LOG(" TEST ERROR\r\n");
        }
       
        pTimer->ms = ulTimerTick_ms;
        pTimer->callback = callback_func;
        pTimer->param = NULL;

        *pTimerId = (uint32_t )pTimer;
       

        return 0;
}





uint32_t ty_timer_start(uint32_t *pTimerId)
{
    if(pTimerId == NULL)
        {
            LOG("ty timer start error!\r\n");
        }
    opencpu_start_timer(*pTimerId); 
    return 0;
    
}

uint32_t ty_timer_stop(uint32_t *pTimerId)
{

    opencpu_stop_timer(*pTimerId);
    
    return 0;
}

uint32_t ty_timer_reset(uint32_t *pTimerId)
{
     return -1;
}

uint32_t ty_timer_delete(uint32_t *pTimerId)
{

   lpa_free(*pTimerId);
   *pTimerId == NULL;

    return 0;
}

void ty_work_lock(int lock_dsp)
{
   
}

void ty_work_unlock()
{
 
}

int ty_wait_tcpip_ok(int timeout)
{
    return opencpu_cgact();
}

int ty_rtc_get_sec (int zone_sec)
{   
    UINT32 pDateTime = 0;
    if(opencpu_rtc_get_int_time(&pDateTime)  != 1)
        opencpu_printf("get datetime failed!\n");
        
    return pDateTime;
    
}

void ty_assert_proc(int value,char* filename, int lineno)
{
    if(!value)
    {
        opencpu_printf("ASSERT %s %d \n",filename,lineno);
        assert(value);
    }
}


void ty_get_timestamp(char* timeBuff)
{

    ST_TIME st;
    char str_tmp[50];
    memset(str_tmp, 0, 50);
    opencpu_getsystemtime(&st);
    opencpu_printf("TIME:%04d/%02d/%02d,%02d:%02d:%02d\n",\
        st.uYear, st.uMonth, st.uDay, st.uHour, st.uMinute, st.uSecond);

    opencpu_systemtime_to_strtime(&st, str_tmp);
//    LOG("str_tmp:%s\n", str_tmp);
    strncpy(timeBuff, str_tmp, strlen(str_tmp));

   
}

static inline void*  nb12_plat_malloc(uint32_t ulSize)
{
    return opencpu_malloc (ulSize);
}

static inline void nb12_plat_xy_free(void* pMem)
{
    opencpu_free (pMem);
}

static void  psm_enter_cb(void)
{
    opencpu_printf("%s\r\n",__FUNCTION__);
    save_user_nv_flash ();
}

static void  psm_quit_cb(void)
{
    opencpu_printf("%s\r\n",__FUNCTION__);
    read_user_nv_flash ();
}

void ty_plat_os_init()
{
    #if MEMLEAK_CHECK
    memleak_set_plat_malloc_free (nb12_plat_malloc, nb12_plat_xy_free);
    dbg_init(1000);
    dbg_catch_sigsegv();
    #endif
    //SS11没有数据读取钩子函数，需要开机时主动读取FLASH数据
    read_user_nv_flash ();
    //添加PSM掉电回调接口，用于数据保存与恢复
    opencpu_register_psmtable_callback (psm_enter_cb , psm_quit_cb);
}


