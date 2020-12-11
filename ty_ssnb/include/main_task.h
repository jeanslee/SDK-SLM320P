#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__


typedef enum
{
    E_MainTaskInit,
    E_MainTaskCereg,//驻网
    E_MainTaskCgpAddr,//网络地址分配
    E_MainTaskDisconnect,
    E_MainTaskConnected ,
    E_MainTaskReportUp,
    E_MainTaskWaitReportDown,//监听上报响应
    E_MainTaskWaitTaskUP,//监听业务处理任务上报
    E_MainTaskWaitTaskDown,//下行任务
    E_MainTaskFota,//FOTA升级，业务停止状态
    E_MainTaskSleep,//超时无下行任务时
    E_MainTaskExit,//退出状态
}E_MainTaskState;


#define TOPIC_MAX_LEN                       128
#ifdef ALI_MQTT
#define TY_CLOUD_REPORT_DOWN_TOPIC          "/%s/%s/user/cloudmsg/report_reply"
#define TY_CLOUD_TASK_DOWN_TOPIC            "/%s/%s/user/cloudmsg/down"

#define TY_CLOUD_REPORT_UP_TOPIC            "/%s/%s/user/devmsg/report"
#define TY_CLOUD_TASK_UP_TOPIC              "/%s/%s/user/devmsg/up"
#else
#define TY_CLOUD_REPORT_DOWN_TOPIC          "iot/%s/%s/user/cloudmsg/report_reply"
#define TY_CLOUD_TASK_DOWN_TOPIC            "iot/%s/%s/user/cloudmsg/down"

#define TY_CLOUD_REPORT_UP_TOPIC            "iot/%s/%s/user/devmsg/report"
#define TY_CLOUD_TASK_UP_TOPIC              "iot/%s/%s/user/devmsg/up"

#define TY_CLOUD_OFFLINE_TOPIC              "iot/%s/%s/user/devmsg/dormancy"

#endif

#define WAIT_REPORTDOWN_RESPONSE_MAX        (30) //sencond

#define WAIT_IDLE_TIMEOUT                   (180) //sencond

#define WAIT_TASKUP_TIMEOUT                   (20) //sencond

#define SIGNAL_PERIOD_LOG_TIME                (30) //信号强度打印周期

#define MQTT_YIELD_PERIOD_LOG_TIME            (30) //MQTT消息轮询周期,>30打印间隔时间

#define MQTT_REPORT_DOWN_ERR_CNT             3 //全量下行错误统计次数

#define MQTT_CONNECT_RETRY_CNT               1 //MQTT连接重试次数

#define MQTT_SUB_RETRY_CNT               1 //MQTT订阅重试次数


#define MAIN_TASK_QUEUE_TIMEOUT             1 //队列超时返回值
/*
启动升级	+FIRMWARE START
固件下载	+FIRMWARE DOWNLOADING
下载成功	+FIRMWARE DOWNLOADED
下载失败	+FIRMWARE DOWNLOAD FAILED
更新中	    +FIRMWARE UPDATING
校验失败	+FIRMWARE UPDATE FAILED
升级成功	+FIRMWARE UPDATE SUCCESS

*/

//上报外部MCU消息
#define HSM_TASK_START                      "\r\n+HSM UPDATING START\r\n"
#define HSM_TASK_STOP                       "\r\n+HSM UPDATING SUCCESS\r\n"


//上报外部MCU消息接口
int ty_cloud_ext_notify(const char *msg);

//FOTA升级通知
void ty_main_task_fota_notify (bool fota);

//FOTA重启通知
void ty_main_fota_reboot_notify (bool reboot);


//TSM SAM 通知
void ty_main_task_tsm_notify (bool tsm);


//业务task上报任务结果通知
int ty_main_task_put_taskup_msg(ty_queue_msg *msg);

//主task loop
void ty_main_task_loop (void * args);


//应用入口地址
void ty_main_task_entry();

//AT指令全量上报
int ty_at_mqtt_report_up();

//AT指令MQTT下线
int ty_at_mqtt_offline();

//AT指令MQTT重新上线
int ty_at_mqtt_resume();


//测试，获取模组信息全量上报
void main_report_up_test();


#endif
