#ifndef __TY_AT_SS_H__
#define __TY_AT_SS_H__

#include "stdint.h"


//AT测试指令宏定义

#define TEST_AT_PROFILE_PRINT 0 //输出当前模组的码号信息

#define TEST_AT_PDU_SMS_WRITE 1 //PDU写测试

#define TEST_AT_PDU_SMS_READ 2 //PDU写测试

#define TEST_AT_HTTPS_POST_SPLIT  3 //POS请求协议分片测试

#define TEST_AT_FOTA_FW_DOWNLOAD_SPLIT  4 //固件分片下载测试

#define TEST_AT_TASK_DOWN_JSON_PARSER  5 //任务下行JSON数据解析测试

#define TEST_AT_PRINT_COMPILE_DATETIME  6 //编译时间打印

#define TEST_AT_LPA_OPTIMIZE_TASK  8 //码号优选任务启动测试

#define TEST_AT_MBEDTLS_DEBUG_CONFIG  9 //mbetdtls打印级别设置

#define TEST_AT_DSP_LOCK  11 //基带DSP上锁

#define TEST_AT_DSP_UNLOCK  12 //基带DSP释放锁

#define TEST_AT_RTC_WAKUP_FUCNTION  13 //RTC睡眠唤醒测试

#define TEST_AT_PRINT_MODULE_INFO  14 //模组信息

#define TEST_AT_LOAD_DEVICESECRET_INFO  16 //三元组设备密钥打印

#define TEST_AT_UPDATE_DEVICESECRET_INFO  17 //更新三元组设备密钥

#define TEST_AT_MQTT_REPORT_UP  19 //手动MQTT全量上报

#define TEST_AT_PUT_MSG_TO_QUEUE_TASK  20 //手动推送消息给队列任务

#define TEST_AT_PUT_MSG_TO_MAIN_TASK  21 //手动推送消息给队列任务

#define TEST_AT_HTTPS_POST_KEEPALIVE  22 //HTTPS 长连接POST测试

#define TEST_AT_HTTPS_POST_KEEPALIVE_CLOSE  27 //关闭HTTPS 长连接POST测试

#define TEST_AT_SAM_HTTPS  23 //SAM发行/激活测试

#define TEST_AT_TSM_HTTPS  24 //TSM下载、个人化、激活和删除测试

#define TEST_AT_GET_APPLET_STATUS_VERSION  25 //获取应用状态和版本

#define TEST_AT_PRINT_AID_INFO  28 //AID应用信息打印

#define TEST_AT_SAVE_AID_INFO  29 //手动写入保存AID应用信息

#define TEST_AT_ALIOS_HTTP_POST 30 //ALIOS HTTP POST接口测试

#define TEST_AT_LINKSDK_HTTP_POST 31  //LINKSDK HTTP POST接口测试

#define TEST_AT_LOADSIM_PROFILE 32  //加载码号信息接口测试

#define TEST_AT_ERASE_ALLAID_INFOS 33  //擦除所有AID应用信息

#define TEST_AT_DUMP_MEMLEAK_INFO 34  //打印内存泄漏信息

#define TEST_AT_MALLOC_ERR_TEST 35  //异常测试，循环内存申请测试(不释放)

#define TEST_AT_PUT_TSMSAM_TASK_UP_TO_MAINTASK 37  //手动推送任务上报结果给主任务队列

#define TEST_AT_PUT_FOTATASK_TO_QUEUETASK 38  //手动推送FOTA升级任务

#define TEST_AT_MQTT_OFFLINE        39  //手动下线

#define TEST_AT_MQTT_ONLINE         40  //手动重新上线

#endif