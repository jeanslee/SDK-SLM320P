#include "ty_mqtt_basic_info.h"
#include "ty_devinfo.h"
#include "ty_ss_cloud.h"
#include "main_task.h"
#include "queue_task.h"
#include "ty_os_adpt.h"
#include "ty_ss_cloud.h"

#ifdef TY_ESIM_PROFILE
#include "lpa.h"
#endif

typedef struct 
{
    char productKey[64];
    char proudctSecret[64];
    char deviceName[32];
    char topic_taskdown[TOPIC_MAX_LEN];
    char topic_taskup[TOPIC_MAX_LEN];
    char topic_reportup[TOPIC_MAX_LEN];
    char topic_reportdown[TOPIC_MAX_LEN];
    char topic_offline[TOPIC_MAX_LEN];
    void *msgQueue;
    ty_queue_msg cache_msg;
    ty_cloudmsg_report_down down;
    uint32_t reportdown_sem;
    bool report_reply;
    uint32_t idletimer;
    bool idletimeout;
    bool fota;
    bool do_samtsm;
    int connect_err_cnt;
    int mqtt_yield_s;
    int status;
    void *mqtt_handle;
}MainTaskCtx;
static MainTaskCtx _maintask = {0,
.reportdown_sem = -1,
.idletimer = -1,
.mqtt_yield_s = 0,
.status = E_MainTaskExit,
.mqtt_handle = NULL
};

static uint32_t g_lpa_app_Handle = -1;
static uint32_t g_lpa_app_QueueHandle = -1;
extern int flash_status;
int ty_cloud_ext_notify(const char *msg)
{
    if(msg != NULL && strlen(msg) > 0)
    {
        ty_send_rsp_str_to_ext(msg);
    }
    return 0;
}

#ifdef TY_ESIM_TSM_SAM
static int free_module_samtsm_info(ty_module_SE_TSM *module_SE_TSM)
{
    int i;
    if(!module_SE_TSM)
        return -1;
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    for(i = 0;i < module_SE_TSM->AID_size ;i++)
    {
        if(module_SE_TSM->AID_list[i].app_status)
            lpa_free(module_SE_TSM->AID_list[i].app_status); 
        if(module_SE_TSM->AID_list[i].app_life_cycle_state)
            lpa_free(module_SE_TSM->AID_list[i].app_life_cycle_state);
        if(module_SE_TSM->AID_list[i].app_aid)
            lpa_free(module_SE_TSM->AID_list[i].app_aid ); 
        if(module_SE_TSM->AID_list[i].app_version)
        lpa_free(module_SE_TSM->AID_list[i].app_version );
    }
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    if(module_SE_TSM->AID_list)
        lpa_free(module_SE_TSM->AID_list);
    module_SE_TSM->AID_list = NULL;
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    return 0;
}

static int load_module_samtsm_info(ty_module_SE_TSM *module_SE_TSM)
{
    int i;
    int ret;
    char *json = NULL;
    int json_len = 0;
    ty_cloudmsg_task_up taskup = {0};
    ty_queue_msg qmsg = {0};
    Aid_List *list = NULL;
    static char seid[65] = {0};
    int space = 0;
    ret = Ty_getSeid(seid);
    if(ret != 0)
    {
        LOG("%s Ty_getSeid err\r\n",__FUNCTION__);
        //return ret;
    }

    //动态获取SAM应用信息
    int appState;
    char version[20] = {0};
    ret = Ty_getAppVersion_appState(SAM_AID,version , &appState);
    LOG("Ty_getAppVersion_appState ret = %d\r\n",ret);
    if (ret == 0)
    {
        LOG("appState = %d ,version = %s\r\n",appState,version);
        sm_aid aid = {0};
        memcpy(aid.app_aid,SAM_AID,strlen(SAM_AID));
        aid.app_status = (unsigned char)appState;
        memcpy(aid.app_version,version,strlen(version));
        aid.app_life_cycle_state = 1;
        ty_SaveAppletInfoNoNv (&aid);
    }
    else
    {
        LOG("SAM_AID %s Ty_getAppVersion_appState ERROR=%d\r\n",SAM_AID,ret);
    }

    ret = Ty_getRestSpace(&space);
    LOG("%s Ty_getRestSpace ret = %d , space = %d\r\n",__FUNCTION__ , ret , space);

    ret =  ty_LoadAppletInfoList (&list);
    if(ret != 0 || list == NULL)
    {
        LOG("%s ty_LoadAppletInfoList err\r\n",__FUNCTION__);
        return ret;
    }
    

    module_SE_TSM->SEID  = seid;
    module_SE_TSM->AID_size = list->aid_size;
    module_SE_TSM->rest_space = space;

    if(list->aid_size > 0)
    {
        module_SE_TSM->AID_list = lpa_malloc(sizeof(ty_AID)*list->aid_size);
        if (NULL == module_SE_TSM->AID_list){
            ret = -1;
            goto exit;
        }
        memset(module_SE_TSM->AID_list, 0, sizeof(ty_AID)*list->aid_size);
        
        for(i = 0;i < list->aid_size;i++)
        {
            module_SE_TSM->AID_list[i].app_status = _byte2str (list->list[i].app_status);
            module_SE_TSM->AID_list[i].app_life_cycle_state = _byte2str (list->list[i].app_life_cycle_state);
            module_SE_TSM->AID_list[i].app_aid = _strdup (list->list[i].app_aid);
            module_SE_TSM->AID_list[i].app_version = _strdup (list->list[i].app_version);
        }
    }
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    if(list != NULL)
        ty_AppletInfoListFree (list);
    list = NULL;
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    return 0;
exit:
    if(list != NULL)
    {   
        LOG("%s %d \r\n",__FUNCTION__,__LINE__);
        if(module_SE_TSM->AID_list)
            free_module_samtsm_info (module_SE_TSM);
        ty_AppletInfoListFree (list);
        list = NULL;
        LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    }
    return ret;
}
#endif

