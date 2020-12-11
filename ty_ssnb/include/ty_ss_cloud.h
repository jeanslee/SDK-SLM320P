#ifndef __TY_SS_CLOUD_H__
#define __TY_SS_CLOUD_H__

#include "ty_devinfo.h"
#include "ty_mqtt_basic_info.h"
#include "ty_http_adpt.h"
#include "ty_mqtt_adpt.h"

#define MAX_TY_CLOUD_AT_SIZE   200 

//HTTP请求包大小
#define REQ_BUF_SIZE 2048
//HTTP包回应包大小
#define RSP_BUF_SIZE 4096
#define HTTP_UP_HDR_SIZE 256

#define HTTP_Content_Type_JSON      1   
#define HTTP_Content_Type_X_Form    2


/*
1 FOTA固件升级
2 远程发行
3 远程发行激活
4 TSM应用下载
5 TSM 应用删除
*/
typedef enum
{
    e_cloud_task_fota = 1,
    e_cloud_task_se_publish = 2,
    e_cloud_task_se_active = 3,
    e_cloud_task_tsm_download = 4,
    e_cloud_task_tsm_personal = 5,
    e_cloud_task_tsm_active = 6,
    e_cloud_task_tsm_delete = 7,   
}e_cloud_task_type;

typedef struct 
{
	const char *host;
	const char *uri;
    const char *productKey;
    const char *deviceName;
    const char *productSecret;
    const char *cert;
    int cert_len;
}ty_mqtt_dev_reg_info;

int ty_LoadMqttCACert(char**cert,int *cert_len);

int ty_LoadHttpCACert(char**cert,int *cert_len);

int ty_mqtt_device_reg(ty_mqtt_dev_reg_info *dev ,char deviceSecret[64]);

int ty_LoadMqttCntInfo(ty_mqtt_cnt_info *info);

int ty_CalcMqttSignInfo(const char *productKey, const char *deviceName, const char *deviceSecret, 
                     char clientId[150], char username[64], char password[65]);

int ty_cloud_report_up_build(char **json,int *json_len,ty_cloudmsg_report_up *report);

int ty_cloud_report_down_parser (char *json,ty_cloudmsg_report_down *down);

int ty_cloud_task_down_parser (char *json,ty_cloudmsg_task_down *task);

int ty_cloud_task_up_build (char **json,int *json_len,ty_cloudmsg_task_up *up);

int ty_cloud_offline_up_build(char **json,int *json_len,ty_cloudmsg_offline_up *offline);

char *http_post_util(const char *host,const char* uri,char* params,const char *cert,int content_type,int *code);
    
char *http_post(const char *host,const char* uri,char* params,const char *cert);

char *http_post_dynreg(const char *cert,int cert_len,const char* host, const char* uri, char* param);

int http_url_paser_host_port(char *server_name,char *host ,int hlen, int *port ,int *secure_tls);

#endif