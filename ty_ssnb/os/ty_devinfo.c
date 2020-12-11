#include "ty_devinfo.h"
#include "ty_mqtt_basic_info.h"
#include "ty_os_adpt.h"

//user can set val when working
unsigned char  *g_user_flash = NULL;
#define  USER_FLASH_LPA_LEN             0x800  //max val is USER_FLASH_LEN_MAX
#define DEV_FW_VER_OFFSET 20

static char IMEI_ID[16] = {0};
ty_cloud_nv_t   g_ty_cloud_nv = {0,60,3,30,30,(60*5),60,3,2,0}; //,{10,200}

ty_cloud_nv_t   g_ty_cloud_nv_calc = {0};


int flash_status = 0;
//const char *g_product_type = PRODUCT_TYPE;
//const char *g_firmware_version = FIRMWARE_VERSION;

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

int ty_LoadModueInfo(ty_module_inf *info)
{
    if(!info)
        return -1;
    info->product_type =  PRODUCT_TYPE;
    at_imei (IMEI_ID);
    info->bind_imei = IMEI_ID;
    info->firmware_version = FIRMWARE_VERSION;//ptr +DEV_FW_VER_OFFSET;
    return 0;
}

int ty_UpdateModueInfo(ty_module_inf *info)
{
    LOG("%s %d Not Surpported!!!\r\n",__FUNCTION__,__LINE__);
    return 0;
}

int ty_LoadDeviceInfo(char productKey[20],char proudctSecret[64], char deviceName[32])
{
    if(!productKey || !proudctSecret || !deviceName)
        return -1;
    memcpy(productKey,MQTT_PRODUCT_KEY,strlen(MQTT_PRODUCT_KEY));
    memcpy(proudctSecret,MQTT_PRODUCT_SECRET,strlen(MQTT_PRODUCT_SECRET));
    at_imei (deviceName);
    return 0;
}

int ty_SaveDeviceSecret(char deviceSecret[64])
{
    uint8_t piv[8] = {0};
    char IMEI[17] = {0};
    uint8_t datakey[16] = {0};
    uint8_t IMEI_[8] = {0};
    uint8_t encData[64] = {0};
    
    at_imei (IMEI);
    
    IMEI[16] = 'F';
    
    if(!deviceSecret)
        return -1;
    
    int dlen = strlen(deviceSecret);
    if(dlen == 0)
        return -1;

    char deviceSecretPad[64] = {0};
    memcpy(deviceSecretPad,deviceSecret,dlen);
        
    asc2hex (IMEI_,IMEI, strlen(IMEI));
    calcDataKey (IMEI_,datakey);

    int encDataLen = des3_cbc_encrypt(encData, deviceSecretPad, 64, datakey,16, piv);
    memcpy(g_ty_cloud_nv.devicesecret,encData,64);
    g_ty_cloud_nv.write_to_flash_num++;
    g_ty_cloud_nv.g_devreg_status = 1;
	ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

int ty_LoadDeviceSecret(char deviceSecret[64])
{
    //LOG("%s %d g_devreg_status=%d\r\n",__FUNCTION__,__LINE__,g_ty_cloud_nv.g_devreg_status);
    if(g_ty_cloud_nv.g_devreg_status == 0)
    {
        return -1;
    }

    uint8_t piv[8] = {0};
    char IMEI[17] = {0};
    uint8_t datakey[16] = {0};
    uint8_t IMEI_[8] = {0};
    uint8_t encData[64] = {0};
    
    at_imei (IMEI);
    
    IMEI[16] = 'F';
    int dlen = strlen(deviceSecret);
    

    asc2hex (IMEI_,IMEI, strlen(IMEI));
    calcDataKey (IMEI_,datakey);
    des3_cbc_decrypt(deviceSecret,g_ty_cloud_nv.devicesecret, 64 , datakey, 16, piv);
    //memcpy(deviceSecret,g_ty_cloud_nv.devicesecret,64);
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);

    if(strlen(deviceSecret)%8 != 0)
    {
        LOG("Error");
        return -1;
    }
    return 0;
}


