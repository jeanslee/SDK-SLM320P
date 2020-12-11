#ifndef __TY_AT_ADPT_H__
#define __TY_AT_ADPT_H__

#include "stdint.h"

//SIM卡上下电接口
int at_sim_reset();

//获取IMEI接口
int at_imei(char *IMEI);

//获取ICCID接口
int at_iccid(char *iccid);

//获取IMSI接口
int at_imsi(char *imsi);

//获取CEREG网络注册状态
int at_creg();

//外部发AT响应接口
int ty_send_rsp_str_to_ext (char *rsp);

//获取信号强度
int ty_get_radio_signal_strength();

//日志打印串口波特率设置
//void ty_log_uart_set_baud(int baud);


//定制AT回调，传入外部的请求数据，传出*rsp_cmd响应数据
//由于不同平台定制AT实现方式不一样，无法做到统一适配
//typedef int (*ty_ser_req_func)(char *at_paras, char **rsp_cmd);

//void ty_register_app_at_req(char *at_prefix, ty_ser_req_func func);



#endif

