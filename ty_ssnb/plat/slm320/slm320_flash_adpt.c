#include "ty_flash_adpt.h"
#include "ty_os_adpt.h"
#include "mg_fs_api.h"
#include "vfs.h"
#define USER_FLASH_BASE_BAKCUP_OFFSET 0x1000
#define USER_FLASH_FILE "/nvm/user_nv.bin"
#define BAK_USER_FLASH_FILE "/nvm/user_nv.bin.bak"
bool ty_flash_check_factory()
{
    struct stat temp = {0};
    struct stat tempBak = {0};
    if(MG_FS_Stat(USER_FLASH_FILE, &temp)== 0 || MG_FS_Stat(BAK_USER_FLASH_FILE, &tempBak )== 0){
#if ADPT_DEBUG
        LOG("get file stat success, file st_mode : %d", temp.st_mode);
        LOG("get file stat success, file st_size : %ld", temp.st_size);

        LOG("get file stat success, bakfile st_mode : %d", tempBak.st_mode);
        LOG("get file stat success, bakfile st_size : %ld", tempBak.st_size);
        
#endif
        return false;
    }else{
#if ADPT_DEBUG
        LOG( "get stat failed");
#endif
        return true;   //不存在为出厂状态
    }
}

int ty_flash_read(void *data, int size)
{
    int ret =  MG_FS_Read(USER_FLASH_FILE, data, size);
    if(ret == -1)
    {
        LOG("read error");
        return ret;
    }
    LOG( "%s ret=%d\r\n",__FUNCTION__,ret);
    return ret == size ? 0 : -1;
}

int ty_flash_write(void *data, int size)
{
    int ret;
    int fd = MG_FS_Creat(USER_FLASH_FILE, 777);
    if (fd > 0)
    {
#if ADPT_DEBUG 
        LOG( "create and open done");
#endif
        if(MG_FS_Close(fd) == 0){
#if ADPT_DEBUG            
            LOG("close success");
#endif
        }else{
#if ADPT_DEBUG
            LOG("close failed");
#endif
        }
        ret =  MG_FS_Write(USER_FLASH_FILE, data, size);
        LOG( "%s ret=%d\r\n",__FUNCTION__,ret);
        return ret == size ? 0 : -1;
    }
    else
        return -1;
}

//user备份分区写入数据
int ty_flash_write_backup(void *data, int size)
{
    int ret ;
    int fd = MG_FS_Creat(BAK_USER_FLASH_FILE, 777);
    if (fd > 0)
    {
#if ADPT_DEBUG    
        LOG( "create and open done");
#endif
        if(MG_FS_Close(fd) == 0){
#if ADPT_DEBUG            
            LOG("close success");
#endif
        }else{
#if ADPT_DEBUG        
            LOG("close failed");
#endif
        }
        ret =  MG_FS_Write(BAK_USER_FLASH_FILE, data, size);
        LOG( "%s ret=%d\r\n",__FUNCTION__,ret);
        return ret == size ? 0 : -1;
    }
    else
        return -1;
}

//user备份分区读取数据
int ty_flash_read_backup(void *data, int size)
{   
    int ret =  MG_FS_Read(BAK_USER_FLASH_FILE, data, size);
    if(ret == -1)
    {
        LOG("read error");
        return ret;
    }
    LOG( "%s ret=%d\r\n",__FUNCTION__,ret);
    return ret == size ? 0 : -1;
    
}