#ifndef TY_ESIM_PROFILE
int ty_LoadModuleSIM(ty_module_SIM **moduleSIMList)
{   
    int i =0 ,ret = -1;
    char output[128] = {0};
    char *iccid = lpa_calloc(1,32);
    if(!iccid)
        return -1;
    do
    {
        if(at_iccid (iccid))
        {
            LOG("SS11_PLAT at_iccid ERROR\r\n");
            ret = -1;
            break;
        }
        LOG("SS11_PLAT iccid=%s\r\n",iccid);
        *moduleSIMList = lpa_malloc(sizeof(ty_module_SIM));
        if(*moduleSIMList == NULL)
        {
            ret = -1;
            break;
        }
        (*moduleSIMList)->size = 1;
        (*moduleSIMList)->code_no_list = lpa_calloc(1,sizeof(ty_code_No));
        if((*moduleSIMList)->code_no_list == NULL)
        {
            lpa_free(*moduleSIMList);
            *moduleSIMList = NULL;
            break;
        }
        (*moduleSIMList)->code_no_list[0].write_iccid = true;
        (*moduleSIMList)->code_no_list[0].use_iccid = true;
        (*moduleSIMList)->code_no_list[0].bind_iccid = iccid;
        ret = 0;
    }while(0);
    if(iccid)
    {
        lpa_free(iccid);
    }
    return ret;
}

void ty_LoadModuleSIMFree(ty_module_SIM *moduleSIMList)
{
    if(!moduleSIMList)
        return ;
    if(!moduleSIMList->code_no_list)
        return ;
    int i;
    for(i = 0 ; i < moduleSIMList->size ; i++ )
    {
        if(moduleSIMList->code_no_list[i].bind_iccid)
            lpa_free(moduleSIMList->code_no_list[i].bind_iccid);
        moduleSIMList->code_no_list[i].bind_iccid = NULL;
        
    }

    lpa_free(moduleSIMList->code_no_list);
    moduleSIMList->code_no_list = NULL;
    lpa_free(moduleSIMList);
    LOG("free esim OK\r\n");
}
#else
int ty_LoadModuleSIM(ty_module_SIM **moduleSIMList)
{   
    int i =0 ,ret = -1;
    char output[128] = {0};
    char *iccid = lpa_calloc(1,32);
    ProfileInfoList list = {0};
    Lpa_Ctx ctx = {0};
    if(!iccid)
        return -1;
    do
    {
        if(at_iccid (iccid))
        {
            LOG("SS11_PLAT at_iccid ERROR\r\n");
            ret = -1;
            break;
        }
        LOG(" iccid=%s\r\n",iccid);
        ret = TyGetProfilesInfo(&ctx,&list, profileClass_test);
        LOG(" TyGetProfilesInfo ret=%d profileSize=%d\r\n",ret,list.profileSize);
        if(ret == 0 &&  0 != list.profileSize)    
        {
            *moduleSIMList = lpa_malloc(sizeof(ty_module_SIM));
            if(*moduleSIMList == NULL)
            {
                ret = -1;
                break;
            }
            (*moduleSIMList)->size = list.profileSize;
            (*moduleSIMList)->code_no_list = lpa_calloc(list.profileSize,sizeof(ty_code_No));
            if((*moduleSIMList)->code_no_list == NULL)
            {
                lpa_free(*moduleSIMList);
                *moduleSIMList = NULL;
                break;
            }

            for(i = 0;i<list.profileSize;i++)
            {
                if(0 == strncmp(iccid,list.profileEntity[i].iccid,strlen(iccid)))
                {
                    (*moduleSIMList)->code_no_list[i].use_iccid = true;
                }
                else
                    (*moduleSIMList)->code_no_list[i].use_iccid = false;
                
                (*moduleSIMList)->code_no_list[i].write_iccid = true;
                
                (*moduleSIMList)->code_no_list[i].bind_iccid = _strdup(list.profileEntity[i].iccid);            
            }
            TyFreeProfileInfoList (&list);
        }
        else  
        {
            *moduleSIMList = lpa_malloc(sizeof(ty_module_SIM));
            if(*moduleSIMList == NULL)
            {
                ret = -1;
                break;
            }
            (*moduleSIMList)->size = 1;
            (*moduleSIMList)->code_no_list = lpa_calloc(1,sizeof(ty_code_No));
            if((*moduleSIMList)->code_no_list == NULL)
            {
                lpa_free(*moduleSIMList);
                *moduleSIMList = NULL;
                break;
            }
            (*moduleSIMList)->code_no_list[0].write_iccid = true;
            (*moduleSIMList)->code_no_list[0].use_iccid = true;
            (*moduleSIMList)->code_no_list[0].bind_iccid = iccid;
            ret = 0;           
        }
    }while(0);
    
    if(iccid)
    {
        lpa_free(iccid);
    }
    return ret;
}
void ty_LoadModuleSIMFree(ty_module_SIM *moduleSIMList)
{
    if(!moduleSIMList)
        return ;
    if(!moduleSIMList->code_no_list)
        return ;
    int i;
    for(i = 0 ; i < moduleSIMList->size ; i++ )
    {
        if(moduleSIMList->code_no_list[i].bind_iccid)
            lpa_free(moduleSIMList->code_no_list[i].bind_iccid);
        moduleSIMList->code_no_list[i].bind_iccid = NULL;
        
    }

    lpa_free(moduleSIMList->code_no_list);
    moduleSIMList->code_no_list = NULL;
    lpa_free(moduleSIMList);
    LOG("free esim OK\r\n");
}

#endif

void report_down_msg_free(ty_cloudmsg_report_down *down)
{
    if(down!= NULL && down->root != NULL)
    {
        cJSON_Delete(down->root);
        down->root = NULL;
        if(down->module_SIM.size && down->module_SIM.code_no_list)
        {
            lpa_free(down->module_SIM.code_no_list);
            LOG("code_no_list free\r\n");
        }
        if(down->module_SE_TSM.AID_list && down->module_SE_TSM.AID_list)
        {
            lpa_free(down->module_SE_TSM.AID_list);
            LOG("AID_list free\r\n");
        }
        memset(down,0,sizeof(ty_cloudmsg_report_down));
    }
}

static void ty_cloud_idle_timer_cb(void *para)
{
    LOG("ty_cloud_idle_timer_cb\r\n");
    //_maintask.idletimeout = true;
}

