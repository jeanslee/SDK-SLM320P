#include "ty_fota_adpt.h"
#include "ty_devinfo.h"
#include "ty_os_adpt.h"
#include "sci_types.h"
#include "mg_fs_api.h"

#define FOTA_UPDATA_FILE "/fota/app_fota.bin"
static uint32 fota_offset = 0;

int ty_ota_nv_check_result(ty_task_fota_wrapper *nv_fota)
{
    LOG("%s\r\n",__FUNCTION__);
    return FOTA_UPD_OK;
}

int ty_ota_write_flash(char* data, unsigned int size)
{

        int flag = 0;
        int fd = MG_FS_Open(FOTA_UPDATA_FILE, 2);
        off_t cur = MG_FS_Seek(fd, 0, 1);
//            LOG("%s: %d offset before seek: %ld", __func__, __LINE__, cur);
       
        MG_FS_Seek(fd, fota_offset, 0);
        cur = MG_FS_Seek(fd, 0, 1);
//            LOG("%s: %d offset after seek: %ld", __func__, __LINE__, cur);
        flag = vfs_write(fd, data, size);
        
        fota_offset += flag;
#ifdef ADPT_DEBUG
        LOG("%s: %d fota_offset = %d", __func__, __LINE__, fota_offset);
#endif
        MG_FS_Close(fd);

}

void ty_ota_update_start(void)
{
//   LOG("%s\r\n",__FUNCTION__);
//   opencpu_fota_upgrade();
}

void ty_ota_update_read()
{

    char buf[1000] = {0};
    MG_FS_Read(FOTA_UPDATA_FILE, buf, sizeof(buf));
    LOG("read : %s", buf);
}

bool ty_ota_update_Init(void)
{
    int fd = MG_FS_Creat(FOTA_UPDATA_FILE, 777);
    fota_offset = 0;
    if (fd > 0)
     {
#if ADPT_DEBUG 
            LOG( "create and open done");
#endif
            if(MG_FS_Close(fd) == 0){
#if ADPT_DEBUG            
                LOG("close success");
                return true;
#endif
            }else{
#if ADPT_DEBUG
                LOG("close failed");
                return false;
#endif
            }
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
    return true;
}

bool ty_ota_check_FwValidate(void)
{
#if 1
    LOG("%s\r\n",__FUNCTION__);
    if(!MG_updateFirmware(FOTA_UPDATA_FILE))
    {
       LOG("mg update failed");
       return false;
    }
	
    ty_sleep(1000);
    ty_sleep("mg update end");
#endif

    return true;
}

