/*
* �������������`Linux`����֧��pthread��POSIX�豸, ����ʾ����SDK����HTTP�������ɹ���Ȩ, Ȼ��
*
* + ����ſ���ע�͵��� demo_http_post_lightswitch() ��������, ����ƽ̨�ϱ�1��HTTP��Ϣ
*
* ��Ҫ�û���ע���޸ĵĲ���, �Ѿ��� TODO ��ע���б���
*
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>

#include "aiot_http_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"
#include "ty_ss_cloud.h"

#define CONFIG_HTTPC_SERVER_NAME_SIZE 64

#define lk_msleep

/* λ��portfiles/aiot_port�ļ����µ�ϵͳ���亯������ */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
/* λ��external/ali_ca_cert.c�еķ�����֤�� */
extern const char *ali_ca_cert;

extern const char *ty_ca_cert;

extern const char *ty_http_ca_cert;


extern const char *charles_ca_cert;

/* TODO: ���Ҫ�ر���־, �Ͱ��������ʵ��Ϊ��, ���Ҫ������־, �ɸ���codeѡ�񲻴�ӡ
*
* ����: [1578127395.399][LK-0409] > POST /auth HTTP/1.1
*
* ����������־��code����0409(ʮ������), codeֵ�Ķ����core/aiot_state_api.h
*
*/

/* ��־�ص�����, SDK����־���������� */
int32_t demo_state_logcb1(int32_t code, char *message)
{
	LOG("%s", message);
	return 0;
}

/* HTTP�¼��ص�����, ���ڲ�Token�仯ʱ������, �¼������core/aiot_http_api.h */
void demo_http_event_handler1(void *handle, const aiot_http_event_t *event, void *user_data)
{
	int32_t res;

	/* tokenʧЧ�¼����� */
	if (event->type == AIOT_HTTPEVT_TOKEN_INVALID) {
		LOG("token invalid, invoke iot_http_auth to get new token\n");
		res = aiot_http_auth(handle);
		LOG("aiot_http_auth in callback, res = -0x%04x\r\n", -res);
	}
}

/* HTTP�¼��ص�����, ��SDK��ȡ�����籨��ʱ������, �����������ͼ�core/aiot_http_api.h */
void demo_http_recv_handler1(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
	switch (packet->type) {
	case AIOT_HTTPRECV_STATUS_CODE: {
		/* TODO: ���´����������ע��, SDK�յ�HTTP����ʱ, ��ͨ������û��ص���ӡHTTP״̬��, ��404, 200, 302�� */
		/* LOG("status code: %d\n", packet->data.status_code.code); */
	}
									break;

	case AIOT_HTTPRECV_HEADER: {
		/* TODO: ���´����������ע��, SDK�յ�HTTP����ʱ, ��ͨ������û��ص���ӡHTTP�ײ�, ��Content-Length�� */
		/* LOG("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value); */
	}
							   break;

							   /* TODO: �����Ҫ������ƽ̨��HTTP��Ӧ����, �޸�����, ����ֻ�ǽ���Ӧ��ӡ���� */
	case AIOT_HTTPRECV_BODY: {
		LOG("%.*s\r\n", packet->data.body.len, packet->data.body.buffer);
	}
							 break;

	default: {
	}
			 break;

	}
}

/* ��HTTPͨ���ϱ�ҵ�����ݸ���ƽ̨, ����: ���ѹر� */
int32_t demo_http_post_lightswitch1(void *handle)
{
	/* TODO: ���Ҫ�޸��ϱ�����������, ���޸� data[] ���� */
	char data[] = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
	int32_t res;

	/* ��HTTPͨ������ƽ̨�ϱ�1��ҵ������, �ӿ���aiot_http_send() */
	res = aiot_http_send(handle,
		"/sys/a13FN5TplKq/http_basic_demo/thing/event/property/post",
		(uint8_t *)data,
		strlen(data));
	if (res < 0) {
		LOG("aiot_http_send res = -0x%04X\r\n", -res);
		return res;
	}

	res = aiot_http_recv(handle);
	if (res >= 0) {
		/* �ɹ����յ�������Ӧ��, ��ҵ��Ӧ����Ϊ0, ˵�������ϱ��ɹ� */
		return 0;
	}
	else {
		LOG("aiot_http_recv res = -0x%04X\r\n", -res);
		return -1;
	}
}