static void ty_cloud_idle_timer_start(MainTaskCtx *ctx)
{
    LOG("ty_cloud_idle_timer_start\r\n");
    //ty_timer_start(&ctx->idletimer);
}

static void ty_cloud_idle_timer_stop(MainTaskCtx *ctx)
{
    LOG("ty_cloud_idle_timer_stop\r\n");
    //ty_timer_stop(&ctx->idletimer);
}

static void ty_cloud_idle_timer_restart(MainTaskCtx *ctx)
{
    LOG("ty_cloud_idle_timer_restart\r\n");
    //ty_timer_reset(&ctx->idletimer);
    ctx->idletimeout = false;
}

static int ty_mqtt_msg_cb(int msgID,const char* topic,int topicLen,int msgLen,const char *message)
{
    int i,ret;
    #ifdef MQTT_MSG_DEBUG
    //LOG("[%s]",topic);
    LOG("%s %d %d %d %d\r\n",__FUNCTION__,msgID,topicLen,msgLen,strlen(message));
    #endif

    ty_cloud_idle_timer_restart(&_maintask);
    
    ty_cloudmsg_task_down task = {0};
    ty_queue_msg msg = {0};
    char *data = NULL;
    LOG("%s  Enter\r\n",__FUNCTION__);
    do{
        data = lpa_malloc(msgLen+1);
        if(!data){
            LOG("%s lpa_malloc fail\r\n",__FUNCTION__);
            break;
        }
        
        memset(data,0,msgLen+1);
        memcpy(data, message, msgLen);
        data[msgLen] = 0;
        ty_printf_hexbuf (data,msgLen);

        //LOG("%s rpd=%s %d \r\n",__FUNCTION__,_maintask.topic_reportdown,strlen(_maintask.topic_reportdown));
        if((strlen(_maintask.topic_reportdown) == topicLen) &&
            (0 == strncmp(_maintask.topic_reportdown,topic,topicLen)))
        {
            LOG(" %s %d topic_reportdown \r\n",__FUNCTION__,__LINE__);
            memset(&_maintask.down,0,sizeof(ty_cloudmsg_report_down));
            ret = ty_cloud_report_down_parser (data, &_maintask.down);
            LOG(" %s %d report_down_parser err=%d\r\n",__FUNCTION__,__LINE__,ret);
            //ty_semaphore_give(&_maintask.reportdown_sem);
            _maintask.report_reply = true;
            //数据未做处理，直接释放下行数据
            report_down_msg_free (&_maintask.down);
            break;
        }
        //LOG("%s tkd=%s %d \r\n",__FUNCTION__,_maintask.topic_taskdown,strlen(_maintask.topic_taskdown));
        if((strlen(_maintask.topic_taskdown) == topicLen )&&
            (0 == strncmp(_maintask.topic_taskdown,topic,topicLen)))
        {
            LOG(" %s %d topic_taskdown \r\n",__FUNCTION__,__LINE__);
            memset(&task,0,sizeof(task));
            ret = ty_cloud_task_down_parser (data, &task);
            if(ret != 0  || task.root == NULL)
            {
                LOG("%s parser task down fail,error=%d\r\n",__FUNCTION__,ret);
                break;
            }
            else
                LOG("%s parser task down OK\r\n",__FUNCTION__);
            msg.type = E_QueueMsg_Cloud;
            msg.msg.cloudmsg = task;
            ty_queue_task_put_msg (&msg);
            break;
        }
        LOG(" %s %d topic err %s\r\n",__FUNCTION__,__LINE__,topic);
    }while(0);
    if(data)
        lpa_free(data);
    LOG("%s  Leave\r\n",__FUNCTION__);
    return 0;
}

