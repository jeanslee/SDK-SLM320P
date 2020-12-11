
#include "ty_os_adpt.h"
#include "mg_os_api.h"

#include "osi_log.h"
#include "osi_mem.h"
#include "mg_date_api.h"
#include "cfw.h"
#include "cJSON.h"
#include "bt_types.h"
#include "assert.h"

//#include "os_queue.h"
//�����ֶ���Ҫ��Makefile�ж���
 #ifndef PRODUCT_TYPE
 #error "no define macro SS11_PRODUCT_TYPE"
 #endif

 #ifndef FIRMWARE_VERSION
 #error "no define macro SS11_FIRMWARE_VERSION"
 #endif

 #ifndef MQTT_PRODUCT_KEY
 #error "no define macro SS11_MQTT_PRODUCT_KEY"
 #endif

 #ifndef MQTT_PRODUCT_SECRET
 #error "no define macro SS11_MQTT_PRODUCT_SECRET"
 #endif

 #ifndef TY_CLOUD_MQTT_SERVER
 #error "no define macro SS11_TY_CLOUD_MQTT_SERVER"
 #endif


 #ifndef TY_CLOUD_PORT
 #error "no define macro SS11_TY_CLOUD_PORT"
 #endif

 #ifndef TY_CLOUD_HTTP_DEVREG_URL
 #error "no define macro SS11_TY_CLOUD_HTTP_DEVREG_URL"
 #endif


//��Ʒ����
//const char *PRODUCT_TYPE = SS11_PRODUCT_TYPE;
//�̼��汾(FOTA����ʱ��Ҫ����ƥ��У��)
//const char *FIRMWARE_VERSION = SS11_FIRMWARE_VERSION;
//(������Ԫ�鶯̬ע�����֤)
//const char *MQTT_PRODUCT_KEY = SS11_MQTT_PRODUCT_KEY;
//(������Ԫ�鶯̬ע��)
//const char *MQTT_PRODUCT_SECRET = SS11_MQTT_PRODUCT_SECRET;
//MQTTƽ̨��ַ
//const char *TY_CLOUD_MQTT_SERVER = SS11_TY_CLOUD_MQTT_SERVER;
//MQTT�˿ں�
//int TY_CLOUD_PORT = SS11_TY_CLOUD_PORT;
//�豸��̬ע���ַ
//const char *TY_CLOUD_HTTP_DEVREG_URL = SS11_TY_CLOUD_HTTP_DEVREG_URL;
#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

typedef struct 
{
    UINT32 *handle;
    UINT32 msg_size;
}QueWrapper;

typedef struct msg{
    uint32_t messageid;
    void* data;
}msg_t;

#if MEMLEAK_CHECK

//do nothing
#else

void * lpa_malloc( size_t size)
{
	 return osiMalloc(size);
}

void lpa_free(void * ptr)
{
    if(ptr)
    osiFree(ptr);
}

void *lpa_calloc(size_t nitems, size_t size)
{
	return osiCalloc(nitems, size);
}

void* lpa_realloc(void *ptr, size_t size)
{   
      if(ptr)
	return osiRealloc(ptr, size);
}
#endif

void * ty_create_mutex()
{
	osiMutex_t *mutex_t =  MG_osiMutexCreate();
    if(!mutex_t)
        return NULL;   
	return (void *)mutex_t;
}

void ty_mutex_lock(void *mutex,uint32_t timeout)
{
	if(!mutex)
        return ;
     MG_osiMutexTryLock((osiMutex_t*)mutex, timeout);

}

void ty_mutex_unlock(void *mutex)
{
	if(!mutex)
		return ;
	MG_osiMutexUnlock((osiMutex_t*)mutex);
}

void ty_mutex_destroy(void *mutex)
{
	if(!mutex)
            return ;
	MG_osiMutexDelete((osiMutex_t *)mutex);
    mutex = NULL;
}

