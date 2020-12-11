#include "ty_os_adpt.h"
#include "ty_sc_adpt.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_pipe.h"
#include <string.h>

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')
#define BUFFUERSIZE 1000
#define TMP_BUFER 2000
static osiSemaphore_t *t_sema = NULL;
static char tmp_buf[TMP_BUFER] = {0};
static osiPipe_t *at_rx_pipe=NULL;
static osiMutex_t *esimmutex = NULL;

#define SEM_TIMEOUT 20

int Ty_ScGetLogicChannelNo( sc_handle handle)
{
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL)
             return -1;
    return ctx->channel;  
}

static void prvVirtAtRespCallback(void *param, unsigned event)
{
    osiPipe_t *pipe = (osiPipe_t *)param;
    char *buf = (char*)lpa_malloc(TMP_BUFER);
    int bytes;
    memset(buf, 0, TMP_BUFER);
    for (;;)
    {
         bytes = osiPipeRead(pipe, buf, TMP_BUFER);
        if (bytes <= 0)
            break;
        buf[bytes] = '\0';
        if(strstr(buf, "+CCHO:") || strstr(buf, "+CGLA:"))   //AT时，对模组响应的关键字符串进行筛选
        {
#if ADPT_DEBUG
            MG_osiTracePrintf(LOG_TAG, "VAT <--Cb(%d): %s", bytes, buf);
#endif
            memset(tmp_buf, 0, sizeof(tmp_buf));
            strncpy(tmp_buf, buf, strlen(buf));
        }
        else if(strstr(buf, "ERROR"))						//响应到ERROR或者OK时接受响应完毕，释放信号量
        {
#if ADPT_DEBUG
            MG_osiTracePrintf(LOG_TAG, "VAT <--Cb(%d): %s", bytes, buf);
#endif 
            memset(tmp_buf, 0, sizeof(tmp_buf));
            strncpy(tmp_buf, buf, strlen(buf));
            MG_osiSemaphoreRelease(t_sema);
        }
        else if(strstr(buf, "OK"))
        {
#if ADPT_DEBUG
            MG_osiTracePrintf(LOG_TAG, "VAT <--Cb(%d): %s", bytes, buf);
#endif
            MG_osiSemaphoreRelease(t_sema);
        }else{}
    }
    lpa_free(buf);
    buf = NULL;
}

//AT+CCHO="A0000005591010FFFFFFFF8900000100"
sc_handle Ty_ScOpenLogicChannel(char *aid)
{
    uint8_t channel_id = 0;
    sc_handle ret = NULL;
    char *open_buffer = NULL;
    unsigned name = OSI_MAKE_TAG('V', 'A', 'T', '1');
    if(aid  == NULL)
            return ret;
    
    if(esimmutex == NULL)
        esimmutex = MG_osiMutexCreate();					      //创建互斥锁
    if(esimmutex == NULL)
	{
		MG_osiTracePrintf(LOG_TAG,"MG_osiMutexCreate failed\n");
		return ret;   
	}
    if(MG_osiMutexTryLock(esimmutex, 30*1000) == false)				//获取互斥锁
	{
		MG_osiTracePrintf(LOG_TAG," Ty_ScOpenLogicChannel MG_osiMutexTryLock  timeout\n");
		return ret;
	} 
    do
    {
        open_buffer = (char*)lpa_malloc(BUFFUERSIZE);
        if(open_buffer == NULL)
        {
            MG_osiTracePrintf(LOG_TAG,"lpa_malloc failed\n");
            return ret;        
        }
        if(at_rx_pipe == NULL)
            at_rx_pipe = MG_atDeviceVirtCreate(name,prvVirtAtRespCallback);    //创建虚拟AT管道
        if(at_rx_pipe == NULL)
        {
            MG_osiTracePrintf(LOG_TAG,"MG_atDeviceVirtCreate  failed\n");
            break;

        }
        memset(open_buffer, 0, BUFFUERSIZE);
        if(BUFFUERSIZE < strlen(aid) +12)										//判断传入aid是否越界
		{   
			MG_osiTracePrintf(LOG_TAG,"buffersize too short!\n");
				break;
		}
    snprintf(open_buffer, BUFFUERSIZE, "AT+CCHO=\"%s\"\r\n", aid);  
#if ADPT_DEBUG
    MG_osiTracePrintf(LOG_TAG, "VAT -->: %s", open_buffer);
#endif 
        memset(tmp_buf,0,sizeof(tmp_buf));
		if(MG_atDeviceVirtSend(at_rx_pipe,open_buffer) == false)                //发送AT指令
		{
			MG_osiTracePrintf(LOG_TAG,"Ty_ScOpenLogicChannel MG_atDeviceVirtSend failed!\n");
			break;
		}
		if(t_sema == NULL)
			t_sema = MG_osiSemaphoreCreate(1,0);				//创建信号量
		if(t_sema == NULL)
		{
			MG_osiTracePrintf(LOG_TAG,"MG_osiSemaphoreCreate failed!\n");
			break;
		}
		if(MG_osiSemaphoreTryAcquire(t_sema, SEM_TIMEOUT*1000) == false)			//获取信号量等待AT响应完毕
		{
			MG_osiTracePrintf(LOG_TAG,"Ty_ScOpenLogicChannel SemaphoreTryAcquire timeout\n");
			break;
		}
		if(strstr(tmp_buf, "ERROR"))
		{
			MG_osiTracePrintf(LOG_TAG,"%s line (%d):AT response error\n", __func__, __LINE__);
			break;
		}   
		char *ptr = strchr(tmp_buf,':');								//筛选响应字符串获取channel_id
		if(ptr == NULL)
		{
			break;
		}
		for(int i=0; i<strlen(ptr); i++)
		{
			if(ptr[i]>='0'&&ptr[i]<='9')
					 channel_id=channel_id*10+(ptr[i]-'0');
		}
		T_TyApduCtx *ctx = lpa_calloc(1,sizeof(T_TyApduCtx));
		if(!ctx)
		{   
			break;
		}
		ctx->channel = channel_id;      
		memcpy(ctx->AID,aid,strlen(aid));
#if ADPT_DEBUG
		MG_osiTracePrintf(LOG_TAG,"aid = %s,channelID = %d\r\n", ctx->AID, ctx->channel);
#endif
		ret = (sc_handle)ctx;
	}while(0);
    lpa_free(open_buffer);
    MG_osiMutexUnlock(esimmutex);
    return ret;
}