static int ty_cloud_mqtt_init(MainTaskCtx *ctx)
{
    int i ,ret;
    char deviceSecret [64] = {0};
    ty_mqtt_cnt_info info = {0};

    ty_mqtt_dev_reg_info dev = {0};
    ty_module_inf mdinfo = {0};
    ty_LoadModueInfo (&mdinfo);

    LOG("%s %s %s\r\n",__FUNCTION__,__DATE__,__TIME__);
    LOG("%s flash_status=%d\r\n",__FUNCTION__,flash_status);
    LOG("%s MQTT_PRODUCT_KEY=%s\r\n",__FUNCTION__,MQTT_PRODUCT_KEY);
    LOG("%s MQTT_PRODUCT_SECRET=%s\r\n",__FUNCTION__,MQTT_PRODUCT_SECRET);
    LOG("%s TY_CLOUD_MQTT_SERVER=%s\r\n",__FUNCTION__,TY_CLOUD_MQTT_SERVER);
    LOG("%s TY_CLOUD_PORT=%d\r\n",__FUNCTION__,TY_CLOUD_PORT);
    LOG("%s TY_CLOUD_HTTP_DEVREG_URL=%s\r\n",__FUNCTION__,TY_CLOUD_HTTP_DEVREG_URL);
    LOG("%s firmware_version=%s\r\n",__FUNCTION__,mdinfo.firmware_version);
    LOG("%s product_type=%s\r\n",__FUNCTION__,mdinfo.product_type);
    LOG("%s imei=%s\r\n",__FUNCTION__,mdinfo.bind_imei);
    LOG("RadioSignal=%d\r\n",ty_get_radio_signal_strength());
    
    memset(ctx->productKey,0,sizeof(ctx->productKey));
    memset(ctx->proudctSecret,0,sizeof(ctx->proudctSecret));
    memset(ctx->deviceName,0,sizeof(ctx->deviceName));
    
    ret = ty_LoadDeviceInfo (ctx->productKey,ctx->proudctSecret,
        ctx->deviceName);
    if(ret != 0)
    {
        LOG("%s %d fail\r\n",__FUNCTION__,__LINE__);
        return ret;
    }
    ret = ty_LoadDeviceSecret (deviceSecret);
    LOG("%s:%d :%s",__func__, __LINE__,deviceSecret);
    if(ret != 0)
    {
        char *cert = NULL;
        int cert_len;
        char url [128] = {0};
        char host[128] = {0};
        char uri[128] = {0};
        int port;
        LOG ("reg dev\r\n");
        do{
            ret = ty_LoadHttpCACert (&cert, &cert_len);
            if(ret != 0)
            {
                LOG("%s %d fail\r\n",__FUNCTION__,__LINE__);
                break;
            }
            ret = ty_LoadDeviceRegUrl (url,&port);
            if(ret != 0)
            {
                LOG("%s %d fail\r\n",__FUNCTION__,__LINE__);
                break;
            }
            ret = httpc_parser_url (url,host, sizeof(host),  uri,sizeof(uri));
            if(ret != 0)
            {
                LOG("%s %d httpc_parser_url fail=%d\r\n",__FUNCTION__,__LINE__,ret);
                break;
            }
            dev.cert = cert;
            dev.cert_len = cert_len;
            dev.productKey = ctx->productKey;
            dev.productSecret = ctx->proudctSecret;
            dev.deviceName = ctx->deviceName;
            dev.host = host;
            dev.uri  = uri;
            
            ty_mqtt_RegHttp (http_post_dynreg);
            ret = ty_mqtt_device_reg (&dev,deviceSecret);
            if(ret != 0)
            {
                LOG ("%s : %d reg dev fail ret = %d\r\n", __func__, __LINE__, ret);
                break;
            }
            ret = ty_SaveDeviceSecret (deviceSecret);
            if(ret != 0)
            {
                LOG ("save fail\r\n");
                break;
            }
            LOG ("reg dev success\r\n");
        }while(0);
    }
    if(ret != 0)
    {
        return ret;
    }
    LOG ("load dev sct success\r\n");
    ret = ty_LoadMqttCntInfo (&info);
    if(ret != 0)
    {
        LOG("%s %d fail\r\n",__FUNCTION__,__LINE__);
        return ret;
    }
    do
    {
        if(ctx->mqtt_handle != NULL)
        {
            LOG("%s %d mqtt_handle != NULL \r\n",__FUNCTION__,__LINE__);
            ty_mqtt_close (&ctx->mqtt_handle);
        }
        for(i = 0;i < MQTT_CONNECT_RETRY_CNT;i++)
        {
            ret = ty_mqtt_connect (&info, ty_mqtt_msg_cb,&ctx->mqtt_handle);
            if(0 != ret)
            {
                LOG("ty_mqtt_connect error=%d\r\n",ret);
                ty_sleep(1000);
            }
            else
            {
                LOG("ty_mqtt_connect success=%d\r\n",ret);
                break;
            }
        }
        
        if(0 != ret)
            break;

        ty_sleep(500);
        snprintf(ctx->topic_taskdown,TOPIC_MAX_LEN,TY_CLOUD_TASK_DOWN_TOPIC,
            ctx->productKey,ctx->deviceName);

        snprintf(ctx->topic_reportdown,TOPIC_MAX_LEN,TY_CLOUD_REPORT_DOWN_TOPIC,
            ctx->productKey,ctx->deviceName);

        snprintf(ctx->topic_taskup,TOPIC_MAX_LEN,TY_CLOUD_TASK_UP_TOPIC,
            ctx->productKey,ctx->deviceName);

        snprintf(ctx->topic_reportup,TOPIC_MAX_LEN,TY_CLOUD_REPORT_UP_TOPIC,
            ctx->productKey,ctx->deviceName);

        snprintf(ctx->topic_offline,TOPIC_MAX_LEN,TY_CLOUD_OFFLINE_TOPIC,
            ctx->productKey,ctx->deviceName);      

        for(i = 0;i< MQTT_SUB_RETRY_CNT;i++)
        {
            ret = ty_mqtt_sub (ctx->mqtt_handle,ctx->topic_taskdown, MQTT_SUB_QOS);
            if(0 != ret)
            {
                LOG("ty_mqtt_sub error=%d\r\n",ret);
                ty_sleep(500);
            }
            else
            {
                LOG("ty_mqtt_sub success=%s\r\n",ctx->topic_taskdown);
                break;
            }
        }
        
        if(0 != ret) break;
        
        for(i = 0;i< MQTT_SUB_RETRY_CNT;i++)
        {
            ret = ty_mqtt_sub (ctx->mqtt_handle,ctx->topic_reportdown, MQTT_SUB_QOS);
            if(0 != ret)
            {
                LOG("ty_mqtt_sub error=%d\r\n",ret);
                ty_sleep(500);
            }
            else
            {
                LOG("ty_mqtt_sub success=%s\r\n",ctx->topic_reportdown);
                break;
            }
        }
        //test
        /*
        char *sub_topic = "/sys/a1tyAZWeu86/864948040775004/thing/event/+/post_reply";
        ret = ty_mqtt_sub(sub_topic, 1);
        if (ret< 0) {
            LOG("ty_mqtt_sub failed, res: 0x%04X\n", ret);
            break;
        }
        
        LOG("ty_mqtt_sub success=%s\r\n",sub_topic);
        */
        
        if(0 != ret) break;

    }while(0);
    if(0 != ret)
    {
        ty_mqtt_close(&ctx->mqtt_handle);
        return ret;
    }
    return ret;
}

