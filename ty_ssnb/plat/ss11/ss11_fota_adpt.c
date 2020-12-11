#include "ty_fota_adpt.h"
#include "ty_devinfo.h"
#include "ty_os_adpt.h"

static uint32 offset = 0;

int ty_ota_nv_check_result(ty_task_fota_wrapper *nv_fota)
{
    LOG("%s\r\n",__FUNCTION__);
    return 0;
}

int ty_ota_write_flash(char* data, unsigned int size)
{
    LOG("%s\r\n",__FUNCTION__);
    int ret = 0;
    ret = opencpu_fota_write(offset, data, (uint16)size);
    if(ret = 0){
        offset += size;
        }
    
    return 0;
}

void ty_ota_update_start(void)
{
   LOG("%s\r\n",__FUNCTION__);
   opencpu_fota_upgrade();
}

bool ty_ota_update_Init(void)
{
    LOG("%s\r\n",__FUNCTION__);
    int ret=0;
    ret = opencpu_fota_init();
    if(ret ==0){
        return true;
        }
    else 
        return false;
}

void ty_ota_update_deInit(void)
{
   LOG("%s\r\n",__FUNCTION__);
}

bool ty_ota_update_Hmac(char* data, unsigned int size)
{
    LOG("%s\r\n",__FUNCTION__);
    return false;
}

bool ty_ota_check_FwValidate(void)
{
    LOG("%s\r\n",__FUNCTION__);
    int ret  = 0;
    ret = opencpu_fota_validate();
    if(ret = 0){
        return true;
        }
    else
        return false;
}