int ty_FindAppletInfo(char *str_aid)
{
    int i = 0;
    for(i = 0;i< APPLET_MAX_SIZE;i++)
    {
        int al = strlen(str_aid);
        if(al == strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid) && 
            0 == strncmp(str_aid,g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid,al))
        {
            return i;
        }
    }
    return -1;
}


int ty_getTsmAppletInfo(char* aid,sm_aid* tsmAppInfo)
{
    if(!aid || !tsmAppInfo )
        return -1;
    int index = ty_FindAppletInfo (aid);
    LOG("%s %d ty_FindAppletInfo index = %d \r\n",__FUNCTION__,__LINE__,index);
    if(index >=0){
        sm_aid tmp = g_ty_cloud_nv.nv_aids.sm_aid_list[index];

        memset(tsmAppInfo->app_aid , 0 , sizeof(tsmAppInfo->app_aid));
        memcpy(tsmAppInfo->app_aid , tmp.app_aid , strlen(tmp.app_aid));

        memset(tsmAppInfo->app_version , 0 , sizeof(tsmAppInfo->app_version));
        memcpy(tsmAppInfo->app_version , tmp.app_version , strlen(tmp.app_version));

        tsmAppInfo->app_status = tmp.app_status;
        tsmAppInfo->app_life_cycle_state = tmp.app_life_cycle_state;
        LOG("%s %d app_status = %d , life_cycle_state = %d \r\n",__FUNCTION__,__LINE__,tsmAppInfo->app_status,tsmAppInfo->app_life_cycle_state);

        return 0;
    }
    return -1;
}

int ty_SaveAppletInfo(sm_aid *aid)
{
    int index;
    int i = 0;
    bool save = false;
    
    if(!aid)
    {
        LOG("%s %d ptr NULL\n",__FUNCTION__,__LINE__);
        return -1;
    }
    //非法值检查
    if(!(aid->app_status == e_app_status_no_personal ||
       aid->app_status == e_app_status_personal  ||
       aid->app_status == e_app_status_active  || 
       aid->app_status == e_app_status_nv_erase) )
    {
        LOG("%s %d app_status=%d error\n",__FUNCTION__,__LINE__,aid->app_status);
        return -2;
    }
    index = ty_FindAppletInfo (aid->app_aid);
    LOG("%s %d save applet ver=%s\r\n",__FUNCTION__,__LINE__,aid->app_version);
    if(index >=0)
    {
        LOG("%s %d update applet info idx=%d\r\n",__FUNCTION__,__LINE__,index);
        //更新版本，应用状态，生命周期状态
        g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_status = aid->app_status;
        g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_life_cycle_state = aid->app_life_cycle_state;
        memset(g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_version,0,sizeof(aid->app_version));
        memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_version,aid->app_version,strlen(aid->app_version));
        LOG("%s %d update applet ver=%s\r\n",__FUNCTION__,__LINE__,g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_version);
        save = true;
        goto write_nv;
    }
    for(i = 0;i< APPLET_MAX_SIZE;i++)
    {
        //判断无效AID信息 状态0xFF   版本 AID长度为空
        if(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_nv_erase || (strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid) == 0 && 
            strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version) == 0 ))
        {
            memset(&g_ty_cloud_nv.nv_aids.sm_aid_list[i],0,sizeof(sm_aid));
            //保存AID,版本，应用状态，生命周期状态
            g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status = aid->app_status;
            g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_life_cycle_state = aid->app_life_cycle_state;
            memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid,aid->app_aid,strlen(aid->app_aid));
            memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version,aid->app_version,strlen(aid->app_version));
            save = true;
            LOG("%s %d save applet info idx=%d\r\n",__FUNCTION__,__LINE__,i);
            break;
        }
    }
    if(!save)
    {
        LOG("%s %d save applet info ERROR\r\n",__FUNCTION__,__LINE__);
        return -1;
    }
write_nv:
    g_ty_cloud_nv.write_to_flash_num++;
	ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