static int ty_cloud_mqtt_report_up(MainTaskCtx *ctx)
{
    int ret;
    char *json = NULL;
    int json_len = 0;
    char id[20] = {0};
    ty_cloudmsg_report_up report = {0};
    ty_module_inf mofule_inf = {0};
    ty_module_SIM *moduleSIMList = NULL;

    //执行下载任务时不做上报(查询码号信息、应用状态时会占用APDU通道)
    if(ctx->do_samtsm)
    {
        LOG("%s do_samtsm~~~~~~~~~~~\r\n",__FUNCTION__);
        return 0;
    }
    
    snprintf(id,sizeof(id),"%d",ty_rtc_get_sec (0));
    report.id = id;
    report.version = "1.0";
    

    /*
    ty_module_SIM moduleSIM = {0};
    moduleSIM.size = 2;
    moduleSIM.code_no_list = lpa_malloc(2*sizeof(ty_code_No));
    moduleSIM.code_no_list[0].bind_iccid = "test bi";
    moduleSIM.code_no_list[0].use_iccid = false;
    moduleSIM.code_no_list[0].write_iccid = true;
    moduleSIM.code_no_list[1].bind_iccid = "test bi 11";
    moduleSIM.code_no_list[1].use_iccid = false;
    moduleSIM.code_no_list[1].write_iccid = true;
    
    

    ty_module_SE_TSM moduleSETSM = {0};
    
    moduleSETSM.SEID = "seid";
    moduleSETSM.rest_space = 20;
    moduleSETSM.AID_size = 2;
    moduleSETSM.AID_list = lpa_malloc(2*sizeof(ty_AID));
    moduleSETSM.AID_list[0].app_aid = "appid";
    moduleSETSM.AID_list[0].app_status = "status 0";
    moduleSETSM.AID_list[0].app_version = "version 0";
    moduleSETSM.AID_list[0].app_life_cycle_state = "life 0";
    moduleSETSM.AID_list[1].app_aid = "appid 1";
    moduleSETSM.AID_list[1].app_status = "status 1";
    moduleSETSM.AID_list[1].app_version = "version 1";
    moduleSETSM.AID_list[1].app_life_cycle_state = "life1";
    report.module_SE_TSM = moduleSETSM;
    */
    ty_module_SE_TSM moduleSETSM = {0};
    
    do
    {
        ty_LoadModueInfo (&mofule_inf);
        report.module_inf = mofule_inf;
        ret = ty_LoadModuleSIM (&moduleSIMList);
        if(ret == 0 && moduleSIMList)
        {
            report.module_SIM = *moduleSIMList;
        }
        
        #ifdef TY_ESIM_TSM_SAM
        ret = load_module_samtsm_info (&moduleSETSM);
        if(ret != 0)
        {
            LOG("load_module_samtsm_info err=%d\r\n",ret);
            break;
        }
        #endif
        
        report.module_SE_TSM = moduleSETSM;
        LOG("%s %d \r\n",__FUNCTION__,__LINE__);
        ret = ty_cloud_report_up_build (&json, &json_len, &report);
        if(ret != 0)
        {
            LOG("ty_cloud_report_up_build err=%d\r\n",ret);
            break;
        }
        LOG("%s %d \r\n",__FUNCTION__,__LINE__);
        if(json != NULL && json_len > 0)
        {
            LOG("report_up msg build OK\r\n");
            ret = ty_mqtt_pub(ctx->mqtt_handle,ctx->topic_reportup,MQTT_PUB_QOS,0,json);
            if(ret == 0)
            {
                LOG("report_up pub ok=%d\r\n",ret);
            }
        }
        else
        {
            LOG("report_up msg build fail!!!\r\n");
        }
    }while(0);
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    if(moduleSIMList)
        ty_LoadModuleSIMFree (moduleSIMList);
    moduleSIMList = NULL;
#ifdef TY_ESIM_TSM_SAM
    free_module_samtsm_info (&moduleSETSM);
#endif
    if(json)
        lpa_free(json);
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    return ret;
}

static int ty_cloud_mqtt_wait_report_down(MainTaskCtx *ctx)
{
    int ret = 0;
    LOG("%s Enter %d \r\n",__FUNCTION__,__LINE__);
    uint32_t sec = ty_get_tick_ms ()/1000;
    do
    {
        if(ty_get_tick_ms ()/1000 - sec > WAIT_REPORTDOWN_RESPONSE_MAX)
        {
            LOG ("no report down\r\n");
            ret = -1;
            break;
        }
        ty_mqtt_yield (ctx->mqtt_handle,1000);
        ctx->mqtt_yield_s = ty_get_tick_ms ()/1000;
        if(_maintask.report_reply)
        {
            _maintask.report_reply  = false;
            LOG("report reply msg recviced!\r\n");
            LOG ("recviced report down\r\n");
            break;
        }
        /*
        while ((ret = ty_semaphore_take(&ctx->reportdown_sem, 1000)) != 0)
        {
            LOG("reportdown_sem timeout\n");
            //return -1;
        }
        */
    }
    while(1);
    
    //if(ctx->down.root)
    //{
    //    LOG("report down code=[%s]\r\n",ctx->down.code);
    //    report_down_msg_free (&ctx->down);
    //}
    LOG("%s Leave %d \r\n",__FUNCTION__,__LINE__);
    return ret;
}


static int ty_cloud_mqtt_wait_task_down(MainTaskCtx *ctx)
{
    int rc;
    static int  signalp = 0; 
    if(ty_get_tick_ms ()/1000 - signalp > SIGNAL_PERIOD_LOG_TIME)
    {
        signalp = ty_get_tick_ms ()/1000;
        LOG("RadioSignal=%d\r\n",ty_get_radio_signal_strength ());
    }
    
    rc = ty_mqtt_yield (ctx->mqtt_handle,1000);
    if(rc != 0)
	{
		LOG("ty_mqtt_yield fail!\r\n");
		LOG("RadioSignal=%d\r\n",ty_get_radio_signal_strength ());
	}
    if(1 != ty_mqtt_status (ctx->mqtt_handle))
    {
        LOG("mqtt disconneted!\r\n");
		LOG("RadioSignal=%d\r\n",ty_get_radio_signal_strength ());
        ty_mqtt_close(&ctx->mqtt_handle);
        return -1;
    }
    
    if(ty_get_tick_ms ()/1000 - ctx->mqtt_yield_s > MQTT_YIELD_PERIOD_LOG_TIME)
    {
        LOG("WARNNIG ,no  ty_mqtt_yield ~~~~%ds~~~~~~~~~\r\n",ty_get_tick_ms ()/1000 - ctx->mqtt_yield_s);
    }
    else
    {
        //LOG("ty_mqtt_yield ~~~~%ds~~~~~~~~~\r\n",ty_get_tick_ms ()/1000 - lastSec);
    }
    ctx->mqtt_yield_s = ty_get_tick_ms ()/1000;
    ty_sleep(100);
    return 0;
}

