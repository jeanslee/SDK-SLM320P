#ifndef __TY_AT_ADPT_H__
#define __TY_AT_ADPT_H__

#include "stdint.h"

//SIM�����µ�ӿ�
int at_sim_reset();

//��ȡIMEI�ӿ�
int at_imei(char *IMEI);

//��ȡICCID�ӿ�
int at_iccid(char *iccid);

//��ȡIMSI�ӿ�
int at_imsi(char *imsi);

//��ȡCEREG����ע��״̬
int at_creg();

//�ⲿ��AT��Ӧ�ӿ�
int ty_send_rsp_str_to_ext (char *rsp);

//��ȡ�ź�ǿ��
int ty_get_radio_signal_strength();

//��־��ӡ���ڲ���������
//void ty_log_uart_set_baud(int baud);


//����AT�ص��������ⲿ���������ݣ�����*rsp_cmd��Ӧ����
//���ڲ�ͬƽ̨����ATʵ�ַ�ʽ��һ�����޷�����ͳһ����
//typedef int (*ty_ser_req_func)(char *at_paras, char **rsp_cmd);

//void ty_register_app_at_req(char *at_prefix, ty_ser_req_func func);



#endif

