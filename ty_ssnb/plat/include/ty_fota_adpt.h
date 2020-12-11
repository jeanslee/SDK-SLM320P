#ifndef __TY_FOTA_ADPT_H__
#define __TY_FOTA_ADPT_H__

#include "ty_fota.h"

//平台检查OTA升级状态
int ty_ota_nv_check_result(ty_task_fota_wrapper *nv_fota);

//OTA升级初始化
bool ty_ota_update_Init(void);

//OTA 固件写FLASH
int ty_ota_write_flash(char* data, unsigned int size);

//OTA 固件分片HMAC数据校验
bool ty_ota_update_Hmac(char* data, unsigned int size);

//固件整体校验
bool ty_ota_check_FwValidate(void);

//启动升级
void ty_ota_update_start(void);

//OTA 反初始化
void ty_ota_update_deInit(void);

#endif