static int ty_cloud_cache_up_msg_pub(MainTaskCtx *ctx)
{
    int ret = 0;
    if(ctx->cache_msg.type == E_QueueMsg_Local &&
      (ctx->cache_msg.msg.localmsg.type == E_LocalMsg_TaskUP || 
      ctx->cache_msg.msg.localmsg.type == E_LocalMsg_FotaTaskUP))
    {
      LOG("cached localmsg.type=%d\r\n",ctx->cache_msg.msg.localmsg.type);
      ret = ty_mqtt_pub(ctx->mqtt_handle,ctx->topic_taskup,MQTT_PUB_QOS,0,ctx->cache_msg.msg.localmsg.data);
      if(ret == 0)
      {
	  	  LOG("topic_task pub ok=%d\r\n",ret);
          lpa_free(ctx->cache_msg.msg.localmsg.data);
          if(ctx->cache_msg.msg.localmsg.type == E_LocalMsg_FotaTaskUP)
          {
                LOG("ty_EraseFOTAInfo\r\n");
                ty_EraseFOTAInfo ();
          }
          memset(&ctx->cache_msg,0,sizeof(ctx->cache_msg));
      }
      else
      {
          //上传失败清空FOTA缓存，下次连接成功后再从FLASH中读取上报，规避重复上报
          if(ctx->cache_msg.msg.localmsg.type == E_LocalMsg_FotaTaskUP)
          {
             LOG("pub err,clear fota task up msg\r\n");
             memset(&ctx->cache_msg,0,sizeof(ctx->cache_msg));
          }
          LOG("task up msg pub error=%d\r\n",ret);
          return -1;
      }
    }

    //处理AT触发的全量上报
    if(ctx->cache_msg.type == E_QueueMsg_Local &&
      (ctx->cache_msg.msg.localmsg.type == E_LocalMsg_AtReportUP))
    {
      LOG(" AtReportUP \r\n");
      ret = ty_cloud_mqtt_report_up(ctx);
      if(ret == 0)
      {
	  	  LOG("AtReportUP pub ok=%d\r\n",ret);
          memset(&ctx->cache_msg,0,sizeof(ctx->cache_msg));
      }
      else
      {
          LOG("AtReportUP pub error=%d\r\n",ret);
          return -1;
      }
    }
    //test
    if(ctx->cache_msg.type == E_QueueMsg_Local &&
      (ctx->cache_msg.msg.localmsg.type == E_LocalMsg_TestUP))
    {
      LOG(" TestUP \r\n");
      if(!ctx->cache_msg.msg.localmsg.data)
        return 0;
      ret = ty_mqtt_pub(ctx->mqtt_handle,ctx->topic_taskup,MQTT_PUB_QOS,0,ctx->cache_msg.msg.localmsg.data);
      lpa_free(ctx->cache_msg.msg.localmsg.data);
      memset(&ctx->cache_msg,0,sizeof(ctx->cache_msg));
      if(ret == 0)
      {
	  	  LOG("TestUP pub ok=%d\r\n",ret);
      }
      else
      {
          LOG("TestUP pub error=%d\r\n",ret);
          return -1;
      }
    }
    
    return ret;
}

static int ty_cloud_offline_msg_pub(MainTaskCtx *ctx)
{
    int ret,json_len;
    char *json = NULL;
    char id[20] = {0};
    snprintf(id,sizeof(id),"%d",ty_rtc_get_sec (0));
    ty_cloudmsg_offline_up offline = {0};
    offline.id = id;
    offline.version = "1.0";
    offline.type = 1;
    
    ret = ty_cloud_offline_up_build (&json,&json_len, &offline);
    if(ret != 0 || json == NULL)
    {
        LOG("offline msg ty_cloud_offline_up_build error=%d\r\n",ret);
        return ret;
    }
    
    ret = ty_mqtt_pub(ctx->mqtt_handle,ctx->topic_offline,MQTT_PUB_QOS,0,json);
    if(ret == 0)
    {
        LOG("offline msg pub ok=%d\r\n",ret);
    }
    else
    {
        LOG("offline msg pub error=%d\r\n",ret);
    }
    lpa_free(json);
    json = NULL;
    return ret;
}

static int ty_cloud_mqtt_wait_task_up(MainTaskCtx *ctx)
{
  int ret;
  //是否缓存消息需要上报
  ret = ty_cloud_cache_up_msg_pub (ctx);
  if(ret != 0)
    return ret;
  //20201013 全部上报消息处理完后再切换到下一个状态
  do
  {
      //取出所以需要上报的消息
      if(!ty_get_msg_que (ctx->msgQueue, &ctx->cache_msg, 1000))
      {
        //LOG("got task up msg timeout\r\n");
        ret = MAIN_TASK_QUEUE_TIMEOUT;
        break;
      }
      else
      {
        LOG("got task up msg OK\r\n");
        ret = ty_cloud_cache_up_msg_pub (ctx);
      }
  }while(ret == 0); //如果失败退出上报流程
  return ret;
}

int ty_main_task_put_taskup_msg(ty_queue_msg *msg)
{
    if(!_maintask.msgQueue)
    {
        LOG("%s msgQueue NULL\r\n",__FUNCTION__);
        return -1;
    }
   
    if(!ty_put_msg_que(_maintask.msgQueue, msg,8))
    {
        LOG("%s ty_main_task_put_taskup_msg FAIL!\r\n",__FUNCTION__);
        return -2;
    }
    //延时，让出时间片给主任务处理上行消息
    ty_sleep(500);
    return 0;
}


int ty_main_task_fota_check ()
{
    ty_queue_msg msg = {0};
    msg.type = E_QueueMsg_Local;
    msg.msg.localmsg.data = NULL;
    msg.msg.localmsg.type = E_LocalMsg_FotaCheck;

    LOG("%s E_LocalMsg_FotaCheck  \r\n",__FUNCTION__);
    return ty_queue_task_put_msg(&msg);
}


void ty_main_task_fota_notify (bool fota)
{
    _maintask.fota = fota;
}

void ty_main_task_tsm_notify (bool tsm)
{
    _maintask.do_samtsm = tsm;
}

//FOTA升级时等待任务上报超时时间，超时后直接重启开始升级
void ty_main_fota_reboot_notify (bool reboot)
{
    int waitsec = WAIT_TASKUP_TIMEOUT;
    while(waitsec--)
    {
        ty_sleep(1000);
        LOG("check task up msg !!!\r\n");
        if(_maintask.cache_msg.type == E_QueueMsg_Local)
        {
            LOG("wait task up msg done!!!\r\n");
        }
        else
        {
            LOG("no task up msg !!!\r\n");
            break;
        }
    }
}

