#ifndef _TY_MQTT_BASIC_INFO_H_
#define _TY_MQTT_BASIC_INFO_H_

#include "ty_devinfo.h"
#include "stdint.h"
#include "cJSON.h"

#if defined(__cplusplus)
extern "C" {
#endif



typedef enum
{
    E_QueueMsg_Cloud = 1, //
    E_QueueMsg_Local = 2, //
}E_QueueMsgType;


typedef enum
{
    E_LocalMsg_FotaCheck = 1, //
    E_LocalMsg_TaskUP, //TSM/SAM
    E_LocalMsg_FotaTaskUP, //FOTA
    E_LocalMsg_AtReportUP, //at report up
    E_LocalMsg_TestUP, //
}E_LocalMsgType;

typedef struct
{
    E_LocalMsgType type;
    char *data;
    int msglen;
}ty_localmsg; //

typedef struct
{
    const  char *start_version;
    const  char *target_version;
    const  char *fota_url;
	const  char *modrsp_url;
    int sign_type;
    const  char *sign_value;
    int retry_strategy;
}ty_task_fota;

typedef struct
{
    const char *tsm_url;
    const char *session_key;
	const char *app_aid;
	int retry_strategy;
}ty_task_tsm;

typedef struct
{
    const char *app_aid;
    const unsigned char *session_key;
    const char *sam_url;
	int retry_strategy;
}ty_task_sam;

typedef struct
{
    cJSON *root;//
    char *id;
    const char *version;
	const char *task_No;
    int task_type;
    union
    {
        ty_task_fota fota; 
        ty_task_sam  sam; 
        ty_task_tsm  tsm; 
    }params;
}ty_cloudmsg_task_down;


typedef struct{
	const char *bind_iccid;
	bool write_iccid;
	bool use_iccid;
}ty_code_No;

typedef struct
{
	int size;
	ty_code_No *code_no_list;
}ty_module_SIM;


typedef struct{
	const char *app_aid;
	const char *app_status;  
	const char *app_version;
	const char *app_life_cycle_state;
}ty_AID;

typedef struct
{
    const char *SEID;   //SE安全载体的唯一标识
    int rest_space;      //单位字节
    int AID_size;
	ty_AID *AID_list;
}ty_module_SE_TSM;

typedef struct
{
    const char *id;
    const char *version;
    const char *code;

    const char *task_no;
    int task_type;
    
    bool fota_msg;
    bool sam_tsm_msg;

    union
    {
        ty_module_inf module_inf;
        ty_module_SE_TSM module_SE_TSM;
    }msg;
}ty_cloudmsg_task_up; //



typedef struct
{
    const char * id;
    const char *version;
    ty_module_inf module_inf;
    ty_module_SIM module_SIM;
	ty_module_SE_TSM module_SE_TSM;  //SE和TSM应用信息
}ty_cloudmsg_report_up; //模组连接全量上报消息


typedef struct
{
    cJSON *root;
    const char *id;
    const char *version;
    const char *code;       //200为成功，其他为失败
    ty_module_inf module_inf;
    ty_module_SIM module_SIM;
	ty_module_SE_TSM module_SE_TSM;  //SE和TSM应用信息
}ty_cloudmsg_report_down; //平台收到全量信息响应


typedef struct
{
    const char *id;
    const char *version;
    int type;
}ty_cloudmsg_offline_up; //模组离线消息



typedef struct
{
  E_QueueMsgType type;
  union
  {
    ty_localmsg localmsg;
    ty_cloudmsg_task_down cloudmsg;
  }msg;
} ty_queue_msg;


typedef int(*ty_http_load_cert_cb_t)(char** cert,int *len);

typedef char *(*ty_http_post_cb_t)(const char *cert,int cert_len,const char* host, const char* uri, char* param);


#if defined(__cplusplus)
}
#endif

#endif
