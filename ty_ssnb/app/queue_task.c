#include "ty_os_adpt.h"
#include "ty_mqtt_basic_info.h"
#include "ty_ss_cloud.h"
#include "queue_task.h"
#include "ty_fota_adpt.h"

#ifdef TY_ESIM_TSM_SAM
#include "ty_esim.h"
#endif

typedef struct
{
    void *msgQueue;
}T_QueueTaskCtx;

T_QueueTaskCtx _queuetaskctx = {0};


char * _byte2str (char byte)
{
    char *dup = lpa_calloc(1,10);
    snprintf(dup,10,"%d",(int)byte);
    return dup;
}


char * _strdup (const char *str)
{
    if(!str)
        return NULL;
    char *dup = lpa_calloc(1,(strlen(str)+1));
    memcpy(dup,str,strlen(str));
    return dup;
}

int put_fota_task_up_msg(ty_task_fota_wrapper *fota,int fota_status_code)
{
    int ret;
    char *json = NULL;
    int json_len = 0;
    ty_cloudmsg_task_up taskup = {0};
    char taskID[33] = {0};
    ty_queue_msg qmsg = {0};
    ty_module_inf info = {0};
    
    ty_LoadModueInfo (&info);
    char errorcode[10] = {0};
    memcpy(taskID,fota->ID,32);
    taskup.id = taskID;
    taskup.version = fota->Ver;
    snprintf(errorcode,sizeof(errorcode),"%d",fota_status_code);
    //?????????????,??????????
    taskup.code = (fota_status_code == FOTA_UPD_OK) || (fota_status_code == FOTA_UPGRADED) ? "200":errorcode;
    taskup.task_type = e_cloud_task_fota;
    taskup.task_no = fota->taskNo;
    taskup.fota_msg = true;
    taskup.msg.module_inf = info;
    

    ret = ty_cloud_task_up_build (&json,&json_len,&taskup);
    if(ret !=0 || json == NULL || json_len == 0)
    {
        LOG("%s ty_cloud_task_up_build err=%d\r\n",__FUNCTION__,ret);
        return -1;
    }
    qmsg.type = E_QueueMsg_Local;
    qmsg.msg.localmsg.type = E_LocalMsg_FotaTaskUP;
    qmsg.msg.localmsg.data = json;
    qmsg.msg.localmsg.msglen = json_len;
    ret = ty_main_task_put_taskup_msg (&qmsg);
    if(ret != 0)
    {
        if(json)
            lpa_free(json);
        json = NULL;
        LOG("%s ty_main_task_put_taskup_msg err\r\n",__FUNCTION__);
        return ret;
    }
    else
    {
        LOG("%s E_LocalMsg_FotaTaskUP put OK\n",__FUNCTION__);
    }
    return ret;
}

