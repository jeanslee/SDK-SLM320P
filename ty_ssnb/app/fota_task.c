#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http.h"
#include "md5.h"
#include "sha256.h"
#include "ty_fota.h"
#include "ty_ss_cloud.h"
#include "ty_devinfo.h"
#include "ty_os_adpt.h"
#include "ty_fota_adpt.h"


#ifdef ALIOS_HTTP
int  FOTA_PACKET_SZ =  1024;
uint8_t rsp_buf[RSP_BUF_SIZE] = {0};
uint8_t req_buf[REQ_BUF_SIZE] = {0};
#else
int  FOTA_PACKET_SZ =  FOTA_DOWNLOAD_PKG_SIZE;
#endif

typedef struct
{
    uint32_t task_Handle;
    uint32_t task_fail_sem;
    void *fota_task_queue;
    bool fota_task_exit;
}FotaTaskCtx;

static FotaTaskCtx _fotatask = {0,.task_Handle = -1,.task_fail_sem = -1,.fota_task_queue = NULL,.fota_task_exit = false};

extern ty_cloud_nv_t   g_ty_cloud_nv;

static int _ty_mgr_module_fota(const char *ID,const char* ver, const char *taskNo,ty_task_fota *fota,bool savenvflash);

#ifdef ALIOS_HTTP
/* send http get request */
int http_get_fota(const char *host,const char* fotaURI,int put,int sigalg,const char *sig)
{
    int fd;
    static char hdr[HTTP_UP_HDR_SIZE] = {0};
    static char rang[128] = {0};
    unsigned char hash[32] = {0};
    unsigned char hashDld[32] = {0};
    unsigned char hashStr[65] = {0};
    int32_t ret;
    http_rsp_info_t rsp_info;
    httpc_handle_t httpc_handle = 0;
    httpc_connection_t settings;

    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_client_initialize();
    MD5_CTX md5;
    iot_sha256_context context;
     
    MD5Init(&md5);  
    
    /* perform inner SHA */
    utils_sha256_init(&context);                                      /* init context for 1st pass */
    utils_sha256_starts(&context);    

    int isigalg = sigalg;
    
    UINT32 TickCount =  ty_get_tick_ms ();
    /* create socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG("alloc socket fd fail\r\n");
        goto exit;
    }
    
    memset(&settings, 0, sizeof(settings));
    settings.socket       = fd;
    settings.server_name  = host;
    settings.req_buf      = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    settings.keep_alive = 1;
    settings.ca_cert = ty_http_ca_cert;

    
    /* httpc initialization */
    if ((httpc_handle = httpc_init(&settings)) == 0) {
        LOG("http session init fail\r\n");
        close(fd);
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    int fw_offset = 0;
    int fwTotalSz = 0;
    int fwsz;

    while(1)
    {
        retry:
        memset(rang,0,sizeof(rang));
        snprintf(rang,sizeof(rang),"bytes=%d-%d",fw_offset,fw_offset+FOTA_PACKET_SZ-1);
        memset(hdr,0,sizeof(hdr));
        if ((httpc_construct_header(hdr, HTTP_UP_HDR_SIZE, "Range",rang )) < 0) {
            LOG("http construct header fail\r\n");
            goto exit;
        }
            int start = 0,end = 0;
            //Range: bytes=60322-60833
            ty_cloud_ext_notify (FIRMWARE_FOTA_DOWNLOADING);
            /* send get request */
            if ((httpc_send_request(httpc_handle, HTTP_GET, fotaURI, hdr,
                                    "application/json", NULL, 0)) != HTTP_SUCCESS) {
                LOG("httpc_send_request fail\n");
                goto exit;
            }
            /* get response */
            memset(&rsp_info,0,sizeof(rsp_info));
            memset(rsp_buf,0,sizeof(rsp_buf));
            if ((httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info,
                                     20000)) < 0) {
                LOG("httpc_recv_response fail\r\n");
                goto exit;
            }

            char *prefix = strstr(rsp_buf,"Content-Range: bytes ");
            if(prefix != NULL)
            {
               sscanf(prefix+strlen("Content-Range: bytes "),"%d-%d/%d\r\n",&start,&end,&fwTotalSz);
               LOG("%d %d fwTotalSz=%d\n",start,end,fwTotalSz);
               if( start != fw_offset)
               {
                    LOG("download err,start=%d fw_offset=%d \r\n",start,fw_offset);
                    //goto retry;
                    break;
               }
               fwsz = end-start+1;
               fw_offset += fwsz;
               if(1 == isigalg)
                    MD5Update(&md5,(const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz);
               else utils_sha256_update(&context, (const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz);

               if((fwsz < FOTA_PACKET_SZ || fw_offset == fwTotalSz))
               {    
                    if(1 == isigalg)
                        MD5Final(&md5,hash);
                    else
                        utils_sha256_finish(&context,hash);
                    int destlen = asc2hex (hashDld, sig, strlen(sig));

                    bytes2hexstr (hash, sizeof(hash),hashStr,sizeof(hashStr));

                    LOG("%s: %d hash=%s", __func__, __LINE__, hashStr);
                    LOG("%s: %d hashDld=%s", __func__, __LINE__, sig);
                    if(0 != memcmp(hash,hashDld,destlen))
                    {
                        goto exit;
                    }
                    LOG("ota file check hash success\r\n");
               }
               if(put)
                    put_fota_packet ((const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz,(fwsz < FOTA_PACKET_SZ || fw_offset == fwTotalSz));
            }
            else
            {
                LOG("httpc_send_request fail ,exit put last pkg\r\n");
                goto exit;
            }
            if(fwsz < 512 || fw_offset == fwTotalSz)
                    break;
    }

    LOG("ota dowload %d bytes,time=%dms\r\n",fwTotalSz,ty_get_tick_ms()-TickCount);
    
    if(fwTotalSz == fw_offset)
    {
        LOG("%s %d fota dowload success\r\n",__FUNCTION__,__LINE__);
    }
    else
        LOG("%s %d fota dowload fail\r\n",__FUNCTION__,__LINE__);
    
    close(settings.socket);
    httpc_deinit(httpc_handle);
    return 0;

 LOG("%s %d\r\n",__FUNCTION__,__LINE__);
exit:
    if(put)
        put_fota_packet ((const uint8_t *)rsp_buf,1,true);//写最后一包数据用于退出写FLASH任务
    LOG("%s %d fota dowload error=%d\r\n",__FUNCTION__,__LINE__);
    close(settings.socket);
    httpc_deinit(httpc_handle);
    return -1;
}
#else
/* send http get request */
int http_get_fota(const char *host,const char* fotaURI,int put,int sigalg,const char *sig)
{
    int fd;
    static char hdr[HTTP_UP_HDR_SIZE] = {0};
    static char rang[128] = {0};
    unsigned char hash[32] = {0};
    unsigned char hashDld[32] = {0};
    unsigned char hashStr[65] = {0};
    int32_t ret,retry_cnt = FOTA_HTTP_GET_FW_RETRY_CNT;
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    MD5_CTX md5;
    iot_sha256_context context;
    MD5Init(&md5);  
    
    /* perform inner SHA */
    utils_sha256_init(&context);                                      /* init context for 1st pass */
    utils_sha256_starts(&context);    

    int isigalg = sigalg;
    
    uint32_t TickCount =  ty_get_tick_ms ();
    ty_http_cfg config = {0};
    config.host = host;
    config.cert = ty_http_ca_cert;
    config.keepalvie = true;

    //1 MD5, 2 SHA256
    if(!(isigalg == 1 || isigalg == 2))
    {
        LOG("%s %d sigalg error\n",__FUNCTION__,__LINE__);
        goto exit;
    }

    if(!host || !fotaURI || !sig)
    {
        LOG("%s %d ptr NULL\n",__FUNCTION__,__LINE__);
        goto exit;
    }
    
    if(strlen(sig) == 0 || strlen(sig)%2 != 0)
    {
        LOG("%s %d sig error\n",__FUNCTION__,__LINE__);
        goto exit;
    }
    
    ty_http_handle_t handle = ty_http_setup(&config);
    if (!handle)
    {
        goto exit;
    }
    
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    int fw_offset = 0;
    int fwTotalSz = 0;
    int fwsz;

    while(1)
    {
        memset(rang,0,sizeof(rang));
        snprintf(rang,sizeof(rang),"bytes=%d-%d",fw_offset,fw_offset+FOTA_PACKET_SZ-1);
        memset(hdr,0,sizeof(hdr));
        ty_cloud_ext_notify (FIRMWARE_FOTA_DOWNLOADING);
        uint8_t *response_content = NULL;
        uint32_t responseLen = 0;
        ret = ty_http_get_request (handle,fotaURI,&response_content,&responseLen, &fwTotalSz,fw_offset,fw_offset+FOTA_PACKET_SZ-1, 10000);
        if(ret == 0 &&  response_content != NULL)
        {
           //计数重置
           retry_cnt = FOTA_HTTP_GET_FW_RETRY_CNT;
           LOG("%d %d responseLen=%d,fwTotalSz=%d\r\n",fw_offset,fw_offset+FOTA_PACKET_SZ-1,responseLen,fwTotalSz);
           fw_offset += responseLen;
           if(1 == isigalg)
                MD5Update(&md5,(const uint8_t *)response_content,responseLen);
           else utils_sha256_update(&context, (const uint8_t *)response_content,responseLen);

           if((responseLen < FOTA_PACKET_SZ )|| (fw_offset == fwTotalSz) ) 
           {    
                if(1 == isigalg)
                    MD5Final(&md5,hash);
                else
                    utils_sha256_finish(&context,hash);
                int destlen = asc2hex (hashDld, sig, strlen(sig));
                hex2asc (hash, sizeof(hash),hashStr,sizeof(hashStr));
                //bytes2hexstr (hash, sizeof(hash),hashStr,sizeof(hashStr));

                LOG("%s: %d hash=%s", __func__, __LINE__, hashStr);
                LOG("%s: %d hashDld=%s", __func__, __LINE__, sig);
                if(0 != memcmp(hash,hashDld,destlen))
                {
                    goto exit;
                }
                LOG("ota file check hash success\r\n");
           }
           if(put)
                put_fota_packet ((const uint8_t *)response_content,responseLen,(responseLen < FOTA_PACKET_SZ ) || (fw_offset == fwTotalSz));

           lpa_free(response_content);
           response_content = NULL;
           
           if(responseLen < FOTA_PACKET_SZ || fw_offset == fwTotalSz)
                break;
                   
        }
        else
        {
            //断点续传重试
            if(!retry_cnt--)
            {
                LOG("httpc_send_request fail ,exit put last pkg\r\n");
                goto exit;               
            }
            //关闭上一次连接
            if(handle)
            {
               ty_http_close(handle);
               handle = NULL;
            }
            //重新连接
            handle = ty_http_setup(&config);
            if (!handle)
            {
                LOG("ota ty_http_setup fail\r\n");
                goto exit;
            }
        }

    }

    LOG("ota dowload %d bytes,time=%dms\r\n",fwTotalSz,ty_get_tick_ms()-TickCount);
    if(fwTotalSz == fw_offset)
    {
        LOG("%s %d fota dowload success\r\n",__FUNCTION__,__LINE__);
    }
    else
        LOG("%s %d fota dowload fail\r\n",__FUNCTION__,__LINE__);
    if(handle)
        ty_http_close(handle);
    return 0;

LOG("%s %d\r\n",__FUNCTION__,__LINE__);
exit:
    if(put)
        put_fota_packet ((const uint8_t *)hash,1,true);//写最后一包数据用于退出写FLASH任务
    LOG("%s %d fota dowload error=%d,time=%dms\r\n",__FUNCTION__,__LINE__,ret,ty_get_tick_ms()-TickCount);
    if(handle)
        ty_http_close(handle);
    handle = NULL;
    return -1;
}

#endif

void put_fota_packet(const uint8_t *data, size_t len,int last_flag)
{
    ty_fota_queue_msg msg = {0};
    char *buf = lpa_malloc(len + 1);
    if(!buf)
    {
        LOG("%s %d mem error\n",__FUNCTION__,__LINE__);
        return ;
    }
    memset(buf, 0x00, len + 1);
    int i=0;
    for(i = 0; i < len; i++)
    {
        if(data[i] == '"')
            buf[i] = '\"';
        else
            buf[i] = data[i];
    }

    
    msg.last_flag = last_flag;
    msg.data = lpa_malloc(len + 1);
    #ifdef NB12_PLAT
    memcpy(msg.data, buf, len + 1);
    #elif defined(SS11_PLAT)
    memcpy(msg.data, data, len);
    #else //SLM320P or other platform
    memcpy(msg.data, data, len);
    #endif
    msg.data_len = len;
    ty_put_msg_que (_fotatask.fota_task_queue, (const void *) &msg,  TY_WAIT_FOREVER);
    lpa_free(buf);
}

//退出为升级失败
bool ty_fota_task_exit()
{
    return _fotatask.fota_task_exit;
}

int ty_fota_task()
{
    int last_flag = 0;
    uint32_t queuelen = -1;
    ty_fota_queue_msg msg = {0};
    _fotatask.fota_task_exit = false;
    unsigned char hash[32] = {0};
    unsigned char hashStr[65] = {0};

    
    LOG("%s in\r\n",__FUNCTION__);

    MD5_CTX md5;

    MD5Init(&md5);  
    
    //fota资源初始化
    if(!ty_ota_update_Init())
    {
        LOG("fota_task init error!\r\n");
        goto end;
    }

    //获取差分包数据写入flash
    while(!last_flag)
    {
        ty_get_msg_que (_fotatask.fota_task_queue, (void *)&msg,TY_WAIT_FOREVER);
        if(msg.data != NULL)
        {
            if(1 == msg.last_flag)
                last_flag = 1;
            MD5Update(&md5,(const uint8_t *)msg.data,msg.data_len);
            ty_ota_update_Hmac(msg.data, msg.data_len);
            //ota_write_flash(msg->data, msg->data_len, last_flag);
            ty_ota_write_flash(msg.data, msg.data_len);
            LOG("%s ty_ota_write_flash\r\n",__FUNCTION__);
            lpa_free(msg.data);
        }        
    }

    MD5Final(&md5,hash);
    hex2asc (hash, sizeof(hash),hashStr,sizeof(hashStr));
    LOG("%s: %d task hash=%s\r\n", __func__, __LINE__, hashStr);
    
    ty_sleep(1000);
    //差分包校验
    if(!ty_ota_check_FwValidate())
    {
        LOG("[ty_fota_task] ota_check_FwValidate error!\r\n");
        goto end;
    }

    //通知主任务准备重启
    ty_main_fota_reboot_notify (true);

    //开始升级
    ty_ota_update_start();

end:
    LOG("ota_update_start out~~~~~~~~~~~~~~~\r\n");
    ty_ota_update_deInit();
    ty_delete_msg_que (_fotatask.fota_task_queue);
    _fotatask.fota_task_queue = NULL;
//    ty_semaphore_destroy (&_fotatask.task_fail_sem);
    _fotatask.fota_task_exit = true;
    #ifdef NB12_PLAT
    ty_task_delete(&_fotatask.task_Handle);
    #elif defined(SS11_PLAT)
    while(1)
    {
        ty_sleep(1000);
        //LOG("SS11_PLAT fota task wait to delete\r\n");
    }
    #else //SLM320P or other platform
    ty_task_delete(&_fotatask.task_Handle);
    #endif
    return 0;    
}

/*
开机固件检查
1.检查NV区的升级状态位(0,上一次升级完成，进一步检查FOTA区升级状态)，如果是2表示上一次没有进入FOTA升级即失败
2.读取FOTA区升级状态，并写入NV区
3.如果升级成功上报升级任务信息
4.如果升级失败，检查升级剩余次数，>0重新开始启动升级流程
*/
int ty_CheckFota(ty_task_fota_wrapper *nv_fota)
{
    int ret = FOTA_OK;
    int taskbit,uploadbit;
    //ty_task_fota_wrapper nv_fota = {0};
    ty_module_inf info = {0};
    //LOG("%s fota_res=%d\r\n",__FUNCTION__,g_softap_fac_nv->fota_res);
    //获取当前固件版本
    ty_LoadModueInfo(&info);
        
    ret = ty_LoadFOTAInfo(nv_fota);
    if(ret != 0)
    {
        LOG("%s no nv_fota info \r\n",__FUNCTION__);
        ret =  FOTA_OK;
        goto exit;
    }
    if(strlen(nv_fota->taskNo) == 0 )
    {
        LOG("%s nv_fota no taskNo info\r\n",__FUNCTION__);
        ret =  FOTA_OK;
        goto exit;

    }
    taskbit = g_ty_cloud_nv.nv_fota.fota_erase & E_FOTA_MASK_VALID;
    uploadbit = g_ty_cloud_nv.nv_fota.fota_erase & E_FOTA_MASK_UPLOAD;
    LOG("%s task flag=%x\r\n",__FUNCTION__,taskbit);
    LOG("%s upload flag=%x\r\n",__FUNCTION__,uploadbit);
    
    if((FOTA_TASK_INVALID == taskbit)  && 
       (FOTA_TASK_UPLOADED == uploadbit) )
    {
        LOG("%s nv_fota invalid & uploaded! \r\n",__FUNCTION__);
        ret =  FOTA_OK;
        goto exit;
    }
    
    if((FOTA_TASK_INVALID == uploadbit)  && 
       (FOTA_TASK_NO_UPLOAD == uploadbit) )
    {
        LOG("%s nv_fota invalid no upload!\r\n",__FUNCTION__);
    }

    ret = ty_ota_nv_check_result (nv_fota);
    switch(ret)
    {
        case FOTA_OK:
            LOG("%s  check FOTA_OK,do nothing\r\n",__FUNCTION__);
            goto exit;
            break;
        case FOTA_RETRY:
            LOG("%s  check FOTA_RETRY\r\n",__FUNCTION__);
            goto fota_retry;
            break;
        case FOTA_UPD_OK:
            LOG("%s  check FOTA_UPD_OK\r\n",__FUNCTION__);
            //比对版本，目标版本与当前版本一致上报升级结果
            if( (strlen(info.firmware_version) == strlen(nv_fota->fota.target_version)) &&
              (0 ==  strncmp(info.firmware_version,nv_fota->fota.target_version,strlen(nv_fota->fota.target_version))) )
            {
                LOG("%s firmware update success\r\n",__FUNCTION__);
                LOG("%s %s upgraded\r\n",__FUNCTION__,info.firmware_version);
                ty_cloud_ext_notify (FIRMWARE_FOTA_UPDATE_SUCCESS);
                ret = FOTA_UPD_OK;
                goto exit;
            }
            
            if( (strlen(info.firmware_version) == strlen(nv_fota->fota.start_version)) &&
              (0 ==  strncmp(info.firmware_version,nv_fota->fota.start_version,strlen(nv_fota->fota.start_version))) )
            {
                LOG("%s fota start_version check ok,try update\r\n",__FUNCTION__);
                //下载失败情况下,尝试重新升级,检查当前任务是否有效，再次升级
                goto fota_retry;
            }
            LOG("%s firmware_version check error,do nothing\r\n",__FUNCTION__);
            ret = FOTA_OK;
            goto exit;
        case FOTA_UPD_FAIL:
            LOG("%s  check FOTA_UPD_FAIL,retry\r\n",__FUNCTION__);
            goto fota_retry;//检查当前任务是否有效，再次升级
    }
    
fota_retry:
    if(taskbit == FOTA_TASK_VALID)
    {
        ret = _ty_mgr_module_fota (nv_fota->ID,nv_fota->Ver,nv_fota->taskNo,&nv_fota->fota,false);
    }
    else
    {
        LOG("%s FOTA_TASK_INVALID upload FOTA_UPD_FAIL\r\n",__FUNCTION__);
        ret =  FOTA_UPD_FAIL;
    }

exit:    
    if(ret != FOTA_OK) //??????
    {
        put_fota_task_up_msg (nv_fota,ret);
        if(!(ret == FOTA_UPD_OK || ret == FOTA_UPGRADED))
        {
            LOG("no FOTA_UPD_OK ret=%d\r\n",ret);
            ty_FotaUploadErrMsg (nv_fota->fota.modrsp_url,nv_fota->taskNo, ret);
            ty_cloud_ext_notify (FIRMWARE_FOTA_UPDATE_FAILED);
        }
    }
    return ret;
}


int ty_UpdateFOTAInfo(int fota_erase)
{
    g_ty_cloud_nv.nv_fota.fota_erase = fota_erase;
    LOG("%s fota_erase=%d\r\n",__FUNCTION__,fota_erase);
    ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

static int ty_decreaseRetrycnt()
{
    if( g_ty_cloud_nv.nv_fota.fota_retrycnt > 0)
        g_ty_cloud_nv.nv_fota.fota_retrycnt--;
    LOG("%s fota_retrycnt=%d\r\n",__FUNCTION__,g_ty_cloud_nv.nv_fota.fota_retrycnt);
    return 0;
}

int ty_SaveFOTAInfo(ty_task_fota_wrapper *fota)
{
    g_ty_cloud_nv.write_to_flash_num++;
    memset(&g_ty_cloud_nv.nv_fota,0,sizeof(g_ty_cloud_nv.nv_fota));
    memcpy(g_ty_cloud_nv.nv_fota.fota_taskID,fota->ID,strlen(fota->ID));
    memcpy(g_ty_cloud_nv.nv_fota.fota_taskVer,fota->Ver,strlen(fota->Ver));
    memcpy(g_ty_cloud_nv.nv_fota.fota_taskNo,fota->taskNo,strlen(fota->taskNo));
    memcpy(g_ty_cloud_nv.nv_fota.fota_currver,fota->fota.start_version,strlen(fota->fota.start_version));
    memcpy(g_ty_cloud_nv.nv_fota.fota_targetver,fota->fota.target_version,strlen(fota->fota.target_version));
    g_ty_cloud_nv.nv_fota.fota_sigalg = fota->fota.sign_type;
    memcpy(g_ty_cloud_nv.nv_fota.fota_sig,fota->fota.sign_value,strlen(fota->fota.sign_value));
    memcpy(g_ty_cloud_nv.nv_fota.fota_url,fota->fota.fota_url,strlen(fota->fota.fota_url));
    memcpy(g_ty_cloud_nv.nv_fota.modrsp_url,fota->fota.modrsp_url,strlen(fota->fota.modrsp_url));
    g_ty_cloud_nv.nv_fota.fota_retrycnt = fota->fota.retry_strategy;
    g_ty_cloud_nv.nv_fota.fota_erase = FOTA_TASK_NO_UPLOAD|FOTA_TASK_VALID;
    ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

int ty_LoadFOTAInfo(ty_task_fota_wrapper *fota)
{
    fota->ID = g_ty_cloud_nv.nv_fota.fota_taskID;
    fota->Ver = g_ty_cloud_nv.nv_fota.fota_taskVer;
    fota->taskNo = g_ty_cloud_nv.nv_fota.fota_taskNo;
    fota->fota.start_version = g_ty_cloud_nv.nv_fota.fota_currver;
    fota->fota.target_version = g_ty_cloud_nv.nv_fota.fota_targetver;
    fota->fota.sign_type = g_ty_cloud_nv.nv_fota.fota_sigalg;
    fota->fota.sign_value = g_ty_cloud_nv.nv_fota.fota_sig;
    fota->fota.fota_url = g_ty_cloud_nv.nv_fota.fota_url;
    fota->fota.modrsp_url= g_ty_cloud_nv.nv_fota.modrsp_url;
    fota->fota.retry_strategy = g_ty_cloud_nv.nv_fota.fota_retrycnt;

    LOG("%s fota->erase=%x\r\n",__FUNCTION__,g_ty_cloud_nv.nv_fota.fota_erase);
    LOG("%s fota->ID=%s\r\n",__FUNCTION__,fota->ID);
    LOG("%s fota->Ver=%s\r\n",__FUNCTION__,fota->Ver);
    LOG("%s fota->taskNo=%s\r\n",__FUNCTION__,fota->taskNo);
    LOG("%s fota->start_version=%s\r\n",__FUNCTION__,fota->fota.start_version);
    LOG("%s fota->target_version=%s\r\n",__FUNCTION__,fota->fota.target_version);
    LOG("%s fota->sign_type=%d\r\n",__FUNCTION__,fota->fota.sign_type);
    LOG("%s fota->sign_value=%s\r\n",__FUNCTION__,fota->fota.sign_value);
    LOG("%s fota->FOTA_URL=%s\r\n",__FUNCTION__,fota->fota.fota_url);
    LOG("%s fota->modrsp_url=%s\r\n",__FUNCTION__,fota->fota.modrsp_url);
    LOG("%s fota->retry_strategy=%d\r\n",__FUNCTION__,fota->fota.retry_strategy);
    
    return 0;
}

int ty_EraseFOTAInfo()
{
    memset(&g_ty_cloud_nv.nv_fota,0,sizeof(g_ty_cloud_nv.nv_fota));
    g_ty_cloud_nv.nv_fota.fota_erase = FOTA_TASK_UPLOADED|FOTA_TASK_INVALID;
    ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

int fota_task_init()
{
    if(_fotatask.fota_task_queue)
        return -1;
   _fotatask.fota_task_queue = ty_create_msg_Que(TY_FOTA_TASK_MSGQ_SIZE, sizeof(ty_fota_queue_msg));
//    ty_semaphore_create(&_fotatask.task_fail_sem);
    return ty_task_create((ty_task_func_t)ty_fota_task, "ty_fota_task", 0x800, NULL, 10, &_fotatask.task_Handle);
}

int fota_http_download(ty_task_fota *fota)
{
    int ret;
    //wait PDP active
    if(ty_wait_tcpip_ok(10) == 0)
    {
        LOG("%s ty_wait_tcpip_ok timeout\r\n",__FUNCTION__);
        put_fota_packet ((const uint8_t *)fota,1,true);//写最后一包数据用于退出写FLASH任务
        return FOTA_DLD_ERR;
    }
    char *url = lpa_malloc(strlen(fota->fota_url)+1);
    memset(url,0,strlen(fota->fota_url)+1);
    memcpy(url,fota->fota_url,strlen(fota->fota_url));
    char host[100] = {0};
    char uri[100] = {0};
    ret = httpc_parser_url (url,host,sizeof(host), uri,sizeof(uri));
    lpa_free(url);
    url= NULL;
    if(ret != 0)
    {
        LOG("%s httpc_parser_url error\r\n",__FUNCTION__);
        put_fota_packet ((const uint8_t *)fota,1,true);
        return FOTA_DLD_ERR;
    }
    ret = http_get_fota (host,uri,1,fota->sign_type,fota->sign_value);
    return ret;
}

static int _ty_mgr_module_fota(const char *ID,const char *ver, const char *taskNo,ty_task_fota *fota,bool savenvflash)
{
    int ret ;
    ty_task_fota_wrapper nv_fota = {0};
    ty_module_inf info = {0};
    if(taskNo == NULL || fota == NULL)
    {
        LOG("%s taskNo NULL\r\n",__FUNCTION__);
        return FOTA_PARAMS_ERR;
    }
    
    //获取当前固件版本
    ty_LoadModueInfo(&info);
    
    if( (strlen(info.firmware_version) == strlen(fota->target_version)) &&
      (0 ==  strncmp(info.firmware_version,fota->target_version,strlen(fota->target_version))) )
    {
        LOG("%s %s upgraded\r\n",__FUNCTION__,info.firmware_version);
        return FOTA_UPGRADED;
    }
    if( !((strlen(info.firmware_version) == strlen(fota->start_version)) &&
      (0 ==  strncmp(info.firmware_version,fota->start_version,strlen(fota->start_version)))) )
    {
        LOG("%s start_version error, %s != %s\r\n",__FUNCTION__,info.firmware_version,fota->start_version);
        return FOTA_VERSION_ERR;
    }
    LOG("%s  start upgrade ,target=%s\r\n",__FUNCTION__,fota->target_version);

    nv_fota.ID =ID;
    nv_fota.Ver = ver;
    nv_fota.taskNo = taskNo;
    memcpy(&nv_fota.fota,fota,sizeof(ty_task_fota));

    //缓存当前升级任务
    if(savenvflash)
        ty_SaveFOTAInfo(&nv_fota);
    
    LOG("%s FIRMWARE_FOTA_START\r\n",__FUNCTION__);
    
    ty_cloud_ext_notify (FIRMWARE_FOTA_START);
    
    int retrycount = fota->retry_strategy;
    int i;
    //不重试情况任务直接置无效状态，从FLASH中的缓存任务只再执行一次
    if(retrycount == 0 || !savenvflash)
    {
        //任务只执行一次
        ty_UpdateFOTAInfo (FOTA_TASK_NO_UPLOAD|FOTA_TASK_INVALID);
    }
    do
    {
        LOG("%s fota_task_init\r\n",__FUNCTION__);
        //启动升级task,读固件并写FLASH队列
        ret = fota_task_init();
        if(ret != 0)
        {
            LOG("%s fota_task_init fail\r\n",__FUNCTION__);
            ret = FOTA_TASK_FAIL;
            break;
        }
        LOG("%s fota_http_download\r\n",__FUNCTION__);
        //下载固件
        ret = fota_http_download (fota);
        if(ret == 0) //下载成功等待升级重启
        {
            ty_cloud_ext_notify (FIRMWARE_FOTA_DOWNLOADED);
        }
        else //下载失败
        {
            LOG("%s download fail, retrycount=%d \r\n",__FUNCTION__,retrycount);
            ty_cloud_ext_notify (FIRMWARE_FOTA_DOWNLOAD_FAILED);
            ret = FOTA_DLD_ERR;
        }
        //监听升级task是否失败
        while(!_fotatask.fota_task_exit)
        {
            LOG("%s wait fota task exit\r\n",__FUNCTION__);
            ty_sleep(3000);
        }
        LOG("%s fota task exit!!! \r\n",__FUNCTION__);
        #ifdef SS11_PLAT
        LOG("SS11_PLAT  fota  task delete\r\n");
        ty_task_delete((void *)&_fotatask.task_Handle);
        #endif
        //当前升级线程退出(升级异常)，FLASH中重新次数更新(没有立即写FLASH，flash hook中写入)
        ty_decreaseRetrycnt();
    }while(retrycount-- > 0);
    //下载成功但升级失败
    if(ret == 0)
        ret = FOTA_UPD_FAIL;
    //升级失败，更新任务标志位
    ty_UpdateFOTAInfo (FOTA_TASK_NO_UPLOAD|FOTA_TASK_INVALID);

    LOG("%s Leave \r\n",__FUNCTION__);
    return ret;
}

int ty_FotaUploadErrMsg(const char *url,const char *task_no,int errcode)
{
    cJSON *root = NULL;
    char host[128] = {0};
    char uri[128] = {0};
    char imei[20] = {0};
    int ret;
    char *jsonStr = NULL;

    LOG("%s Enter url=%s\r\n",__FUNCTION__,url);
    
    ret = httpc_parser_url (url,host, sizeof(host),  uri,sizeof(uri));
    if(ret != 0)
    {
        LOG("%s httpc_parser_url error,ret=%d\r\n",__FUNCTION__,ret);
        return ret;
    }
    root =  cJSON_CreateObject();
    if(!root)
    {
        return -1;
    }
    if (task_no)
    {
        cJSON_AddItemToObject(root, "task_no", cJSON_CreateString(task_no));
    }
    at_imei(imei);
    cJSON_AddItemToObject(root, "imei", cJSON_CreateString(imei));
    cJSON_AddItemToObject(root, "err_code", cJSON_CreateNumber(errcode));
    jsonStr = cJSON_PrintUnformatted(root);
    int code = -1;
    int i ;
    for( i = 0;i < 3;i++)
    {
        char *resp = http_post_util (host, uri, jsonStr,ty_http_ca_cert,HTTP_Content_Type_JSON,&code);
        if(resp)
        {
            LOG("%s resp=%s\r\n",__FUNCTION__,resp);
            lpa_free(resp);
            resp = NULL;
        }
        if(code == 0)
        {
            LOG("%s http_post_util success \r\n",__FUNCTION__);
            break;
        }
            
    }
    if(root)
        cJSON_Delete(root);
    
    if(jsonStr)
        lpa_free(jsonStr);
    LOG("%s Leave \r\n",__FUNCTION__);
    return 0;
}
int ty_MgrModuleFota(const char *ID,const char *ver, const char *taskNo,ty_task_fota *fota)
{
    int ret;
    ty_task_fota_wrapper nv_fota = {0};
    
    ty_main_task_fota_notify (true);
    
    ret = _ty_mgr_module_fota (ID,ver,taskNo, fota, true);
    if(ret != FOTA_OK) //??????
    {
        nv_fota.fota = *fota;
        nv_fota.ID = ID;
        nv_fota.taskNo = taskNo;
        nv_fota.Ver = ver;
        ret = put_fota_task_up_msg (&nv_fota,ret);
        LOG("put_fota_task_up_msg  ret=%d\r\n",ret);
        if(!(ret == FOTA_UPD_OK || ret == FOTA_UPGRADED))
        {
            ty_FotaUploadErrMsg (fota->modrsp_url,taskNo, ret);
            ty_cloud_ext_notify (FIRMWARE_FOTA_UPDATE_FAILED);
        }
    }
    //通知退出升级
    ty_main_task_fota_notify (false);
    return 0;
}



