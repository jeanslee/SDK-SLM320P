#include "ty_esim_at.h"
#include "mg_cmd_api.h"
#include "lpa.h"
#include "osi_api.h"
#include "osi_log.h"
#include "ty_os_adpt.h"

#define PROFILE_LIST_LEN 2048
#define ESIM_FUNC_PROFILE_LIST      "PROFILESINFO"
#define ESIM_FUNC_ENABLE_PROFILE    "ENABLE"
#define ESIM_FUNC_DISABLE_PROFILE    "DISABLE"
#define ESIM_FUNC_VER_INFO    "VER"

#define ESIM_FUNC_TEST_ERR    "TESTERR"


#define SETCFG_FUNC_TEST    "test"
#define SETCFG_FUNC_SSTEST    "smstest"
#define SETCFG_FUNC_DUMPMEM    "dumpmem"
#define SETCFG_FUNC_FOTA    "fota"


static int ty_esim_thread_handle = 0;
static uint32_t *ty_esim_queue = NULL;
static osiSemaphore_t *ty_esim_sem = NULL;
#ifdef AT_LOG_PRINT
static  atCmdEngine_t* debug_eng = NULL;
#endif

typedef struct
{
    int cmdid;
    atCmdEngine_t* eng;
    char *request;
}T_TyEsimMsg;

typedef enum
{
    E_TyEsimCmd_ProfileList = 1,
    E_TyEsimCmd_ProfileEnable,
    E_TyEsimCmd_ProfileDisable,
    E_TyEsimCmd_Fota,
    E_TyEsimCmd_smstest,
}E_TyEsimCmd;

#ifndef SSNB_BUILD_LIB

#ifndef PRODUCT_TYPE
#error "no define macro PRODUCT_TYPE"
#endif

#ifndef FIRMWARE_VERSION
#error "no define macro FIRMWARE_VERSION"
#endif

#else
//由外部链接调用库定义这些全局变量
extern const char *PRODUCT_TYPE;
extern const char *FIRMWARE_VERSION;
extern const char *MQTT_PRODUCT_KEY;
extern const char *MQTT_PRODUCT_SECRET;

#endif



#ifdef AT_LOG_PRINT
void AT_LOG(const char *format, ... )
{
    #if TY_LOG_ENABLE
    va_list argp;
    char str[256] = {0};
    int ret = 0;
    va_start(argp, format);
    ret = vsnprintf( str, sizeof(str), format, argp);    
    va_end(argp);
    if(debug_eng)
    {
        MG_sendUartText(debug_eng, str);
    }
    #endif
}
#endif


static int at_get_next_int(char **line,int *value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;

    }
    ret = at_tok_nextint(line, value); // 
    if (ret < 0)
    {
        return ret;
    } 

    return 0;
}

static int at_get_next_str(char **line,char **value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;
    }
    ret  = at_tok_nextstr(line, value); // ""
    if (ret < 0)
    {
        return -2;
    }

    return 0;
}


static int ty_esim_at_profile_list(char **outResp)
{
    int ret,i,offset =0;
    Lpa_Ctx ctx = {0};
    ProfileInfoList list = {0};
    ret = TyGetProfilesInfo(&ctx,&list, profileClass_test);
    if(ret < 0)
        return ret;
    if(list.profileSize > 0)
        *outResp = lpa_calloc(1,PROFILE_LIST_LEN);
    LOG("%s profileSize =%d\r\n",__FUNCTION__,list.profileSize);
    ret = snprintf((*outResp)+offset,PROFILE_LIST_LEN - offset,"\r\n+ESIM PROFILESINFO:");
    offset += ret;
    for(i = 0;i < list.profileSize;i++)
    {
        ret = snprintf((*outResp)+offset,PROFILE_LIST_LEN - offset,"[%s,%s]\r\n",list.profileEntity[i].iccid,list.profileEntity[i].profileState);
        offset += ret;
    }
    TyFreeProfileInfoList (&list);
    LOG("%s offset =%d\r\n",__FUNCTION__,offset);
    return offset; 
}

static int ty_esim_at_profile_enable(char *iccid,char **outResp)
{
    int ret,i,offset =0;
    Lpa_Ctx ctx = {0};
    char *result_msg = NULL;
    ret = TyEnable (&ctx, iccid, opr_mark_null);
    *outResp = lpa_calloc(1,PROFILE_LIST_LEN);
    if(ret == 0)
        result_msg = "OK";
    else result_msg = "FAIL";
    ret = snprintf((*outResp)+offset,PROFILE_LIST_LEN - offset,"\r\nESIM ENABLE:%s\r\n",result_msg);
    
    return ret; 
}

