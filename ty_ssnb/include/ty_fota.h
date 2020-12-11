#ifndef __TY_FOTA_H__
#define __TY_FOTA_H__

#include "stdint.h"

#include "ty_mqtt_basic_info.h"

typedef struct
{
    const char *ID;
    const char *Ver;
    const char *taskNo;
    ty_task_fota fota;
}ty_task_fota_wrapper;


typedef struct  {
    int     _unused;    /** FIFO uses first 4 bytes itself, reserve space */
    int     last_flag;  /*0 not, 1 yes*/
    int     data_len;
    unsigned char    *data; 
}ty_fota_queue_msg;

#define TY_FOTA_TASK_MSGQ_SIZE 16

typedef enum
{
    FOTA_RETRY = -1,
    FOTA_OK = 0,
    FOTA_UPGRADED = 1,
    FOTA_VERSION_ERR = 2,
    FOTA_PARAMS_ERR = 3,
    FOTA_TASK_FAIL = 4,
    FOTA_DLD_ERR = 5,
    FOTA_UPD_OK = 6,
    FOTA_UPD_FAIL = 7,
}FOTA_ERR_CODE;


#define FOTA_TASK_VALID             (0xA5 << 0)       //è????′?′DDíê
#define FOTA_TASK_INVALID           (0x5A << 0)       //è????′DDíê

#define FOTA_TASK_UPLOADED          (0xA5 << 8)      //ò?é?±¨
#define FOTA_TASK_NO_UPLOAD         (0x5A << 8)      //?′é?±¨

#define FOTA_DOWNLOAD_PKG_SIZE      2048


typedef enum
{
    E_FOTA_MASK_VALID =  (0xFF<<0),
    E_FOTA_MASK_UPLOAD = (0xFF<<8),
}E_NvTaskMask;

#define FOTA_HTTP_GET_FW_RETRY_CNT          5 //单次下载断点续传连续重试次数

#define FIRMWARE_FOTA_START                 "\r\n+FIRMWARE START\r\n"
#define FIRMWARE_FOTA_DOWNLOADING           "\r\n+FIRMWARE DOWNLOADING\r\n"
#define FIRMWARE_FOTA_DOWNLOADED            "\r\n+FIRMWARE DOWNLOADED\r\n"
#define FIRMWARE_FOTA_DOWNLOAD_FAILED       "\r\n+FIRMWARE DOWNLOAD FAILED\r\n"
#define FIRMWARE_FOTA_UPDATE_FAILED         "\r\n+FIRMWARE UPDATE FAILED\r\n"
#define FIRMWARE_FOTA_UPDATE_SUCCESS        "\r\n+FIRMWARE UPDATE SUCCESS\r\n"

extern const char *ty_http_ca_cert;

int ty_CheckFota(ty_task_fota_wrapper *nv_fota);

int ty_MgrModuleFota(const char *ID,const char *ver, const char *taskNo,ty_task_fota *fota);

int ty_LoadFOTAInfo(ty_task_fota_wrapper *fota);

int ty_SaveFOTAInfo(ty_task_fota_wrapper *fota);

int ty_UpdateFOTAInfo(int fota_erase);

int ty_EraseFOTAInfo();

int ty_FotaUploadErrMsg(const char* url,const char *task_no,int errcode);


#endif