int put_samtsm_task_up_msg(const char *ID,const char *ver,
    const char *taskNo,int task_type,int samtsm_status_code)
{
    int i;
    int ret;
    char *json = NULL;
    int json_len = 0;
    ty_cloudmsg_task_up taskup = {0};
    ty_queue_msg qmsg = {0};
    Aid_List *list = NULL;
    char seid[65] = {0};
    int  space = 0;
    #ifdef TY_ESIM_TSM_SAM
    ret = Ty_getSeid(seid);
    if(ret != 0)
    {
        LOG("%s Ty_getSeid err\r\n",__FUNCTION__);
        return ret;
    }
    
    ret = Ty_getRestSpace(&space);
    LOG("%s Ty_getRestSpace ret = %d , space = %d\r\n",__FUNCTION__,ret , space);
    if (ret < 0 )
    {
        LOG("%s Ty_getRestSpace err\r\n",__FUNCTION__);
        return ret;
    }
    #endif
    

    ret =  ty_LoadAppletInfoList (&list);
    if(ret != 0 || list == NULL)
    {
        LOG("%s ty_LoadAppletInfoList err\r\n",__FUNCTION__);
        return ret;
    }
    
    taskup.id = ID;
    taskup.version = ver;
    taskup.code = (samtsm_status_code == 0) ? "200":"404";
    taskup.task_type = task_type;
    taskup.task_no = taskNo;
    taskup.sam_tsm_msg = true;
    taskup.msg.module_SE_TSM.SEID  = seid;
    taskup.msg.module_SE_TSM.AID_size = list->aid_size;
    taskup.msg.module_SE_TSM.rest_space = space;
  
    LOG("%s list->aid_size = %d\r\n",__FUNCTION__,list->aid_size);
    if(list->aid_size > 0)
    {
        taskup.msg.module_SE_TSM.AID_list = lpa_malloc(sizeof(ty_AID)*list->aid_size);
        if (NULL == taskup.msg.module_SE_TSM.AID_list){
            ret = -1;
            goto exit;
        }
        memset(taskup.msg.module_SE_TSM.AID_list, 0, sizeof(ty_AID)*list->aid_size);
        
        LOG("============== printf aidList ===========");
        for(i = 0;i < list->aid_size;i++)
        {
            taskup.msg.module_SE_TSM.AID_list[i].app_status = _byte2str (list->list[i].app_status);
            taskup.msg.module_SE_TSM.AID_list[i].app_life_cycle_state = _byte2str (list->list[i].app_life_cycle_state);
            taskup.msg.module_SE_TSM.AID_list[i].app_aid = _strdup (list->list[i].app_aid);
            taskup.msg.module_SE_TSM.AID_list[i].app_version = _strdup (list->list[i].app_version);
            LOG(" i = %d , aid = %s , version = %s \r\n", i , taskup.msg.module_SE_TSM.AID_list[i].app_aid,taskup.msg.module_SE_TSM.AID_list[i].app_version);
            LOG(" app_status = %s , app_life_cycle_state = %s \r\n",  taskup.msg.module_SE_TSM.AID_list[i].app_status,taskup.msg.module_SE_TSM.AID_list[i].app_life_cycle_state);
        }
    }
    
    ret = ty_cloud_task_up_build (&json,&json_len,&taskup);
    if(ret != 0 || json == NULL || json_len == 0)
    {
        ret = -2;
        LOG("%s ty_cloud_task_up_build err=%d\r\n",__FUNCTION__,ret);
        goto exit;
    }
    qmsg.type = E_QueueMsg_Local;
    qmsg.msg.localmsg.type = E_LocalMsg_TaskUP;
    qmsg.msg.localmsg.data = json;
    qmsg.msg.localmsg.msglen = json_len;
    ret = ty_main_task_put_taskup_msg (&qmsg);
    if(ret != 0)
    {
        if(json)
            lpa_free(json);
        json = NULL;
        LOG("%s ty_main_task_put_taskup_msg err\r\n",__FUNCTION__);
    }
    LOG("============== put taskup msg OK===========");
exit:
    if(list != NULL)
    {
        if (taskup.msg.module_SE_TSM.AID_list){
            for(i = 0;i < list->aid_size;i++)
            {
                lpa_free(taskup.msg.module_SE_TSM.AID_list[i].app_status); 
                lpa_free(taskup.msg.module_SE_TSM.AID_list[i].app_life_cycle_state);
                lpa_free(taskup.msg.module_SE_TSM.AID_list[i].app_aid ); 
                lpa_free(taskup.msg.module_SE_TSM.AID_list[i].app_version );
            }
            lpa_free(taskup.msg.module_SE_TSM.AID_list);
            taskup.msg.module_SE_TSM.AID_list = NULL;
        }
        ty_AppletInfoListFree (list);
        list = NULL;
    }
    return ret;
}

void ty_local_msg_done(void *ctx,ty_queue_msg *imsg)
{
    int ret ;
    ty_queue_msg msg = {0};
    ty_task_fota_wrapper nv_fota = {0};
    LOG("%s in \r\n",__FUNCTION__);
    if(ctx == NULL|| imsg == NULL)
    {
        LOG("ctx or imsg NULL\r\n");
        return ;
    }
   
    switch(imsg->msg.localmsg.type)
    {
        case E_LocalMsg_FotaCheck: //????
            ret = ty_CheckFota (&nv_fota);
            break;
          defaut:
            LOG("localmsg type unkown =%d\r\n",imsg->msg.localmsg.type);
            break;         
    }
    if(imsg->msg.localmsg.data)
    {
        LOG("localmsg done data=%s\r\n",imsg->msg.localmsg.data);
        lpa_free(imsg->msg.localmsg.data);
        imsg->msg.localmsg.data = NULL;        
    }
}