char* linksdk_http_post(char *host,char *path,char *json)
{
	void       *http_handle = NULL;
	int32_t     res;
	uint16_t    port = 443;
	aiot_sysdep_network_cred_t cred;



	/* ����SDK�ĵײ����� */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* ����SDK����־��� */
	aiot_state_set_logcb(demo_state_logcb1);

	/* ����SDK�İ�ȫƾ��, ���ڽ���TLS���� */
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
	cred.max_tls_fragment = 4096;
	/*cred.x509_server_cert = ali_ca_cert;
	cred.x509_server_cert_len = strlen(ali_ca_cert);*/
	cred.x509_server_cert = ty_http_ca_cert;
	cred.x509_server_cert_len = strlen(ty_http_ca_cert);

	/* ����1��HTTP�ͻ���ʵ�����ڲ���ʼ��Ĭ�ϲ��� */
	http_handle = aiot_http_init();

	/* ���÷��������� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* ���÷������˿� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* �����豸��ȫƾ֤ */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);

	/*
	* TODO: ���´����������ע��, ����ʾ��TCP(������TLS)������ƽ̨
	*/

	/*
	
	{
	int32_t     tcp_port = 80;
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, &tcp_port);
	}

	*/

	/* �豸��֤, ��ȡtoken */
	char* resp = NULL;
    uint32_t totalLen = 0;
    res  = aiot_http_post(http_handle,path,json,&resp,&totalLen);
	if (res == 0 && resp != NULL) {
		LOG("aiot_http_post succeed\r\n\r\n");
	}
	else {
		/* �����֤ʧ��, ������ʵ��, ������Դ, ���������˳� */
		LOG("aiot_http_post failed, res = -0x%04X\r\n\r\n", -res);
		//aiot_http_deinit(&http_handle);
	}

	/* TODO: ���´����������ע��, ����ʾ�ϱ� 1 �����ݵ�������ƽ̨ */
	/*
	demo_http_post_lightswitch(http_handle);
	LOG("\r\n");
	*/

	/* ����HTTPʵ�� */
	aiot_http_deinit(&http_handle);
	LOG("program exit as normal return\r\n");
	LOG("\r\n");

	return resp;
}


ty_http_handle_t ty_http_setup(ty_http_cfg *cfg)
{
	void       *http_handle = NULL;
	int32_t     res;
	int    port = 443;
	aiot_sysdep_network_cred_t cred;

	/* ����SDK�ĵײ����� */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* ����SDK����־��� */
	aiot_state_set_logcb(demo_state_logcb1);

	/* ����SDK�İ�ȫƾ��, ���ڽ���TLS���� */
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
	cred.max_tls_fragment = 4096;
	/*cred.x509_server_cert = ali_ca_cert;
	cred.x509_server_cert_len = strlen(ali_ca_cert);*/
	cred.x509_server_cert = cfg->cert;
	cred.x509_server_cert_len = strlen(cfg->cert);
    char server_name[128] = {0};
    char host[128] = {0};
    int secure_tls = 0;
    memcpy(server_name,cfg->host,strlen(cfg->host));
   
    res = http_url_paser_host_port (server_name,host, sizeof(host),&port,&secure_tls);
    if(res != 0)
    {
        LOG("%s, http_url_paser_host_port err =%d\r\n", __func__,res);
        return NULL;
    }
    
    LOG("%s, server name =%s port =%d\r\n", __func__,host,port);
	/* ����1��HTTP�ͻ���ʵ�����ڲ���ʼ��Ĭ�ϲ��� */
	http_handle = aiot_http_init();
	if (NULL == http_handle)
	{
		LOG("%s, aiot_http_init ,%d\r\n", __func__,__LINE__);
		return NULL;
	}
	LOG("%s, aiot_http_init success, %d\r\n", __func__,__LINE__);

	/* ���÷��������� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* ���÷������˿� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* �����豸��ȫƾ֤ */
    if(secure_tls)
	    aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
    else
    {
        cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
        aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
    }

	LOG("Leave %s, %d\r\n", __func__,__LINE__);
    return (ty_http_handle_t)http_handle;

}