#if 1
 void *ty_create_msg_Que(uint32_t msg_count, uint32_t msg_size)
{
    QueWrapper *wrapper = lpa_malloc(sizeof(QueWrapper));
    if(wrapper == NULL)
        return NULL;
    wrapper->msg_size = msg_size;
    wrapper->handle = MG_osiMessageQueueCreate(msg_count, sizeof(msg_t*));
#ifdef ADPT_DEBUG 
    LOG("%s %x : msg_size = %d",__FUNCTION__,wrapper->handle, wrapper->msg_size);
#endif
    return (void*)wrapper;
}

void ty_delete_msg_que(void *mq)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
#ifdef ADPT_DEBUG     
    LOG("%s %x : msg_size = %d",__FUNCTION__,wrapper->handle, wrapper->msg_size);
#endif
    if(wrapper)
    {
        MG_osiMessageQueueDelete(wrapper->handle);
        lpa_free(wrapper);
        wrapper = NULL;
    }
}

bool ty_put_msg_que(void *mq, const void *msg, uint32_t timeout)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
    int ret;
    if(wrapper == NULL)
        return false;
    msg_t *temp = lpa_malloc(sizeof(msg_t));
    temp->data = lpa_malloc(wrapper->msg_size);
    if(temp == NULL || temp->data == NULL)
        return false;
    memcpy(temp->data, msg, wrapper->msg_size);
#ifdef ADPT_DEBUG     
    //LOG("%s %x : msg_size = %d",__FUNCTION__,wrapper->handle, wrapper->msg_size);
#endif
    ret = MG_osiMessageQueueTryPut((osiMessageQueue_t*)wrapper->handle, (void*)&temp, timeout);
//     ret = MG_osiMessageQueuePut((osiMessageQueue_t*)wrapper->handle, (void*)&temp);
    return ret;
}

bool ty_get_msg_que(void *mq,void *msg, uint32_t timeout)
{
    QueWrapper *wrapper = (QueWrapper *)mq;
    msg_t *temp = NULL;
    if(wrapper == NULL)
        return false;
    int ret ;
#ifdef ADPT_DEBUG     
  //  LOG("%s %x : msg_size = %d",__FUNCTION__,wrapper->handle, wrapper->msg_size);
#endif
    ret = MG_osiMessageQueueTryGet((osiMessageQueue_t*)wrapper->handle, (void*)&temp, timeout);
// ret = MG_osiMessageQueueGet((osiMessageQueue_t*)wrapper->handle, (void*)&temp);

    if(ret == 1 && temp != NULL)
    {
//        LOG("%s temp: %s", __func__, temp->data);
        if(msg != NULL)
            memcpy(msg, temp->data, wrapper->msg_size);
        lpa_free(temp->data);
        temp->data = NULL;
        lpa_free(temp);
        temp = NULL;
        return true;
    }
   // else
    //    LOG("%s : no message", __func__);
    return false;
}

#endif
uint32_t ty_get_tick_ms()
{
    int64_t ret = MG_osiUpTimeUS();
//    LOG("%s, time  = %ld\r\n", __func__, ret);
    return ret/1000;
}

void ty_sleep(int ms)
{
    osiThreadSleep(ms);   
} 


void LOG(const char *format, ... )
{
    #if TY_LOG_ENABLE
    va_list argp;
    char str[300] = {0};
    int ret = 0;
    va_start(argp, format);
    ret = vsnprintf( str, sizeof(str), format, argp);    
    va_end(argp);
    //#define TY_USE_AT_LOG
 
    #ifdef AT_LOG_PRINT
    AT_LOG(str);
    #else
    MG_osiTracePrintf(LOG_TAG, str);
    #endif
    
    #endif
}


void ty_printf_hexbuf(char *message,int len)
{
    int i;
    if(message){
    for(i = 0;i < strlen(message);i+=64)
    {
        char debug[64+3] = {0};
        memcpy(debug,message+i,(len-i >= 64) ? 64:(len-i));
        LOG("%i,[%s]\r\n",i,debug);
    }
        }
}

int ty_task_create(ty_task_func_t task_entry,char * name,int StackSize,void *pvParameters,int Priority,void * TCB_hanndler)
{
    uint32_t *handle = (uint32_t*)TCB_hanndler;
   *handle = (uint32_t)MG_osiThreadCreate(name, task_entry, pvParameters, Priority, StackSize);  ; 
    if(*handle == NULL)
    {
        MG_osiTracePrintf(LOG_TAG,"ty_task_create create failed\n");
        return -1;
    }
     return 0;  
}