static int ty_cloud_task_go_sleep(MainTaskCtx *ctx)
{
    int ret = 0;
    while(ctx->do_samtsm || ctx->fota )
    {
        LOG("%s do_samtsm=%d,fota=%d\n",__FUNCTION__,ctx->do_samtsm,ctx->fota);
        ty_sleep(1000);
    }

    //清除缓存和队列中的数据
    do
    {
        ret = ty_cloud_mqtt_wait_task_up (ctx);
        LOG("%s clear task up ret=%d\r\n",__FUNCTION__,ret);
        //上报失败
        if(ret < 0 && ctx->cache_msg.type == E_QueueMsg_Local && ctx->cache_msg.msg.localmsg.data)
        {
            lpa_free(ctx->cache_msg.msg.localmsg.data);
            memset(&ctx->cache_msg,0,sizeof(ctx->cache_msg));
        }
    }while(ret != MAIN_TASK_QUEUE_TIMEOUT);
    
    if(1 == ty_mqtt_status (ctx->mqtt_handle))
    {
        ty_cloud_offline_msg_pub (ctx);
        
    }
    else
    {
        LOG("mqtt not connected\r\n");
    }
    ty_mqtt_close(&ctx->mqtt_handle);
    return 0;
}

void ty_main_task_loop(void *args)
{
    int ret;
    MainTaskCtx *ctx = (MainTaskCtx *)args;
    if(ctx == NULL)
        return ;

#ifdef MAIN_TASK_TEST_LOOP
//for test 
 loop:
#endif
    memset(ctx,0,sizeof(MainTaskCtx));
    ty_work_lock(1);
    ctx->msgQueue = ty_create_msg_Que(TY_QUEUE_TASK_MSGQ_SIZE, sizeof(ty_queue_msg));
    LOG("ty_semaphore_create OK\r\n");
    ctx->status = E_MainTaskInit;
    ctx->idletimeout = false;
    ctx->do_samtsm = false;
    ctx->fota = false;
    LOG("ty_timer_create OK\r\n");
    //ty_cloud_ext_notify (HSM_TASK_START);
    do
    {
        switch(ctx->status)
        {
            case E_MainTaskInit:
                ctx->status = E_MainTaskCereg;
                break;
            case E_MainTaskCereg:
                if(ctx->idletimeout)
                {
                    ctx->status = E_MainTaskSleep;
                    LOG("E_MainTaskCereg idletimeout,go Sleep\r\n");
                    break;
                }
                if(1 != at_creg())
                {
                    ty_sleep(3000);
                    break;
                }
                LOG("at_creg OK\r\n");
                ctx->status = E_MainTaskCgpAddr;
                break;
             case E_MainTaskCgpAddr:
                if(ty_wait_tcpip_ok(2*60) == 0)
                {
                    //LOG("wait tcpip fail!\r\n");
                    ty_sleep(3000);
                    break;
                }
                LOG("wait tcpip OK\r\n");
                ctx->status = E_MainTaskDisconnect;
                //ty_log_uart_set_baud (115200);
                break;
             case E_MainTaskDisconnect:
                LOG("%s : %d", __func__, __LINE__);
                ret = ty_cloud_mqtt_init (ctx);
                if(ret != 0)
                {
                    LOG("ty_cloud_mqtt_init fail!\r\n");
                    ctx->status = E_MainTaskCereg;
                    ty_sleep(5000);
                    break;
                }
                LOG("ty_cloud_mqtt_init OK\r\n");
                #ifdef MAIN_TASK_TEST_LOOP
                //for test 
                //ty_mqtt_close(&ctx->mqtt_handle);
                //goto unlock;
                #endif
                
                ctx->status = E_MainTaskConnected;
                break;
             case E_MainTaskConnected:
                ctx->status = E_MainTaskReportUp;
                ty_cloud_ext_notify (HSM_TASK_START);
                if(!ctx->fota)
                {
                    ret = ty_main_task_fota_check ();//检查FOTA状态升级
                    if(ret == 0)
                        ty_sleep(2000);
                }
                else
                {
                    LOG("fota running!!!\r\n");
                }
                break;
             case E_MainTaskReportUp:
                ret = ty_cloud_mqtt_report_up (ctx);
                if(ret != 0)
                {
                    LOG("ty_cloud_mqtt_report_up fail!\r\n");
                    ty_sleep(3000);
                    if(ctx->mqtt_handle != NULL)
                    {
                        LOG("ty_cloud_mqtt_report_up fail ,disconnect mqtt \r\n");
                        ty_mqtt_close(&ctx->mqtt_handle);
                    }
                    ctx->status = E_MainTaskCereg;
                    break;
                }
                ctx->status = E_MainTaskWaitReportDown;
                break;
             case E_MainTaskWaitReportDown:
                ret = ty_cloud_mqtt_wait_report_down(ctx);
                if(ret != 0)
                {
                    if(ctx->connect_err_cnt++ > MQTT_REPORT_DOWN_ERR_CNT)
                    {
                        if(ctx->mqtt_handle != NULL)
                        {
                            LOG("ty_cloud_mqtt_wait_report_down fail ,disconnect mqtt \r\n");
                            ty_mqtt_close(&ctx->mqtt_handle);
                        }
                        ctx->status = E_MainTaskCereg;
                        break;
                    }
                    ctx->status = E_MainTaskReportUp;
                    LOG("ty_cloud_mqtt_wait_report_down fail!\r\n");
                    ty_sleep(3000);
                    break;
                }
                ctx->connect_err_cnt = 0;
                ty_cloud_idle_timer_start(ctx);
                ctx->status = E_MainTaskWaitTaskUP;
                break;
              case E_MainTaskWaitTaskUP:
                #ifdef MAIN_TASK_TEST_LOOP
                //for test 
                ty_mqtt_close(&ctx->mqtt_handle);
                goto unlock;
                #endif
                
                ret = ty_cloud_mqtt_wait_task_up(ctx);
                if(!(ret == 0 || ret == MAIN_TASK_QUEUE_TIMEOUT))
                {
                    LOG("ty_cloud_mqtt_wait_task_up fail!\r\n");
                    ctx->status = E_MainTaskCereg;
                    break;
                }
                ctx->status = E_MainTaskWaitTaskDown;
                break;
              case E_MainTaskWaitTaskDown:
                ret = ty_cloud_mqtt_wait_task_down(ctx);
                if(ret != 0)
                {
                    LOG("ty_cloud_mqtt_wait_task_down fail!\r\n");
                    ty_sleep(1000);
                    ctx->status = E_MainTaskCereg;
                    break;
                }
                //优先响应FOTA升级 
                if(ctx->fota)
                {
                    ctx->status = E_MainTaskFota;
                    break;
                }
                if(ctx->idletimeout)
                    ctx->status = E_MainTaskSleep;
                else
                    ctx->status = E_MainTaskWaitTaskUP;
                break;

               case E_MainTaskFota:
                    LOG("E_MainTaskFota \r\n");
                    if(!ctx->fota)//升级异常情况，退出升级，重新恢复业务处理
                    {
                        LOG("E_MainTaskFota exit\r\n");
                        ctx->status = E_MainTaskWaitTaskUP;
                        break;
                    }
                    else //升级中只检查是否需要上报任务结果，其他下行任务不处理!!!
                    {
                        LOG("E_MainTaskFota ty_cloud_mqtt_wait_task_up\r\n");
                        ret = ty_cloud_mqtt_wait_task_up(ctx);
                        LOG("E_MainTaskFota ty_cloud_mqtt_wait_task_up ret=%d\r\n",ret);
                    }
                    ty_sleep(5000);
                    break;
               case E_MainTaskSleep:
                    ret = ty_cloud_task_go_sleep (ctx);
                    goto unlock;
                    break;

        }
    }while(1);
unlock:
    ctx->reportdown_sem = -1;
    ctx->idletimer = -1;
    ty_delete_msg_que (ctx->msgQueue);
    ctx->msgQueue = NULL;
    LOG("ty_work_unlock  \r\n");
    ty_work_unlock();
    ctx->status = E_MainTaskExit;
    #ifdef NB12_PLAT
    LOG("NB12_PLAT   task delete\r\n");
    ty_task_delete((void *)&g_lpa_app_Handle);
    g_lpa_app_Handle = -1;
    ty_cloud_ext_notify (HSM_TASK_STOP);
    LOG("%s Task Delete out\r\n",__FUNCTION__);
    #elif defined(SS11_PLAT)
    #ifdef MAIN_TASK_TEST_LOOP
    //for test
    goto loop;
    #endif
    while(1)
    {
        LOG("SS11_PLAT  wait to delete\r\n");
        ty_sleep(1000);
    }
    #else // other platform
    LOG("other  platform  ty_task_delete \r\n");
    ty_task_delete((void *)&g_lpa_app_Handle);
    g_lpa_app_Handle = -1;
    LOG("other  platform  ty_task_delete out\r\n");
    #endif
}

