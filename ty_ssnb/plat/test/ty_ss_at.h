#ifndef __TY_AT_SS_H__
#define __TY_AT_SS_H__

#include "stdint.h"


//AT����ָ��궨��

#define TEST_AT_PROFILE_PRINT 0 //�����ǰģ��������Ϣ

#define TEST_AT_PDU_SMS_WRITE 1 //PDUд����

#define TEST_AT_PDU_SMS_READ 2 //PDUд����

#define TEST_AT_HTTPS_POST_SPLIT  3 //POS����Э���Ƭ����

#define TEST_AT_FOTA_FW_DOWNLOAD_SPLIT  4 //�̼���Ƭ���ز���

#define TEST_AT_TASK_DOWN_JSON_PARSER  5 //��������JSON���ݽ�������

#define TEST_AT_PRINT_COMPILE_DATETIME  6 //����ʱ���ӡ

#define TEST_AT_LPA_OPTIMIZE_TASK  8 //�����ѡ������������

#define TEST_AT_MBEDTLS_DEBUG_CONFIG  9 //mbetdtls��ӡ��������

#define TEST_AT_DSP_LOCK  11 //����DSP����

#define TEST_AT_DSP_UNLOCK  12 //����DSP�ͷ���

#define TEST_AT_RTC_WAKUP_FUCNTION  13 //RTC˯�߻��Ѳ���

#define TEST_AT_PRINT_MODULE_INFO  14 //ģ����Ϣ

#define TEST_AT_LOAD_DEVICESECRET_INFO  16 //��Ԫ���豸��Կ��ӡ

#define TEST_AT_UPDATE_DEVICESECRET_INFO  17 //������Ԫ���豸��Կ

#define TEST_AT_MQTT_REPORT_UP  19 //�ֶ�MQTTȫ���ϱ�

#define TEST_AT_PUT_MSG_TO_QUEUE_TASK  20 //�ֶ�������Ϣ����������

#define TEST_AT_PUT_MSG_TO_MAIN_TASK  21 //�ֶ�������Ϣ����������

#define TEST_AT_HTTPS_POST_KEEPALIVE  22 //HTTPS ������POST����

#define TEST_AT_HTTPS_POST_KEEPALIVE_CLOSE  27 //�ر�HTTPS ������POST����

#define TEST_AT_SAM_HTTPS  23 //SAM����/�������

#define TEST_AT_TSM_HTTPS  24 //TSM���ء����˻��������ɾ������

#define TEST_AT_GET_APPLET_STATUS_VERSION  25 //��ȡӦ��״̬�Ͱ汾

#define TEST_AT_PRINT_AID_INFO  28 //AIDӦ����Ϣ��ӡ

#define TEST_AT_SAVE_AID_INFO  29 //�ֶ�д�뱣��AIDӦ����Ϣ

#define TEST_AT_ALIOS_HTTP_POST 30 //ALIOS HTTP POST�ӿڲ���

#define TEST_AT_LINKSDK_HTTP_POST 31  //LINKSDK HTTP POST�ӿڲ���

#define TEST_AT_LOADSIM_PROFILE 32  //���������Ϣ�ӿڲ���

#define TEST_AT_ERASE_ALLAID_INFOS 33  //��������AIDӦ����Ϣ

#define TEST_AT_DUMP_MEMLEAK_INFO 34  //��ӡ�ڴ�й©��Ϣ

#define TEST_AT_MALLOC_ERR_TEST 35  //�쳣���ԣ�ѭ���ڴ��������(���ͷ�)

#define TEST_AT_PUT_TSMSAM_TASK_UP_TO_MAINTASK 37  //�ֶ����������ϱ���������������

#define TEST_AT_PUT_FOTATASK_TO_QUEUETASK 38  //�ֶ�����FOTA��������

#define TEST_AT_MQTT_OFFLINE        39  //�ֶ�����

#define TEST_AT_MQTT_ONLINE         40  //�ֶ���������

#endif