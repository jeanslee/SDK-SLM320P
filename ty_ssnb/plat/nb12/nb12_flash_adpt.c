#include "ty_flash_adpt.h"
#include "softap_api.h"
#include "ty_os_adpt.h"

#define USER_FLASH_BASE_BAKCUP_OFFSET 0x1000

bool ty_flash_check_factory()
{
    if(*((unsigned int*)(USER_FLASH_BASE)) == 0XFFFFFFFF)
        return true;
    else
        return false;
}

void ty_flash_read(void *data, int size)
{
    xy_Flash_Read( USER_FLASH_BASE, data,  size);
}


int ty_flash_write(void *data, int size)
{
    return xy_Flash_Write( USER_FLASH_BASE, data,  size);
}


//user备份分区写入数据
int ty_flash_write_backup(void *data, int size)
{
    return xy_Flash_Write( USER_FLASH_BASE+USER_FLASH_BASE_BAKCUP_OFFSET, data,  size);
}

//user备份分区读取数据
void ty_flash_read_backup(void *data, int size)
{
    xy_Flash_Read( USER_FLASH_BASE+USER_FLASH_BASE_BAKCUP_OFFSET, data,  size);
}



