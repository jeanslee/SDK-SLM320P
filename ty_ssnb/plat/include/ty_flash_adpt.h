#ifndef __TY_FLASH_ADPT_H__
#define __TY_FLASH_ADPT_H__

#include "stdbool.h"

//ƽ̨����ӿ�

//���FLASH����״̬(user��������)
bool ty_flash_check_factory();

//user����д������
int ty_flash_write(void *data, int size);

//user������ȡ����
int ty_flash_read(void *data, int size);

//user���ݷ���д������
int ty_flash_write_backup(void *data, int size);

//user���ݷ�����ȡ����
int ty_flash_read_backup(void *data, int size);


#endif