int ty_SaveAppletInfoNoNv(sm_aid *aid)
{
    int index;
    int i = 0;
    bool save = false;
    if(!aid)
    {
        return -1;
    }
    
    index = ty_FindAppletInfo (aid->app_aid);
    if(index >=0)
    {
        //更新版本，应用状态，生命周期状态
        g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_status = aid->app_status;
        g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_life_cycle_state = aid->app_life_cycle_state;
        memset(g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_version,0,sizeof(aid->app_version));
        memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_version,aid->app_version,strlen(aid->app_version));

        save = true;
        goto write_nv;
    }
    for(i = 0;i< APPLET_MAX_SIZE;i++)
    {
        //判断无效AID信息 状态0xFF   版本 AID长度为空
        if(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_nv_erase || (strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid) == 0 && 
            strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version) == 0 ))
        {
            memset(&g_ty_cloud_nv.nv_aids.sm_aid_list[i],0,sizeof(sm_aid));
            //保存AID,版本，应用状态，生命周期状态
            g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status = aid->app_status;
            g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_life_cycle_state = aid->app_life_cycle_state;
            memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid,aid->app_aid,strlen(aid->app_aid));
            memcpy(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version,aid->app_version,strlen(aid->app_version));
            save = true;
            break;
        }
    }
    if(!save)
    {
        return -1;
    }
write_nv:
    return 0;
}


int ty_LoadAppletInfoList(Aid_List **list)
{
    int ret;
    int i;
    int cnt = 0;
    Aid_List *mlist = NULL;
    if(!list)
    {
        return -1;
    }

    mlist = lpa_calloc(1,sizeof(Aid_List));
    if(!mlist)
        return -1;
    mlist->list = NULL;
    mlist->aid_size = 0;
    for(i = 0;i< APPLET_MAX_SIZE;i++)
    {
        if(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_no_personal ||
           g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_personal ||
           g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_active)
        {
            //LOG("%s %d app_status = %d  idx = %d\r\n",__FUNCTION__,__LINE__,g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status,i);
            mlist->aid_size+=1;
        }
        else if(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_nv_erase)
        {
            //LOG("%s %d app_status nv erase status idx=%d\r\n",__FUNCTION__,__LINE__,i);
        }
        else 
        {
            LOG("%s %d app_status unkown status idx=%d\r\n",__FUNCTION__,__LINE__,i);
        }
    }
    
    LOG("%s %d aid_size = %d\r\n",__FUNCTION__,__LINE__,mlist->aid_size);
  
    if(mlist->aid_size)
    {
        mlist->list = lpa_calloc(mlist->aid_size,sizeof(sm_aid));
        if(!mlist->list)
            return -1;
        
        for(i = 0;i< APPLET_MAX_SIZE;i++)
        {
           //LOG("%s %d ,idx = %d app_status = %d \r\n",__FUNCTION__,__LINE__,i,g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status);
           if(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_no_personal ||
              g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_personal ||
              g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status == e_app_status_active)

           {
               mlist->list[cnt].app_status = g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status;
               mlist->list[cnt].app_life_cycle_state  = g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_life_cycle_state;
               memcpy(mlist->list[cnt].app_aid,g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid,strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_aid));
               memcpy(mlist->list[cnt].app_version,g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version,strlen(g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_version));
               cnt++;
           }
        }
    }
    else
    {
        mlist->list = NULL;
    }
    *list = mlist;
    //LOG("Leave %s %d\r\n",__FUNCTION__,__LINE__);
    return 0;
}

void ty_AppletInfoListFree(Aid_List *list)
{
    if(!list)
        return ;
    int i;
    if(list->list)
        lpa_free(list->list);
    list->list = NULL;
    lpa_free(list);
    
}

int ty_EraseAppletInfo(sm_aid *aid)
{
    int index ;
    int i = 0;
    if(!aid)
        return -1;
        
    index = ty_FindAppletInfo (aid->app_aid);
    if(index <0)
    {
        LOG("%s %d ty_FindAppletInfo ERROR,aid=%s\r\n",__FUNCTION__,__LINE__,aid->app_aid);
        return -1;
    }
    g_ty_cloud_nv.nv_aids.sm_aid_list[index].app_status = e_app_status_nv_erase;
    g_ty_cloud_nv.write_to_flash_num++;
	ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}

