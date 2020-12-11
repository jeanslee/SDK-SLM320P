#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__


typedef enum
{
    E_MainTaskInit,
    E_MainTaskCereg,//פ��
    E_MainTaskCgpAddr,//�����ַ����
    E_MainTaskDisconnect,
    E_MainTaskConnected ,
    E_MainTaskReportUp,
    E_MainTaskWaitReportDown,//�����ϱ���Ӧ
    E_MainTaskWaitTaskUP,//����ҵ���������ϱ�
    E_MainTaskWaitTaskDown,//��������
    E_MainTaskFota,//FOTA������ҵ��ֹͣ״̬
    E_MainTaskSleep,//��ʱ����������ʱ
    E_MainTaskExit,//�˳�״̬
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

#define SIGNAL_PERIOD_LOG_TIME                (30) //�ź�ǿ�ȴ�ӡ����

#define MQTT_YIELD_PERIOD_LOG_TIME            (30) //MQTT��Ϣ��ѯ����,>30��ӡ���ʱ��

#define MQTT_REPORT_DOWN_ERR_CNT             3 //ȫ�����д���ͳ�ƴ���

#define MQTT_CONNECT_RETRY_CNT               1 //MQTT�������Դ���

#define MQTT_SUB_RETRY_CNT               1 //MQTT�������Դ���


#define MAIN_TASK_QUEUE_TIMEOUT             1 //���г�ʱ����ֵ
/*
��������	+FIRMWARE START
�̼�����	+FIRMWARE DOWNLOADING
���سɹ�	+FIRMWARE DOWNLOADED
����ʧ��	+FIRMWARE DOWNLOAD FAILED
������	    +FIRMWARE UPDATING
У��ʧ��	+FIRMWARE UPDATE FAILED
�����ɹ�	+FIRMWARE UPDATE SUCCESS

*/

//�ϱ��ⲿMCU��Ϣ
#define HSM_TASK_START                      "\r\n+HSM UPDATING START\r\n"
#define HSM_TASK_STOP                       "\r\n+HSM UPDATING SUCCESS\r\n"


//�ϱ��ⲿMCU��Ϣ�ӿ�
int ty_cloud_ext_notify(const char *msg);

//FOTA����֪ͨ
void ty_main_task_fota_notify (bool fota);

//FOTA����֪ͨ
void ty_main_fota_reboot_notify (bool reboot);


//TSM SAM ֪ͨ
void ty_main_task_tsm_notify (bool tsm);


//ҵ��task�ϱ�������֪ͨ
int ty_main_task_put_taskup_msg(ty_queue_msg *msg);

//��task loop
void ty_main_task_loop (void * args);


//Ӧ����ڵ�ַ
void ty_main_task_entry();

//ATָ��ȫ���ϱ�
int ty_at_mqtt_report_up();

//ATָ��MQTT����
int ty_at_mqtt_offline();

//ATָ��MQTT��������
int ty_at_mqtt_resume();


//���ԣ���ȡģ����Ϣȫ���ϱ�
void main_report_up_test();


#endif
