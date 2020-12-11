/*****************************************************************************
* Copyright (c) 2019 ABUP.Co.Ltd. All rights reserved.
* File name: abup_fota.h
* Description:
* Author: WQH
* Version: v1.0
* Date: 20190903
*****************************************************************************/
#ifndef __ABUP_FOTA_H__
#define __ABUP_FOTA_H__
#include <stdint.h>

//MG ADD
typedef  struct ABUP_SERVER_INFO
{
	char *oem;
	char *model;
	char *product_id;
	char *product_secret;
	char *device_type;
	char *platform;

	char   abup_version[200];

	void (*abup_cb)(int event);

}abup_server_info_t;

extern abup_server_info_t abup_server_info;

typedef enum
{
	ABUP_FOTA_IDLE,
	ABUP_FOTA_RG,	//ע���豸��Ϣ
	ABUP_FOTA_CV,	//���汾
	ABUP_FOTA_DL,	//���ز�ְ�
	ABUP_FOTA_RD,	//���ؽ���ϱ�
	ABUP_FOTA_RU	//��������ϱ�
}ABUP_FOTA_STATUS;

typedef enum
{
    ABUP_ERROR_TYPE_NONE,
    ABUP_ERROR_TYPE_UART_TIMEOUT,// 1 //���ڳ�ʱ
    ABUP_ERROR_TYPE_NO_NETWORK,		//������	
    ABUP_ERROR_TYPE_DNS_FAIL,// 3	//DNS����ʧ��
    ABUP_ERROR_TYPE_CREATE_SOCKET_FAIL,
    ABUP_ERROR_TYPE_NETWORK_ERROR,// 5
    ABUP_ERROR_TYPE_NO_NEW_UPDATE,		//�����°汾
    ABUP_ERROR_TYPE_MD5_NOT_MATCH,// 7	//MD5У��ʧ��
    ABUP_ERROR_TYPE_NOT_ENOUGH_SPACE,	//�ռ䲻��
    ABUP_ERROR_TYPE_ERASE_FLASH,// 9
    ABUP_ERROR_TYPE_WRITE_FLASH,
    ABUP_ERROR_TYPE_NO_ACCESS_TIMES,// 11	//�����IMEI�ﵽ�����ʴ��� ,100��
    ABUP_ERROR_TYPE_UNKNOW,
}ABUP_ERROR_TYPE_ENUM;

//MG Add function
int MG_abup_create_fota_task(void);
void MG_abup_set_service_info(abup_server_info_t _abup_server_info);
void MG_abup_set_version(char  *_version);
char  * MG_abup_get_version(void);


void abup_create_fota_task(void);
void abup_task_exit(void);
void abup_free_dns(void);
void abup_close_socket(void);
int abup_socket_read_ex(char *buf, int len);
void abup_set_fota_ratio(uint16_t ratio);
uint32_t abup_get_fota_ratio(void);
void abup_exit_restore_context(void);
uint32_t abup_download_len(void);
uint32_t abup_socket_buff_len(void);
void abup_start_download_packet(void);
void abup_device_reboot(void);
char *abup_get_host_domain(void);
char *abup_get_host_port(void);
void abup_socket_buf_memory(void);
uint32_t abup_recv_buff_len(void);

#endif