int ty_EraseAllAppletInfo()
{
    int index ;
    int i = 0;
    
    g_ty_cloud_nv.write_to_flash_num++;
    for(i = 0;i< APPLET_MAX_SIZE;i++)
        g_ty_cloud_nv.nv_aids.sm_aid_list[i].app_status = e_app_status_nv_erase;
    
	ty_flash_save((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    return 0;
}


int ty_LoadDeviceRegUrl(char url[128],int *port)
{
    if(url == NULL || port == NULL)
        return -1;
    memcpy(url,TY_CLOUD_HTTP_DEVREG_URL,strlen(TY_CLOUD_HTTP_DEVREG_URL));
    *port = TY_CLOUD_HTTP_DEVREG_PORT;
    return 0;
}

static void NvStrcutInfoFactory()
{
    memset(&g_ty_cloud_nv,0,sizeof(ty_cloud_nv_t));
    //{0,60,3,30,30,(60*5),60,3,2,0}; //,{10,200}
    g_ty_cloud_nv.g_attach_time = 60;
	g_ty_cloud_nv.g_attach_retry= 3;
	g_ty_cloud_nv.g_regist_time = 30;
	g_ty_cloud_nv.g_wait_down_data_time = 30;
	g_ty_cloud_nv.g_working_time = (60*5);
	g_ty_cloud_nv.g_next_work_time = 60;
	g_ty_cloud_nv.g_send_retry = 3;//3
	g_ty_cloud_nv.g_send_num = 2;
}

static void appletInfoFactory()
{
    //AID区域初始化为全FF
    memset(&g_ty_cloud_nv.nv_aids,0xFF,sizeof(g_ty_cloud_nv.nv_aids));
    //保存预置的SAM应用AID信息
    sm_aid aid = {0};
    memcpy(aid.app_aid,SAM_AID,strlen(SAM_AID));
    memcpy(aid.app_version,"2",strlen("2"));
    aid.app_life_cycle_state = 1;
    aid.app_status = e_app_status_no_personal;
    ty_SaveAppletInfoNoNv (&aid);
}

//only called by user_hook_flash_restore when wakeup from deep sleep
void  read_user_nv_flash()
{
    int ret;
	//flash is NULL,use g_ty_cloud_nv default value
	if(ty_flash_check_factory())
	{
		//LOG("+DBGINFO:ty cloud NV read is NULL\r\n");
        appletInfoFactory ();
	}
	else
	{
		ret = ty_flash_read((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        unsigned int crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
        //校验失败/数据读取错误
        if(ret != 0 || crc32 != g_ty_cloud_nv.crc32)
        {
            
            ty_cloud_nv_t *nv = &g_ty_cloud_nv_calc;
            memset(nv,0,sizeof(ty_cloud_nv_t));
            ty_flash_read_backup ((unsigned char *)nv,sizeof(ty_cloud_nv_t));
            crc32 = ty_crc32(0, (unsigned char *)nv, sizeof(ty_cloud_nv_t)-sizeof(unsigned int));
            //备份区数据正常，恢复到主数据区
            if(crc32 == nv->crc32)
            {
                flash_status = e_flash_status_main_rebuild;
                memcpy(&g_ty_cloud_nv,nv,sizeof(ty_cloud_nv_t));
                ty_flash_write((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
            }
            else //备份区数据异常
            {
                flash_status = e_flash_status_main_bak_err;
                //数据初始化
                NvStrcutInfoFactory ();
                appletInfoFactory ();
            }
            
        }
        else
        {
            ty_cloud_nv_t *nv = &g_ty_cloud_nv_calc;
            memset(nv,0,sizeof(ty_cloud_nv_t));
            ret = ty_flash_read_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
            unsigned int bak_crc32 = ty_crc32(0, (unsigned char *)nv, sizeof(ty_cloud_nv_t)-sizeof(unsigned int));
            //备份区数据异常，重写备份区
            if(ret != 0 || bak_crc32 != nv->crc32)
            {
                memcpy(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
                ty_flash_write_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
                flash_status = e_flash_status_bak_rebuild;
                return ;
            }
            
            if(bak_crc32 == crc32)
            {
                flash_status = e_flash_status_crc_ok;
            }
            else //备份区数据不一致
            {
                if(nv->write_to_flash_num > g_ty_cloud_nv.write_to_flash_num)
                {
                    memcpy(&g_ty_cloud_nv,nv,sizeof(ty_cloud_nv_t));
                    flash_status = e_flash_status_bak_newer;
                }
                else if(nv->write_to_flash_num < g_ty_cloud_nv.write_to_flash_num)
                {
                    memcpy(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
                    flash_status = e_flash_status_main_newer;
                }
                else
                    flash_status = e_flash_status_unkown;
            }
        }
		//LOG("+DBGINFO:ty cloud NV read\r\n"); 
	}
}

void  save_user_nv_flash()
{
    int retryCNT = NV_WRITE_RETRY_CNT ;
    int retryCNT_backup = NV_WRITE_RETRY_CNT ;
    ty_cloud_nv_t *nv = NULL;
    unsigned int write_to_flash_num = g_ty_cloud_nv.write_to_flash_num+1;
    nv = &g_ty_cloud_nv_calc;
    memset(nv,0,sizeof(ty_cloud_nv_t));
retry_writeNV_backup:   
    do
    {
        if(!retryCNT_backup--)
            goto exit; 

        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        //数据没有更新
        if(0 == memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            //LOG("NV  backup data no update!\r\n");
            break;
        }
        
        //有更新数据
        g_ty_cloud_nv.write_to_flash_num = write_to_flash_num;
        g_ty_cloud_nv.crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, 
            sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
        
        ty_flash_write_backup((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        if(0 != memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            goto retry_writeNV_backup;
        }
    }while(0);
    //LOG("NV write backup ok\r\n"); 
retry_writeNV:
    do
    {
        if(!retryCNT--)
            goto exit;
        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        //数据没有更新
        if(0 == memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            //LOG("NV data no update!\r\n"); 
            break;
        }
        //有更新数据
        g_ty_cloud_nv.write_to_flash_num = write_to_flash_num;
        g_ty_cloud_nv.crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, 
            sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
        
        
        ty_flash_write((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        if(0 != memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            goto retry_writeNV;
        }
        //LOG("NV write ok\r\n"); 
    }while(0);
exit:
    return;
}

//测试只写NV 备份区
void  save_user_nv_bk_flash()
{
    int retryCNT = NV_WRITE_RETRY_CNT ;
    int retryCNT_backup = NV_WRITE_RETRY_CNT ;
    ty_cloud_nv_t *nv = NULL;
    unsigned int write_to_flash_num = g_ty_cloud_nv.write_to_flash_num+1;
    nv = &g_ty_cloud_nv_calc;
    memset(nv,0,sizeof(ty_cloud_nv_t));

retry_writeNV_backup:   
    do
    {
        if(!retryCNT_backup--)
            goto exit; 

        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        //数据没有更新
        if(0 == memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            LOG("NV  backup data no update!\r\n");
            break;
        }
        
        //有更新数据
        g_ty_cloud_nv.write_to_flash_num = write_to_flash_num;
        g_ty_cloud_nv.crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, 
            sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
        
        ty_flash_write_backup((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        memset(nv,0,sizeof(ty_cloud_nv_t));
        ty_flash_read_backup((unsigned char *)nv,sizeof(ty_cloud_nv_t));
        if(0 != memcmp(nv,&g_ty_cloud_nv,sizeof(ty_cloud_nv_t)))
        {
            goto retry_writeNV_backup;
        }
    }while(0);
    LOG("NV write backup ok\r\n"); 
exit:
    return ;
}


int ty_flash_save(void *data, int size)
{
    save_user_nv_flash ();
}