void ty_task_delete( void *TCB_hanndler )
{
	uint32_t* currentId =  (uint32_t*)MG_osiThreadCurrent() ;
	if(currentId == *(uint32_t*)TCB_hanndler)
	{
		MG_osiTracePrintf(LOG_TAG, " ty_task_delete ok\n");
		MG_osiThreadExit();
	}
}

uint32_t ty_semaphore_create(uint32_t *pSemaphoreId)
{
	uint32_t max_count = 10;
	uint32_t init_count = 1;
	osiSemaphore_t* sem = MG_osiSemaphoreCreate(max_count, init_count);
	if(sem == NULL)
		return -1;
	*pSemaphoreId = (uint32_t)sem;
	return 1;
}

uint32_t ty_semaphore_take(uint32_t *pSemaphoreId,uint32_t xTicksToWait_ms)
{
//     MG_osiSemaphoreAcquire((osiSemaphore_t*)pSemaphoreId);
     MG_osiSemaphoreTryAcquire((osiSemaphore_t*)pSemaphoreId, xTicksToWait_ms);
//     ty_sleep(xTicksToWait_ms);
}

uint32_t ty_semaphore_give(uint32_t *pSemaphoreId)
{
    MG_osiSemaphoreRelease((osiSemaphore_t*)pSemaphoreId);
}

uint32_t ty_semaphore_destroy(uint32_t *pSemaphoreId)
{
      if(pSemaphoreId)
	    MG_osiSemaphoreDelete((osiSemaphore_t*)pSemaphoreId);
	if(!pSemaphoreId)
		return 1;
	return  -1;
}

uint32_t ty_timer_create(char* name, uint32_t *pTimerId,uint32_t ulTimerTick_ms,const uint32_t uxAutoReload, ty_timer_func_t callback_func )
{
	*pTimerId = (uint32_t)MG_osiTimerCreate(NULL, callback_func, NULL);
	if(NULL == *pTimerId)
	{
            MG_osiTracePrintf(LOG_TAG,"timer create failed\n");
            return -1;
	}
	if (MG_osiTimerSetPeriod( *pTimerId, ulTimerTick_ms, true))
	{
            MG_osiTracePrintf(LOG_TAG, "MG set period success");
            return 0;
	}
	else
	{
            MG_osiTracePrintf(LOG_TAG, "MG set period failed");
            return -1;
	}
//	return *pTimerId;     
       
}

uint32_t ty_timer_start(uint32_t *pTimerId)
{
    if(pTimerId)
        return MG_osiTimerStartLast((osiTimer_t *)*pTimerId);
    return -1;
}

uint32_t ty_timer_stop(uint32_t *pTimerId)
{
    if(pTimerId)
        return MG_osiTimerStop((osiTimer_t *)*pTimerId);  
    return -1;
}

uint32_t ty_timer_reset(uint32_t *pTimerId)
{
      return 0;
}

uint32_t ty_timer_delete(uint32_t *pTimerId)
{
      if(pTimerId){
        MG_osiTimerDelete((osiTimer_t *)*pTimerId);
        return 0;
    }
    return -1;
}

void ty_work_lock(int lock_dsp)
{
   
}

void ty_work_unlock()
{
 
}


