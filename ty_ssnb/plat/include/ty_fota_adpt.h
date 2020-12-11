#ifndef __TY_FOTA_ADPT_H__
#define __TY_FOTA_ADPT_H__

#include "ty_fota.h"

//ƽ̨���OTA����״̬
int ty_ota_nv_check_result(ty_task_fota_wrapper *nv_fota);

//OTA������ʼ��
bool ty_ota_update_Init(void);

//OTA �̼�дFLASH
int ty_ota_write_flash(char* data, unsigned int size);

//OTA �̼���ƬHMAC����У��
bool ty_ota_update_Hmac(char* data, unsigned int size);

//�̼�����У��
bool ty_ota_check_FwValidate(void);

//��������
void ty_ota_update_start(void);

//OTA ����ʼ��
void ty_ota_update_deInit(void);

#endif