static int ty_cloud_msg_done(void *ctx,ty_queue_msg *msg)
{
    int ret = 0,i,mqtt_retVal = 0;
    int tasktype;
    
    uint32_t ms = ty_get_tick_ms();
    LOG("%s in \r\n",__FUNCTION__);
    
    if(ctx == NULL|| msg == NULL)
    {
        LOG("ctx or msg NULL\r\n");
        return -1;
    }
    tasktype = msg->msg.cloudmsg.task_type;
    LOG("task_type=%d taskNo=%s\r\n",tasktype,msg->msg.cloudmsg.task_No);
    switch(tasktype)
    {
        case e_cloud_task_fota:
            LOG("e_cloud_task_fota \r\n");
            //???????????
            ret = ty_MgrModuleFota (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version,
            msg->msg.cloudmsg.task_No,&msg->msg.cloudmsg.params.fota);
            break;
#ifdef TY_ESIM_TSM_SAM
        case e_cloud_task_se_publish:
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_se_publish \r\n");
            ret = ty_MgrModuleSE(&msg->msg.cloudmsg.params.sam ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);
            LOG("%s ty_MgrModuleSE ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);
            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);

            break;
        case e_cloud_task_se_active:
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_se_active \r\n");
            ret = ty_MgrModuleSE(&msg->msg.cloudmsg.params.sam ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);
            LOG("%s ty_MgrModuleSE ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);
            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);    
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);
            break;
        case e_cloud_task_tsm_download:    
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_tsm_download \r\n");

            ret = ty_MgrModuleTSM(&msg->msg.cloudmsg.params.tsm ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);
            LOG("%s ty_MgrModuleTSM ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);

            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);  
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);

            break;
        case e_cloud_task_tsm_personal:
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_tsm_personal \r\n");

            ret = ty_MgrModuleTSM(&msg->msg.cloudmsg.params.tsm ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);
            LOG("%s ty_MgrModuleTSM ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);
            
            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);
            break;
        case e_cloud_task_tsm_active:
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_tsm_active \r\n");

            ret = ty_MgrModuleTSM(&msg->msg.cloudmsg.params.tsm ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);
            LOG("%s ty_MgrModuleTSM ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);
                            
            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);     
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);
         
            break;
        case e_cloud_task_tsm_delete:
            ty_main_task_tsm_notify (true);
            LOG("e_cloud_task_tsm_delete \r\n");
            ret = ty_MgrModuleTSM(&msg->msg.cloudmsg.params.tsm ,msg->msg.cloudmsg.task_No , msg->msg.cloudmsg.task_type);      
            LOG("%s ty_MgrModuleTSM ret = [%d], %d \r\n",__FUNCTION__,ret,__LINE__);
            
            put_samtsm_task_up_msg (msg->msg.cloudmsg.id,msg->msg.cloudmsg.version, msg->msg.cloudmsg.task_No,
                msg->msg.cloudmsg.task_type,ret);            
            LOG("%s %d \r\n",__FUNCTION__,__LINE__);
            break;
#endif

        defaut:
            LOG("tasktype =%d unkown\r\n",tasktype);
            break;
    }
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    if(msg->msg.cloudmsg.root != NULL)
    {
        LOG("task down json delete\n");
        cJSON_Delete(msg->msg.cloudmsg.root);
        msg->msg.cloudmsg.root = NULL;
    }
    LOG("%s out ret=%d consume=%dms\r\n",__FUNCTION__,ret,ty_get_tick_ms() - ms);
    ty_main_task_tsm_notify (false);
    return ret;
}

int ty_queue_task_put_msg(ty_queue_msg *msg)
{
    if(!_queuetaskctx.msgQueue)
    {
        LOG("%s msgQueue NULL\r\n",__FUNCTION__);
        return -1;
    }
   LOG("%s : %d", __func__, __LINE__);
    if(!ty_put_msg_que(_queuetaskctx.msgQueue, msg,8))
    {
        LOG("%s ty_put_msg_que FAIL!\r\n",__FUNCTION__);
        return -2;
    }
    return 0;
}
void ty_queue_task_loop(void *args)
{
    ty_queue_msg msg = {0};  
    int ret = 0;
    LOG("%s %d", __func__, __LINE__);
    _queuetaskctx.msgQueue = ty_create_msg_Que(TY_QUEUE_TASK_MSGQ_SIZE, sizeof(ty_queue_msg));
   LOG("%s %d", __func__, __LINE__);
   
    if(!_queuetaskctx.msgQueue)
    {
        LOG("%s msgQueue NULL \r\n",__FUNCTION__);
        return ;
    }
    do
    {
//         LOG("%s %d", __func__, __LINE__);
        memset(&msg,0,sizeof(ty_queue_msg));
        if(!ty_get_msg_que (_queuetaskctx.msgQueue, &msg, 1000))
        {
//             LOG("%s %d", __func__, __LINE__);
            ty_sleep(100);
            continue;
        }
        LOG("%s : %d type=%d  \r\n",__FUNCTION__, __LINE__, msg.type);
        switch(msg.type)
        {
            case E_QueueMsg_Local:
                LOG("%s E_QueueMsg_Local \r\n",__FUNCTION__);
                ty_local_msg_done(&_queuetaskctx,&msg);
                break;
            case E_QueueMsg_Cloud:
                LOG("%s E_QueueMsg_Cloud \r\n",__FUNCTION__);
                ret = ty_cloud_msg_done(&_queuetaskctx,&msg);
                break;
            default:
                LOG("%s type=%d unkonwn  \r\n",__FUNCTION__,msg.type);
                continue;
        }
     }while(1);
}

