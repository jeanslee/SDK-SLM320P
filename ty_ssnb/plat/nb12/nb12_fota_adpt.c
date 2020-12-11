#include "softap_api.h"
#include "xy_fota.h"
#include "ty_fota_adpt.h"
#include "ty_devinfo.h"
#include "ty_os_adpt.h"

int ty_ota_nv_check_result(ty_task_fota_wrapper *nv_fota)
{
    int ret = FOTA_OK;
    ty_module_inf info = {0};
    ty_LoadModueInfo(&info);
    if(g_softap_fac_nv->fota_res == 0)
    {
        if(*(unsigned int*)0X271FF000 == 1)
        {
             //Take update result, success: 0000 0110
             LOG("%s take update result, success\r\n",__FUNCTION__);
            g_softap_fac_nv->fota_res = g_softap_fac_nv->fota_res|0X02|0X04;
            SET_FACTORY_NV_SAVE_FLAG(0);
            return FOTA_UPD_OK;
        }
        else if(*(unsigned int*)0X271FF000 == 2)
        {
            LOG("%s Take OTA partition update result fail=2\r\n",__FUNCTION__);
            //Take update result, failed: 0000 1010
            g_softap_fac_nv->fota_res = g_softap_fac_nv->fota_res|0X02|0X08;
            SET_FACTORY_NV_SAVE_FLAG(0);
            //snprintf(at_str, MAX_ONE_NET_AT_SIZE, "\r\n+MIPLEVENT:%d,%d\r\n", CIS_REF, CIS_EVENT_FIRMWARE_UPDATE_FAILED);
            ret = FOTA_UPD_FAIL;
            goto exit;
        }
        else
        {
            LOG("%s Take OTA partition update result unknonw\r\n",__FUNCTION__);
            //????：o?━??：1?：|????,3?└：o???D?：|y??
            return FOTA_RETRY;
        }
    }
    else if(g_softap_fac_nv->fota_res == 2)
    {
        LOG("%s firmware update fail ,try again\r\n",__FUNCTION__);
        //：o?━??：1?：|????,3?└：o???D?：|y??
        return FOTA_RETRY;
    }

exit:
    return ret;
}

int ty_ota_write_flash(char* data, unsigned int size)
{
    return ota_write_flash (data, size);
}

void ty_ota_update_start(void)
{
    ota_update_start();
}

bool ty_ota_update_Init(void)
{
    return ota_update_Init ();
}

void ty_ota_update_deInit(void)
{
    ota_update_deInit();
}

bool ty_ota_update_Hmac(char* data, unsigned int size)
{
    ota_update_Hmac(data,size);
}

bool ty_ota_check_FwValidate(void)
{
    return ota_check_FwValidate();
}