//AT+CGLA=1,10,"81CAFF2100"
int Ty_ScTransimt(sc_handle handle,  char *inData, char *outData,int *ioLen)
{     
    int lenth = 0;
    int ret = ApduUnknown;    
    char *tr_buffer =NULL;
    if(MG_osiMutexTryLock(esimmutex, 30*1000) == false)
    {
        MG_osiTracePrintf(LOG_TAG,"Ty_ScTransimt MG_osiMutexTryLock  timeout\n");
        return ApduTimeout;   
    }
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL || !( ctx->channel == 1 || ctx->channel == 2 || ctx->channel == 3))
	{
        MG_osiMutexUnlock(esimmutex);
        MG_osiTracePrintf(LOG_TAG,"%s line (%d):ApduParamErr\n", __func__, __LINE__);
        return ApduParamErr;
    }    
    do
    {
        tr_buffer= (char*)lpa_malloc(BUFFUERSIZE);
        if(tr_buffer == NULL)
        {
            MG_osiTracePrintf(LOG_TAG,"lpa_malloc failed\n");
            return ApduMemErr;
        }
        memset(tr_buffer, 0, BUFFUERSIZE);  
        if( inData == NULL || ioLen == NULL)
        {
            ret = ApduParamErr;
            break;
        }
        if(BUFFUERSIZE < strlen(inData) + 18)
        {   
            MG_osiTracePrintf(LOG_TAG,"buffersize too short!\n");
            ret = ApduMemErr;
            break;
        }
        snprintf(tr_buffer, BUFFUERSIZE, "AT+CGLA=%d,%d,\"%s\"\r\n", (uint8_t)ctx->channel, strlen(inData), inData);
#if ADPT_DEBUG
        MG_osiTracePrintf(LOG_TAG, "VAT -->: %s", tr_buffer);
#endif
        if(MG_atDeviceVirtSend(at_rx_pipe,tr_buffer) == false)
    	{
            MG_osiTracePrintf(LOG_TAG,"MG_atDeviceVirtSend failed!\n");
            ret = ApduCmdErr;
            break;
    	}    
        if(MG_osiSemaphoreTryAcquire(t_sema, 20*1000) == false)
        {
            MG_osiTracePrintf(LOG_TAG,"Ty_ScTransimt SemaphoreTryAcquire timeout\n");
            ret = ApduTimeout;
            break;
        }     
        if(strstr(tmp_buf, "ERROR"))
        {
            MG_osiTracePrintf(LOG_TAG,"%s line (%d):AT response error\n", __func__, __LINE__);
            ret = ApduCmdErr;
            break;
        }   
        char *ptr = strchr(tmp_buf,':');
        if(ptr == NULL)
        {
            ret =  ApduMemErr;
            break;
        }
        for(int i=0; ptr[i] != ','; i++)
        {
            if(ptr[i]>='0'&&ptr[i]<='9')
                lenth=lenth*10+(ptr[i]-'0');
        } 
        if(lenth > *ioLen)
        {
            ret = ApduParamErr;
        }
        *ioLen = lenth;
        ptr=strchr(ptr, ',');
        memcpy(outData, ptr+2, lenth);
        ret = 0;
    }while(0);
    lpa_free(tr_buffer);
    MG_osiMutexUnlock(esimmutex);
    return ret;      
}
//AT+CCHC=1
int Ty_ScCloseLogicChannel(sc_handle handle)
{    
    char close_buffer[15] = {0};  
    int ret = ApduUnknown;  
    if(MG_osiMutexTryLock(esimmutex, 30*1000) == false)
    {
        MG_osiTracePrintf(LOG_TAG," Ty_ScCloseLogicChannel MG_osiMutexTryLock  timeout\n");
        return ApduTimeout;
    }
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL || !( ctx->channel == 1 || ctx->channel == 2 || ctx->channel == 3))
	{
        MG_osiMutexUnlock(esimmutex);
        MG_osiTracePrintf(LOG_TAG,"%s  line (%d):ApduParamErr\n", __func__, __LINE__);
        return ApduParamErr;
    }    
    do
	{
		memset(close_buffer, 0, sizeof(close_buffer));  
		snprintf(close_buffer, 15, "AT+CCHC=%d\r\n",(uint8_t)ctx->channel);
	#if ADPT_DEBUG
		MG_osiTracePrintf(LOG_TAG, "VAT -->: %s", close_buffer);
	#endif
		if(MG_atDeviceVirtSend(at_rx_pipe,close_buffer) == false)
		{
			MG_osiTracePrintf(LOG_TAG,"MG_atDeviceVirtSend failed!\n");
			ret =  ApduCmdErr;
			break;
		} 
		if(MG_osiSemaphoreTryAcquire(t_sema, 20*1000) == false)
		{
			MG_osiTracePrintf(LOG_TAG,"Ty_ScCloseLogicChannel SemaphoreTryAcquire timeout\n");
			ret =  ApduTimeout;
			break;
		}
		if(strstr(tmp_buf, "ERROR"))
		{
			MG_osiTracePrintf(LOG_TAG,"%s  line (%d):AT response error\n", __func__, __LINE__);
			ret =  ApduCmdErr;
			break;
		}   
	}while(0);
    lpa_free(ctx);    
    MG_osiMutexUnlock(esimmutex);
    return ret;
}