static int ty_esim_at_profile_disable(char *iccid,char **outResp)
{
    int ret,i,offset =0;
    Lpa_Ctx ctx = {0};
    char *result_msg = NULL;
    ret = TyDisable (&ctx, iccid, opr_mark_null);
    *outResp = lpa_calloc(1,PROFILE_LIST_LEN);
    if(ret == 0)
        result_msg = "OK";
    else result_msg = "FAIL";
    ret = snprintf((*outResp)+offset,PROFILE_LIST_LEN - offset,"\r\nESIM DISABLE:%s\r\n",result_msg);
    
    return ret; 

}



void ty_esim_AT_HANDLE(atCmdEngine_t* eng, char* text)
{
    //deal with text
    char * err = NULL,*text_end = "\r\nOK\r\n";
    int errcode = -1;
    char *func = NULL;
    char *iccid = NULL;
    T_TyEsimMsg msg = {0};
#ifdef AT_LOG_PRINT
        if(!debug_eng)
        debug_eng = eng;
#endif
    if (err)
    {
        MG_sendUartText(eng, err);
        MG_sendUartError(eng, errcode);     //send error and terminate AT handle
        return ;
    }

    char *line = text;
    errcode = at_get_next_str (&line, &func);
    if(errcode < 0)
    {
        MG_sendUartError(eng, errcode);
        return ;
    }
    LOG("%s func=%s\r\n",__FUNCTION__,func);
    if(strlen(func) == strlen(ESIM_FUNC_PROFILE_LIST) && 
      0 == strncasecmp (func,ESIM_FUNC_PROFILE_LIST,strlen(ESIM_FUNC_PROFILE_LIST)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,ESIM_FUNC_PROFILE_LIST);
        msg.cmdid = E_TyEsimCmd_ProfileList;
    }
    else if(strlen(func) == strlen(ESIM_FUNC_ENABLE_PROFILE) && 
      0 == strncasecmp (func,ESIM_FUNC_ENABLE_PROFILE,strlen(ESIM_FUNC_ENABLE_PROFILE)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,ESIM_FUNC_ENABLE_PROFILE);
        msg.cmdid = E_TyEsimCmd_ProfileEnable;
        errcode = at_get_next_str (&line, &iccid);
        if(errcode < 0 || iccid == NULL)
        {
            LOG("%s %d iccid=%s\r\n",__FUNCTION__,__LINE__,iccid);
            MG_sendUartError(eng, errcode);
            return ;
        }
        LOG("%s %d iccid=[%s]\r\n",__FUNCTION__,__LINE__,iccid);
    }
    else if(strlen(func) == strlen(ESIM_FUNC_DISABLE_PROFILE) && 
      0 == strncasecmp (func,ESIM_FUNC_DISABLE_PROFILE,strlen(ESIM_FUNC_DISABLE_PROFILE)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,ESIM_FUNC_DISABLE_PROFILE);
        msg.cmdid = E_TyEsimCmd_ProfileDisable;
        errcode = at_get_next_str (&line, &iccid);
        if(errcode < 0 || iccid == NULL)
        {
            LOG("%s %d iccid=%s\r\n",__FUNCTION__,__LINE__,iccid);
            MG_sendUartError(eng, errcode);
            return ;
        }
        LOG("%s %d iccid=[%s]\r\n",__FUNCTION__,__LINE__,iccid);
    }
    else if(strlen(func) == strlen(ESIM_FUNC_VER_INFO) && 
      0 == strncasecmp (func,ESIM_FUNC_VER_INFO,strlen(ESIM_FUNC_VER_INFO)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,ESIM_FUNC_VER_INFO);
        if(FIRMWARE_VERSION)
        {
            MG_sendUartText(eng, FIRMWARE_VERSION);
        }
        MG_sendUartOK(eng);
        return ;
    }
    else if(strlen(func) == strlen(ESIM_FUNC_TEST_ERR) && 
      0 == strncasecmp (func,ESIM_FUNC_TEST_ERR,strlen(ESIM_FUNC_TEST_ERR)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,ESIM_FUNC_TEST_ERR);
        msg.cmdid = 10000;
    }
    else
    {
        LOG("%s %d not support\r\n",__FUNCTION__,__LINE__);
        MG_sendUartError(eng, errcode);
        return ;
    }
    if(iccid)
    {
        msg.request = (char*)lpa_calloc(1,strlen(iccid)+1);
        if(!msg.request)
        {
            MG_sendUartError(eng, errcode);
            return ;
        }
        memcpy(msg.request,iccid,strlen(iccid));
    }
    
    msg.eng = eng;
    if(ty_esim_queue)
        ty_put_msg_que (ty_esim_queue, &msg, 0);
    else
        MG_sendUartError(eng, errcode);
    return ;
}