char *ty_http_post_request(ty_http_handle_t  handle,const char* uri,char* params, int timeout_ms)
{
    char *resp = NULL;
    uint32_t totalLen = 0;
    LOG("Enter %s, %d\r\n", __func__,__LINE__);
    aiot_http_post((void*)handle,uri,params,&resp,&totalLen);
    LOG("Leave %s, %d\r\n", __func__,__LINE__);
    return resp;
}


int ty_http_get_request(ty_http_handle_t  handle,const char* uri,uint8_t **response_content, uint32_t* responseLen, uint32_t* totalLen,
    uint32_t startRange, uint32_t endRange,int timeout_ms)
{
    int res;
	res = aiot_http_get((void*)handle, uri, response_content, responseLen, totalLen,startRange, endRange);
	//core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)*response, *responseLen);
	LOG("responseLen is %d\r\n", *responseLen);
	if (res == 0) {
		LOG("ty_http_get_request succeed\r\n\r\n");
	}
	else {
		/* �����֤ʧ��, ������ʵ��, ������Դ, ���������˳� */
		LOG("aiot_http_auth failed, res = -0x%04X\r\n\r\n", -res);
	}
    return res;
}


void ty_http_close(ty_http_handle_t handle)
{
    aiot_http_deinit((void **)&handle);
}


int http_get(char *host, char *path, uint8_t **response, uint32_t *responseLen, uint32_t startRange, uint32_t endRange)
{
	void       *http_handle = NULL;
	int32_t     res;
	uint16_t    port = 443;
	aiot_sysdep_network_cred_t cred;



	/* ����SDK�ĵײ����� */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* ����SDK����־��� */
	aiot_state_set_logcb(demo_state_logcb1);

	/* ����SDK�İ�ȫƾ��, ���ڽ���TLS���� */
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
	cred.max_tls_fragment = 16384;
	/*cred.x509_server_cert = ali_ca_cert;
	cred.x509_server_cert_len = strlen(ali_ca_cert);*/
	cred.x509_server_cert = ty_ca_cert;
	cred.x509_server_cert_len = strlen(ty_ca_cert);

	/* ����1��HTTP�ͻ���ʵ�����ڲ���ʼ��Ĭ�ϲ��� */
	http_handle = aiot_http_init();

	/* ���÷��������� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* ���÷������˿� */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* �����豸��ȫƾ֤ */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);

	/*
	* TODO: ���´����������ע��, ����ʾ��TCP(������TLS)������ƽ̨
	*/


	/*
	{
	int32_t     tcp_port = 80;
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_NONE;
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, &tcp_port);
	}*/

	/**/


	//res = aiot_http_post(http_handle,path,json,response);
	uint32_t totalLen;
	res = aiot_http_get(http_handle, path, response, responseLen,&totalLen ,startRange, endRange);
	core_log_hexdump(STATE_HTTP_LOG_RECV_CONTENT, '<', (uint8_t *)*response, *responseLen);
	LOG("responseLen is %d\n", *responseLen);
	if (res == 0) {
		LOG("aiot_http_auth succeed\r\n\r\n");
	}
	else {
		/* �����֤ʧ��, ������ʵ��, ������Դ, ���������˳� */
		LOG("aiot_http_auth failed, res = -0x%04X\r\n\r\n", -res);
		aiot_http_deinit(&http_handle);
	}

	/* TODO: ���´����������ע��, ����ʾ�ϱ� 1 �����ݵ�������ƽ̨ */
	/*
	demo_http_post_lightswitch(http_handle);
	LOG("\r\n");
	*/

	/* ����HTTPʵ�� */
	aiot_http_deinit(&http_handle);
	LOG("program exit as normal return\r\n");
	LOG("\r\n");

	return res;
}