int Ty_ScTransimtAutoFetch(sc_handle handle, char *inData, char **outData)
{
     int lenth = 0;
    int ret = ApduUnknown;    
    char *tr_buffer =NULL;
    char* tempoutData = (char*)lpa_malloc(TMP_BUFER);
    if(tempoutData == NULL)
        {
            LOG("%s: %d malloc failed!", __func__, __LINE__);
            return ApduParamErr;
        }
    memset(tempoutData, 0, TMP_BUFER);
    if(MG_osiMutexTryLock(esimmutex, 30*1000) == false)
    {
        MG_osiTracePrintf(LOG_TAG,"Ty_ScTransimt MG_osiMutexTryLock  timeout\n");
        return ApduTimeout;   
    }
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL || !( ctx->channel == 1 || ctx->channel == 2 || ctx->channel == 3))
	{
        MG_osiMutexUnlock(esimmutex);
        MG_osiTracePrintf(LOG_TAG,"%s line (%d):ApduParamErr\n", __func__, __LINE__);
        return ApduParamErr;
    }    
    do
    {
        tr_buffer= (char*)lpa_malloc(BUFFUERSIZE);
        if(tr_buffer == NULL)
        {
            MG_osiTracePrintf(LOG_TAG,"lpa_malloc failed\n");
            return ApduMemErr;
        }
        memset(tr_buffer, 0, BUFFUERSIZE);  
        if( inData == NULL)
        {
            ret = ApduParamErr;
            break;
        }
        if(BUFFUERSIZE < strlen(inData) + 18)
        {   
            MG_osiTracePrintf(LOG_TAG,"buffersize too short!\n");
            ret = ApduMemErr;
            break;
        }
        snprintf(tr_buffer, BUFFUERSIZE, "AT+CGLA=%d,%d,\"%s\"\r\n", (uint8_t)ctx->channel, strlen(inData), inData);
#if ADPT_DEBUG
        MG_osiTracePrintf(LOG_TAG, "VAT -->: %s", tr_buffer);
#endif
        if(MG_atDeviceVirtSend(at_rx_pipe,tr_buffer) == false)
    	{
            MG_osiTracePrintf(LOG_TAG,"MG_atDeviceVirtSend failed!\n");
            ret = ApduCmdErr;
            break;
    	}    
        if(MG_osiSemaphoreTryAcquire(t_sema, 20*1000) == false)
        {
            MG_osiTracePrintf(LOG_TAG,"Ty_ScTransimt SemaphoreTryAcquire timeout\n");
            ret = ApduTimeout;
            break;
        }     
        if(strstr(tmp_buf, "ERROR"))
        {
            MG_osiTracePrintf(LOG_TAG,"%s line (%d):AT response error\n", __func__, __LINE__);
            ret = ApduCmdErr;
            break;
        }   
        char *ptr = strchr(tmp_buf,':');
        if(ptr == NULL)
        {
            ret =  ApduMemErr;
            break;
        }
        for(int i=0; ptr[i] != ','; i++)
        {
            if(ptr[i]>='0'&&ptr[i]<='9')
                lenth=lenth*10+(ptr[i]-'0');
        } 
        if(lenth > TMP_BUFER)
        {
            LOG("%s: %d malloc tmp_buffer too short!", __func__, __LINE__);
            ret = ApduParamErr;
        }
//        *ioLen = lenth;
        ptr=strchr(ptr, ',');
        memcpy(tempoutData, ptr+2, lenth);
        *outData = tempoutData;
        ret = lenth;
    }while(0);
    lpa_free(tr_buffer);
    MG_osiMutexUnlock(esimmutex);
    return ret;      
}