void ty_SetJulinkConfig_AT_HANDLE(atCmdEngine_t* eng, char* text)
{
    //deal with text
    char * err = NULL,*text_end = "\r\nSetJulinkConfig OK\r\n";
    int errcode =  -1;
    T_TyEsimMsg msg = {0};
#ifdef AT_LOG_PRINT
    if(!debug_eng)
    debug_eng = eng;
#endif
    if (err)
    {
        MG_sendUartText(eng, err);
        MG_sendUartError(eng, errcode);     //send error and terminate AT handle
        return ;
    }
    char *line = text,*func = NULL;
    int option;
    errcode = at_get_next_str (&line, &func);
    if(errcode < 0 || !func)
    {
        MG_sendUartError(eng, errcode);
        return ;
    }
    LOG("%s func=%s\r\n",__FUNCTION__,func);
    if(strlen(func) == strlen(SETCFG_FUNC_TEST) && 
      0 == strncasecmp (func,SETCFG_FUNC_TEST,strlen(SETCFG_FUNC_TEST)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,SETCFG_FUNC_TEST);
        errcode = at_get_next_int (&line, &option);
        if(errcode < 0)
        {
            MG_sendUartError(eng, errcode);
            return ;
        }
        #ifdef TY_CLOUD_ENABLE
        app_main_test (option);
        #endif
    }
    else if(strlen(func) == strlen(SETCFG_FUNC_SSTEST) && 
      0 == strncasecmp (func,SETCFG_FUNC_SSTEST,strlen(SETCFG_FUNC_SSTEST)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,SETCFG_FUNC_SSTEST);
        msg.cmdid = E_TyEsimCmd_smstest;
        msg.request = (char*)lpa_calloc(1,strlen(line)+1);
        if(!msg.request)
        {
            MG_sendUartError(eng, errcode);
            return ;
        }
        memcpy(msg.request,line,strlen(line));
        msg.eng = eng;
        if(ty_esim_queue)
            ty_put_msg_que (ty_esim_queue, &msg, 0);
        else
            MG_sendUartError(eng, errcode);
        return ;
    }
    else if(strlen(func) == strlen(SETCFG_FUNC_DUMPMEM) && 
      0 == strncasecmp (func,SETCFG_FUNC_DUMPMEM,strlen(SETCFG_FUNC_DUMPMEM)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,SETCFG_FUNC_DUMPMEM);
        
#if MEMLEAK_CHECK
        dbg_mem_stat();
        dbg_heap_dump("");
#endif

    }   
    else if(strlen(func) == strlen(SETCFG_FUNC_FOTA) && 
      0 == strncasecmp (func,SETCFG_FUNC_FOTA,strlen(SETCFG_FUNC_FOTA)))
    {
        LOG("%s do %s \r\n",__FUNCTION__,SETCFG_FUNC_FOTA);
        msg.cmdid = E_TyEsimCmd_Fota;
        msg.request = (char*)lpa_calloc(1,strlen(line)+1);
        if(!msg.request)
        {
            MG_sendUartError(eng, errcode);
            return ;
        }
        memcpy(msg.request,line,strlen(line));
        msg.eng = eng;
        if(ty_esim_queue)
            ty_put_msg_que (ty_esim_queue, &msg, 0);
        else
            MG_sendUartError(eng, errcode);
        return ;
    }
    //deal end
    MG_sendUartText(eng, text);
    MG_sendUartText(eng, text_end);
    MG_sendUartOK(eng);                     //send OK and terminate AT handle
    return ;
}


void ty_GetJulinkConfig_AT_HANDLE(atCmdEngine_t* eng, char* text)
{
    //deal with text
    char * err = NULL,*text_end = "\r\nGetJulinkConfig OK\r\n";
    int errcode;
#ifdef AT_LOG_PRINT
        if(!debug_eng)
        debug_eng = eng;
#endif
    if (err)
    {
        MG_sendUartText(eng, err);
        MG_sendUartError(eng, errcode);     //send error and terminate AT handle
        return ;
    }
    
    //deal end
    MG_sendUartText(eng, text);
    MG_sendUartText(eng, text_end);
    MG_sendUartOK(eng);                     //send OK and terminate AT handle
    return ;
}


