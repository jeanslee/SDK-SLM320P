#ifndef __TY_FLASH_ADPT_H__
#define __TY_FLASH_ADPT_H__

#include "stdbool.h"

//平台适配接口

//检查FLASH出厂状态(user分区擦除)
bool ty_flash_check_factory();

//user分区写入数据
int ty_flash_write(void *data, int size);

//user分区读取数据
int ty_flash_read(void *data, int size);

//user备份分区写入数据
int ty_flash_write_backup(void *data, int size);

//user备份分区读取数据
int ty_flash_read_backup(void *data, int size);


#endif

