#ifndef __TY_DEVINFO_ADPT_H__
#define __TY_DEVINFO_ADPT_H__

#include "stdbool.h"

#ifdef ALI_MQTT
#define MQTT_PRODUCT_KEY            "a1tyAZWeu86"
#define MQTT_PRODUCT_SECRET          "a1tyAZWeu86"

#define TY_CLOUD_MQTT_SERVER         "a1tyAZWeu86.iot-as-mqtt.cn-shanghai.aliyuncs.com"//"59.173.2.76"
#define TY_CLOUD_PORT               443 //3301//3600

#define TY_CLOUD_HTTP_DEVREG_URL         "https://iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define TY_CLOUD_HTTP_DEVREG_PORT        443

#else

//ProductKey
//#define MQTT_PRODUCT_KEY            "378e776ed031483491da2bd39aaa8f2e"
//#define MQTT_PRODUCT_SECRET          "9e99ef8a0e114b4497bbea8c70582b17"
//#define TY_CLOUD_MQTT_SERVER        "yfzx.whty.com.cn"
//#define TY_CLOUD_PORT               3710 //3301//3600

//#define TY_CLOUD_HTTP_DEVREG_URL         "https://yfzx.whty.com.cn/dev-iot-rq/services/iot-module/auth/register/device"
#define TY_CLOUD_HTTP_DEVREG_PORT        443

#endif

#define TY_CLOUD_MQTT_CLEANSESSION   1
#define TY_CLOUD_MQTT_KEEPALIVE      60
#define TY_CLOUD_MQTT_TLS           1

#define DEV_FW_VER_MAX_LEN          7

#define APPLET_MAX_SIZE  20
#define NV_WRITE_RETRY_CNT 3

#define SAM_AID "4E9A534E6C148868"

typedef enum
{
    e_flash_status_crc_ok = 1,//主分区校验正常
    e_flash_status_main_rebuild = 2,//主分区重写
    e_flash_status_bak_newer = 3,//备份区数据新
    e_flash_status_main_bak_err = 4,//主、备分区数据异常
    e_flash_status_bak_rebuild = 5,//备份区重写
    e_flash_status_main_newer = 6,//主份区数据新
    e_flash_status_unkown = 7,
}e_flash_status;



typedef enum
{
    e_app_life_cycle_selectable = 1, //0x07??(SELECTABLE)
    e_app_life_cycle_lock = 2, //x83??
}e_app_life_cycle_state;

typedef enum
{
    e_app_status_no_personal = 1,//1、未个人化
    e_app_status_personal = 2,//2、已个人化
    e_app_status_active = 3,//3、已激活
    e_app_status_nv_erase = 0xFF,//NV出厂
}e_app_status;

typedef struct
{
    unsigned char app_status;
    unsigned char app_life_cycle_state;
    char app_version[13];
    char app_aid[33];
}sm_aid;


typedef struct
{
    int aid_size;
    sm_aid *list;
}Aid_List;


typedef struct {
	unsigned int  write_to_flash_num;   //write to flash times,and if FFFFFFFF,flash context is invalid

	
	int  g_attach_time;// = 60;
	int  g_attach_retry;// = 3;
	int  g_regist_time;// = 30;
	int  g_wait_down_data_time;// = 30;
	int  g_working_time;// = (60*5);
	int  g_next_work_time;// = 60;
	int  g_send_retry;//3
	int  g_send_num;// = 2;

    
	int  g_devreg_status;// = 2;
	char devicesecret[64];
    struct
    {
        char fota_taskID[33];//
        char fota_taskVer[8];//
        char fota_taskNo[21];//
        char fota_currver[DEV_FW_VER_MAX_LEN];//Vxx.xxx
        char fota_targetver[DEV_FW_VER_MAX_LEN];//Vxx.xxx
        int  fota_sigalg;
        char fota_sig[64];
        char fota_url[128];
        char modrsp_url[128];
        int  fota_retrycnt;
        int  fota_erase;
    }nv_fota;
    
    struct
    {
        sm_aid sm_aid_list[APPLET_MAX_SIZE];
    }nv_aids;

	int  g_send_char[10];// = {10,200};
	char g_server_ip[16];
	char g_server_auth[16];	
	int  g_server_port;
	
	int  g_cur_state;//0,first power on;1,attach;2,regist;3,send;
	int  g_fail_num;
	int  g_succ_num;
	int  g_cur_attch_retry;  //attach fail,will reboot,so must write to flash
	int  g_attach_retry_num;
	int  g_regist_retry_num;
	int  g_rcv_fail_num;
	int  g_rcv_succ;
    unsigned int crc32;
}ty_cloud_nv_t;

typedef struct
{
   const char *product_type;
   const char *bind_imei;
   const char *firmware_version;
}ty_module_inf;


#ifndef SSNB_BUILD_LIB

#ifndef PRODUCT_TYPE
#error "no define macro PRODUCT_TYPE"
#endif

#ifndef FIRMWARE_VERSION
#error "no define macro FIRMWARE_VERSION"
#endif

#ifndef MQTT_PRODUCT_KEY
#error "no define macro MQTT_PRODUCT_KEY"
#endif

#ifndef MQTT_PRODUCT_SECRET
#error "no define macro MQTT_PRODUCT_SECRET"
#endif

#ifndef TY_CLOUD_MQTT_SERVER
#error "no define macro TY_CLOUD_MQTT_SERVER"
#endif


#ifndef TY_CLOUD_PORT
#error "no define macro TY_CLOUD_PORT"
#endif

#ifndef TY_CLOUD_HTTP_DEVREG_URL
#error "no define macro TY_CLOUD_HTTP_DEVREG_URL"
#endif

#else
//由外部链接调用库定义这些全局变量
extern const char *PRODUCT_TYPE;
extern const char *FIRMWARE_VERSION;
extern const char *MQTT_PRODUCT_KEY;
extern const char *MQTT_PRODUCT_SECRET;
extern const char *TY_CLOUD_MQTT_SERVER;
extern int TY_CLOUD_PORT;
extern const char *TY_CLOUD_HTTP_DEVREG_URL;
#endif




int ty_LoadModueInfo(ty_module_inf *info) ;
int ty_UpdateModueInfo(ty_module_inf *info) ;
int ty_LoadDeviceInfo(char productKey[20],char proudctSecret[64], char deviceName[32]);
int ty_SaveDeviceSecret(char deviceSecret[64]);
int ty_LoadDeviceSecret(char deviceSecret[64]);
int ty_LoadDeviceRegUrl(char url[128],int *port);


//查找AID信息
int ty_FindAppletInfo(char *str_aid);

//保存AID
int ty_SaveAppletInfo(sm_aid *aid);

//加载所有AID信息
int ty_LoadAppletInfoList(Aid_List **list);

//释放AID列表
void ty_AppletInfoListFree(Aid_List *list);

//删除单个AID
int ty_EraseAppletInfo(sm_aid *aid);

int ty_EraseAllAppletInfo();

int ty_getTsmAppletInfo(char* aid,sm_aid* tsmAppInfo);

//双备份数据保存
int ty_flash_save(void *data, int size);

void  read_user_nv_flash();

void  save_user_nv_flash();

#endif