void ty_esim_at_thread(void* param)
{
    int ret;
    while(1)
    {
        T_TyEsimMsg msg = {0};
        char *outResp  = NULL;
        if(ty_get_msg_que(ty_esim_queue, (void *)&msg, 1000))
        {
            if(!msg.eng)
                continue;
            switch(msg.cmdid)
            {
                case E_TyEsimCmd_ProfileList:
                {
                    outResp = NULL;
                    int len = ty_esim_at_profile_list (&outResp);
                    if(len <= 0)
                    {
                        LOG("%s ty_esim_at_profile_list fail\r\n",__FUNCTION__);
                        MG_sendUartError (msg.eng, 1);
                        break;
                    }
                    //LOG("%s\r\n",outResp);
                    MG_sendUartText(msg.eng, outResp);
                    MG_sendUartOK(msg.eng);
                    lpa_free(outResp);
                    outResp = NULL;
                    LOG("%s ty_esim_at_profile_list OK\r\n",__FUNCTION__);
                    break;
                }
                case E_TyEsimCmd_ProfileEnable:
                {
                    if(!msg.request)
                    {
                        MG_sendUartError (msg.eng, 1);
                        break;
                    }
                    outResp = NULL;
                    int len = ty_esim_at_profile_enable (msg.request,&outResp);
                    if(len <= 0)
                    {
                        LOG("%s ty_esim_at_profile_enable fail\r\n",__FUNCTION__);
                        MG_sendUartError (msg.eng, 1);
                        break;
                    }
                    //LOG("%s\r\n",outResp);
                    MG_sendUartText(msg.eng, outResp);
                    MG_sendUartOK(msg.eng);
                    lpa_free(outResp);
                    lpa_free(msg.request);
                    outResp = NULL;
                    LOG("%s ty_esim_at_profile_enable OK\r\n",__FUNCTION__);
                    break;
                }
                case E_TyEsimCmd_ProfileDisable:
                {
                    outResp = NULL;
                    if(!msg.request)
                    {
                        MG_sendUartError (msg.eng, 1);
                        break;
                    }                    
                    int len = ty_esim_at_profile_disable (msg.request,&outResp);
                    if(len <= 0)
                    {
                        LOG("%s ty_esim_at_profile_disable fail\r\n",__FUNCTION__);
                        MG_sendUartError (msg.eng, 1);
                        break;
                    }
                    //LOG("%s\r\n",outResp);
                    MG_sendUartText(msg.eng, outResp);
                    MG_sendUartOK(msg.eng);
                    lpa_free(outResp);
                    lpa_free(msg.request);
                    outResp = NULL;
                    LOG("%s ty_esim_at_profile_disable OK\r\n",__FUNCTION__);
                    break;
                 }
                /*
                case 10000:
                {
                    LOG("%s test cmd %d\r\n",__FUNCTION__,msg.cmdid);
                    break;
                }
                */
                case E_TyEsimCmd_Fota:
                {
                    if(msg.request)
                    {
                        ret = ty_fata_at (msg.request);
                        lpa_free(msg.request);
                    }
                    else
                    {
                        LOG("%s request NULL\r\n",__FUNCTION__);
                    }
                    if(ret == 0)
                    {
                        MG_sendUartText(msg.eng, "\r\n+FIRMEWARE DOWNLOAD OK\r\n");
                        while(!ty_fota_task_exit())
                        {
                            ty_sleep(1000);
                        }
                        MG_sendUartText(msg.eng, "\r\n+FIRMEWARE UPDATE FAIL\r\n");
                        MG_sendUartError (msg.eng, -1);
                    }
                    else
                    {
                         MG_sendUartText(msg.eng, "\r\n+FIRMEWARE DOWNLOAD FAIL\r\n");
                         MG_sendUartError (msg.eng, ret);
                    }
                    break;
                }                
                case E_TyEsimCmd_smstest:
                {
                    if(msg.request)
                    {
                        #ifdef TY_CLOUD_ENABLE
                        LOG("%s do at_proc_lpa_app_sms_test\r\n",__FUNCTION__);
                        at_proc_lpa_app_sms_test (msg.request, NULL);
                        #endif
                        lpa_free(msg.request);
                    }
                    else
                    {
                        LOG("%s request NULL\r\n",__FUNCTION__);
                    }
                    MG_sendUartOK(msg.eng);
                    break;
                }
                default:
                {
                    LOG("%s unkown cmd %d\r\n",__FUNCTION__,msg.cmdid);
                    MG_sendUartError (msg.eng, 1);
                    break;

                }
                    
            }
        }
      
    }
}


void ty_esim_at_init ()
{

    ty_esim_queue =(uint32_t*)ty_create_msg_Que(10, sizeof(T_TyEsimMsg));
    if(!ty_esim_queue)
        return ;
    if(ty_esim_sem == NULL)
			ty_esim_sem = MG_osiSemaphoreCreate(1,0);
    ty_task_create(ty_esim_at_thread, "ty_esim_at", 1024*32, NULL, OSI_PRIORITY_NORMAL, &ty_esim_thread_handle);
    
    MG_registCusATHandle(Cus_AT_eSIM, ty_esim_AT_HANDLE);
    MG_registCusATHandle(Cus_AT_SetJulinkConfig, ty_SetJulinkConfig_AT_HANDLE);
    MG_registCusATHandle(Cus_AT_GetJulinkConfig, ty_GetJulinkConfig_AT_HANDLE);

    
}