//ע�����粢��ȡ�ź�ǿ��
static uint8_t int_net(const char *pApn, uint8_t *siglevel, int timeout)
{
        uint8_t cid = 1;
        uint64_t   ts_end = ty_get_tick_ms() + timeout;
        int64_t time_left = 0;
	if(MG_NwModifyPdpContext(0,cid,1,pApn))
	{
		MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt success");
		if(MG_NwActivePdpContext(0,cid))
		{
			MG_osiThreadSleep(10000);
			MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt failure");
		}
		bool active = false;
		while(!active){
                time_left = ts_end - (uint64_t)ty_get_tick_ms(); 
                MG_osiTracePrintf(LOG_TAG, "no active pdp context,check again");
                if (time_left < 0 && timeout != 0) {
#if ADPT_DEBUG                
                MG_osiTracePrintf(LOG_TAG, "active pdp context timeout\r\n");
#endif 
                return -1;
                }
                active = MG_NwHasActivePdp(0);
		}
		
		uint8_t rat = 0, biterr = 0;
		if(MG_NwGetSignalQuality(&rat, siglevel, &biterr)){
		MG_osiTracePrintf(LOG_TAG, "have active pdp context.");
		return *siglevel;
		}
	}
	
    return -1;
}
static bool getApn(char name[THE_APN_MAX_LEN]){
	CFW_GPRS_PDPCONT_DFTPDN_INFO PdnInfo = {0,};
	memset(&PdnInfo, 0, sizeof(CFW_GPRS_PDPCONT_DFTPDN_INFO));
	MG_CFW_GprsGetDefaultPdnInfo(&PdnInfo, 0);
	memcpy(name, PdnInfo.AccessPointName, PdnInfo.APNLen);
	for(int i = 0; ((i < PdnInfo.APNLen)&&(PdnInfo.AccessPointName[i]!='\0')); ++i)
	{
		if(PdnInfo.AccessPointName[i] == '.'){
		name[i] = '\0';
		break;
		}
		name[i] = PdnInfo.AccessPointName[i];
	}
	return (name[0]) ? false : true ;
}

// Return      : if return 1,tcpip is OK;else PDP not active,or not have SIM card;
int ty_wait_tcpip_ok(int timeout)
{
       int ret = -1;
    	int simStatus = 0, registerStatus = 0;
	char name[THE_APN_MAX_LEN] = {0};
	uint8_t siglevel = 0;
        uint64_t   ts_end = ty_get_tick_ms() + timeout;
        int64_t time_left;
    
do{
        simStatus = MG_cfwGetSimStatus(0);   //��ȡsim��״̬
        
#if ADPT_DEBUG
	MG_osiTracePrintf(LOG_TAG, "SIM status : %d", simStatus);
#endif
        
        	//��ȡ��Ӫ������
	while(getApn(name)){
                        time_left = 0;
                        time_left = ts_end - (uint64_t)ty_get_tick_ms(); 
#if ADPT_DEBUG
//                        MG_osiTracePrintf(LOG_TAG, "time_left %ld\r\n", time_left);
#endif
                        if (time_left < 0 && timeout != 0) {
#if ADPT_DEBUG                
                            MG_osiTracePrintf(LOG_TAG, "get APN timeout\r\n");
#endif 
                            return -1;
                        }
                     
#if ADPT_DEBUG         
//             MG_osiTracePrintf(LOG_TAG, "get APN\r\n");
#endif
		MG_osiThreadSleep(1000);
	}
#if ADPT_DEBUG    
	MG_osiTracePrintf(LOG_TAG, "Operators : %s", name);
#endif
        simStatus = MG_cfwGetSimStatus(0);   //�ٴλ�ȡsim��״̬        
#if ADPT_DEBUG
	MG_osiTracePrintf(LOG_TAG, "check again SIM status : %d", simStatus);
#endif
        	//��ȡ����״̬
	CFW_NW_STATUS_INFO sStatus;
	uint32_t uErrCode = MG_CFW_GprsGetstatus(&sStatus, 0);
	if(uErrCode == 0)
	{
		registerStatus = sStatus.nStatus;
	}
	else
	{
		registerStatus = -1;
#if ADPT_DEBUG        
             MG_osiTracePrintf(LOG_TAG, "register status: %d", registerStatus);
#endif            
	}
#if ADPT_DEBUG        
	MG_osiTracePrintf(LOG_TAG, "register status: %d", registerStatus);
#endif
        if(simStatus != 0)
        {
                if(-1 == int_net(name, &siglevel, timeout))
                {
#if ADPT_DEBUG                    
                    MG_osiTracePrintf(LOG_TAG, "register failed");
#endif
                    ret = -1;
                    break;
                }
                else
                {
#if ADPT_DEBUG                        
                    MG_osiTracePrintf(LOG_TAG, "signal quality : %d", siglevel);
#endif
                    ret = 1;
                    break;
                }
        }
        else
        {
#if ADPT_DEBUG            
                MG_osiTracePrintf(LOG_TAG, "sim error : %d", simStatus);
#endif
                ret = -1;
                break;

        }
    }while(0);
    return ret;
}