int ty_at_mqtt_report_up ()
{
    //使用队列方式消息通讯
    int ret;
    ty_queue_msg qmsg = {0};
    qmsg.type = E_QueueMsg_Local;
    qmsg.msg.localmsg.type = E_LocalMsg_AtReportUP;     
    qmsg.msg.localmsg.data = NULL;
    qmsg.msg.localmsg.msglen = 0;
    ret = ty_main_task_put_taskup_msg (&qmsg);
    if(ret != 0)
    {
        LOG("%s put queue err\r\n",__FUNCTION__);
        return ret;
    }
    else
    {
        LOG("%s  put queue OK\r\n",__FUNCTION__);
    }
    //return ty_cloud_mqtt_report_up (&_maintask);
}

int ty_at_mqtt_offline()
{
    //进入睡眠状态
    _maintask.idletimeout = true;
    #ifdef SS11_PLAT
    LOG("ty_at_mqtt_offline, task delete \r\n");
    while(_maintask.status != E_MainTaskExit)
    {
        ty_sleep(500);
    }
    if(g_lpa_app_Handle == -1)
    {
        LOG("SS11_PLAT task deleted\r\n");
        return -1;
    }
    
    LOG("SS11_PLAT   task delete\r\n");
    ty_task_delete((void *)&g_lpa_app_Handle);
    g_lpa_app_Handle = -1;
    ty_cloud_ext_notify (HSM_TASK_STOP);
    #endif
    LOG("%s\r\n",__FUNCTION__);
    return 0;
}

int ty_at_mqtt_task_delete()
{
    if(g_lpa_app_Handle == -1)
    {
        LOG("SS11_PLAT task deleted\r\n");
        return -1;
    }
    ty_task_delete((void *)&g_lpa_app_Handle);
    g_lpa_app_Handle = -1;
    LOG("%s\r\n",__FUNCTION__);
    return 0;
}


int ty_at_mqtt_resume()
{
    int ret =  -1;
    if(_maintask.status == E_MainTaskExit )
    {
        ret = ty_task_create((ty_task_func_t)ty_main_task_loop,"ty_main_task",0x2000,(void*)&_maintask,10,(void *)&g_lpa_app_Handle);
    }
    else
    {
        LOG("%s ty_queue_task running!!! _maintask.status=%d g_lpa_app_Handle=%d\r\n",__FUNCTION__,g_lpa_app_Handle);
    }
    return ret;
}

void main_report_up_test()
{
    _maintask.mqtt_handle = NULL;
    ty_cloud_mqtt_report_up (&_maintask);
}

//can call by  user_app_init
void ty_main_task_entry()
{
//  ty_at_test_entry ();
    LOG("%s %d", __func__, __LINE__);
    ty_task_create((ty_task_func_t)ty_queue_task_loop,"ty_queue_task",0x2000,NULL,11,(void *)&g_lpa_app_QueueHandle);
    ty_task_create((ty_task_func_t)ty_main_task_loop,"ty_main_task",0x2000,(void*)&_maintask,10,(void *)&g_lpa_app_Handle);


}