int ty_pdp_active(int timeout)
{
    uint8_t cid = 1;
    bool ret = MG_NwActivePdpContext(0, cid);
    if(true == ret)
        return 1;
    return -1;
}

int ty_pdp_deactive()
{
    uint8_t cid = 1;
    bool ret = MG_NwDeactivePdpContext(0, cid);
    if(true == ret)
        return 1;
    return -1;
}


int ty_rtc_get_sec (int zone_sec)
{   
    MG_date date;
    int64_t rtc_sec ;
    int offset = zone_sec;
    MG_GetCurDate(&date);
    MG_SetCurDate(&date,offset);
    rtc_sec  = MG_RtcToSec(&date);
    return rtc_sec - 8*60*60;
}

void ty_assert_proc(int value,char* filename, int lineno)
{
	if(value)
	{
		LOG("ASSERT %s %d \n", __func__, __LINE__);
		//assert(value);
	}
}


void ty_get_timestamp(char* timeBuff)
{
        if(timeBuff == NULL)
           return;
        MG_date date;
        char tmpbuff[24] = {0};
        MG_GetCurDate(&date);
//       LOG("current date : %04d-%02d-%02d-%02d-%02d-%02d" ,
//	  	date.nYear,
//	  	date.nMonth ,
//	  	date.nDay,
//	  	date.nHour,
//	  	date.nMin,
//	  	date.nSec );
       snprintf(tmpbuff,  24, "%04d%02d%02d%02d%02d%02d", 
                	  	date.nYear,
        	  	date.nMonth ,
        	  	date.nDay,
        	  	date.nHour,
        	  	date.nMin,
        	  	date.nSec );
#ifdef ADPT_DEBUG
        LOG("tmpbuff = %s %d", tmpbuff, strlen(tmpbuff));
#endif
        memcpy(timeBuff, tmpbuff, strlen(tmpbuff));         
}

static inline void*  nb12_plat_malloc(uint32_t ulSize)
{
    // return opencpu_malloc (ulSize);
}

static inline void nb12_plat_xy_free(void* pMem)
{
    // opencpu_free (pMem);
}

static void  psm_enter_cb(void)
{
    // opencpu_printf("%s\r\n",__FUNCTION__);
    // save_user_nv_flash ();
}

static void  psm_quit_cb(void)
{
    // opencpu_printf("%s\r\n",__FUNCTION__);
    // read_user_nv_flash ();
}

#if MEMLEAK_CHECK
static inline void*  slm320_dbg_malloc(uint32_t ulSize)
{
    return lpa_malloc (ulSize);
}

static inline void slm320_dbg_free(void* pMem)
{
    lpa_free (pMem);
}
#endif


void ty_plat_os_init()
{
    #if MEMLEAK_CHECK
    memleak_set_plat_malloc_free (osiMalloc, osiFree);
    dbg_init(1000);
    dbg_catch_sigsegv();
    //����CJSON�ڴ�������ͷŹ��Ӻ���
    cJSON_Hooks hooks;
    hooks.malloc_fn = slm320_dbg_malloc;
    hooks.free_fn = slm320_dbg_free;
    cJSON_InitHooks(&hooks);
    #else
    cJSON_Hooks hooks;
    hooks.malloc_fn = osiMalloc;
    hooks.free_fn = osiFree;
    cJSON_InitHooks(&hooks);
    #endif
    
  //  //SS11û�����ݶ�ȡ���Ӻ�������Ҫ����ʱ������ȡFLASH����
     read_user_nv_flash ();
  //  // ���PSM����ص��ӿڣ��������ݱ�����ָ�
    // opencpu_register_psmtable_callback (psm_enter_cb , psm_quit_cb);
}


