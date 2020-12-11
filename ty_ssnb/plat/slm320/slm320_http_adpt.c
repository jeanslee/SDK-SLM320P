
/*
* 这个例程适用于`Linux`这类支持pthread的POSIX设备, 它演示了用SDK配置HTTP参数并成功鉴权, 然后
*
* + 如果放开被注释掉的 demo_http_post_lightswitch() 函数调用, 会向平台上报1条HTTP消息
*
* 需要用户关注或修改的部分, 已经用 TODO 在注释中标明
*
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>

#include "aiot_http_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"
//#include "ty_ss_cloud.h"
#include "ty_http_adpt.h"
#define CONFIG_HTTPC_SERVER_NAME_SIZE 64
#include "http_parser.h"
#define lk_msleep

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;

extern const char *ty_ca_cert;

extern const char *ty_http_ca_cert;


extern const char *charles_ca_cert;

/* TODO: 如果要关闭日志, 就把这个函数实现为空, 如果要减少日志, 可根据code选择不打印
*
* 例如: [1578127395.399][LK-0409] > POST /auth HTTP/1.1
*
* 上面这条日志的code就是0409(十六进制), code值的定义见core/aiot_state_api.h
*
*/

/* 日志回调函数, SDK的日志会从这里输出 */
int32_t demo_state_logcb1(int32_t code, char *message)
{
	LOG("%s", message);
	return 0;
}

/* HTTP事件回调函数, 当内部Token变化时被触发, 事件定义见core/aiot_http_api.h */
void demo_http_event_handler1(void *handle, const aiot_http_event_t *event, void *user_data)
{
	int32_t res;

	/* token失效事件处理 */
	if (event->type == AIOT_HTTPEVT_TOKEN_INVALID) {
		LOG("token invalid, invoke iot_http_auth to get new token\n");
		res = aiot_http_auth(handle);
		LOG("aiot_http_auth in callback, res = -0x%04x\r\n", -res);
	}
}

/* HTTP事件回调函数, 当SDK读取到网络报文时被触发, 报文描述类型见core/aiot_http_api.h */
void demo_http_recv_handler1(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
	switch (packet->type) {
	case AIOT_HTTPRECV_STATUS_CODE: {
		/* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP状态码, 如404, 200, 302等 */
		/* LOG("status code: %d\n", packet->data.status_code.code); */
	}
									break;

	case AIOT_HTTPRECV_HEADER: {
		/* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP首部, 如Content-Length等 */
		/* LOG("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value); */
	}
							   break;

							   /* TODO: 如果需要处理云平台的HTTP回应报文, 修改这里, 现在只是将回应打印出来 */
	case AIOT_HTTPRECV_BODY: {
		LOG("%.*s\r\n", packet->data.body.len, packet->data.body.buffer);
	}
							 break;

	default: {
	}
			 break;

	}
}

/* 用HTTP通道上报业务数据给云平台, 例如: 灯已关闭 */
int32_t demo_http_post_lightswitch1(void *handle)
{
	/* TODO: 如果要修改上报的数据内容, 就修改 data[] 数组 */
	char data[] = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
	int32_t res;

	/* 用HTTP通道向云平台上报1条业务数据, 接口是aiot_http_send() */
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
		/* 成功接收到服务器应答, 且业务应答码为0, 说明数据上报成功 */
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



	/* 配置SDK的底层依赖 */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* 配置SDK的日志输出 */
	aiot_state_set_logcb(demo_state_logcb1);

	/* 创建SDK的安全凭据, 用于建立TLS连接 */
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
	cred.max_tls_fragment = 4096;
	/*cred.x509_server_cert = ali_ca_cert;
	cred.x509_server_cert_len = strlen(ali_ca_cert);*/
	cred.x509_server_cert = ty_http_ca_cert;
	cred.x509_server_cert_len = strlen(ty_http_ca_cert);

	/* 创建1个HTTP客户端实例并内部初始化默认参数 */
	http_handle = aiot_http_init();

	/* 配置服务器域名 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* 配置服务器端口 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* 配置设备安全凭证 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);

	/*
	* TODO: 以下代码如果不被注释, 会演示用TCP(而不是TLS)连接云平台
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

	/* 设备认证, 获取token */
	char* resp = NULL;
    uint32_t totalLen = 0;
    res  = aiot_http_post(http_handle,path,json,&resp,&totalLen);
	if (res == 0 && resp != NULL) {
		LOG("aiot_http_post succeed\r\n\r\n");
	}
	else {
		/* 如果认证失败, 就销毁实例, 回收资源, 结束程序退出 */
		LOG("aiot_http_post failed, res = -0x%04X\r\n\r\n", -res);
		//aiot_http_deinit(&http_handle);
	}

	/* TODO: 以下代码如果不被注释, 会演示上报 1 次数据到物联网平台 */
	/*
	demo_http_post_lightswitch(http_handle);
	LOG("\r\n");
	*/

	/* 销毁HTTP实例 */
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

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
    /* 配置SDK的日志输出 */
    aiot_state_set_logcb(demo_state_logcb1);

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
    cred.max_tls_fragment = 4096;
    /*cred.x509_server_cert = ali_ca_cert;
    cred.x509_server_cert_len = strlen(ali_ca_cert);*/
    LOG("%s %d", __func__, __LINE__);
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
    
    LOG("%s, server name =%s port =%d, secure_tls = %d\r\n", __func__,host,port, secure_tls);
	/* 创建1个HTTP客户端实例并内部初始化默认参数 */
	http_handle = aiot_http_init();
	if (NULL == http_handle)
	{
		LOG("%s, aiot_http_init ,%d\r\n", __func__,__LINE__);
		return NULL;
	}
	LOG("%s, aiot_http_init success, %d\r\n", __func__,__LINE__);

	/* 配置服务器域名 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* 配置服务器端口 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* 配置设备安全凭证 */
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
		/* 如果认证失败, 就销毁实例, 回收资源, 结束程序退出 */
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



	/* 配置SDK的底层依赖 */
	aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile);
	/* 配置SDK的日志输出 */
	aiot_state_set_logcb(demo_state_logcb1);

	/* 创建SDK的安全凭据, 用于建立TLS连接 */
	memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
	cred.option = AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA;
	cred.max_tls_fragment = 16384;
	/*cred.x509_server_cert = ali_ca_cert;
	cred.x509_server_cert_len = strlen(ali_ca_cert);*/
	cred.x509_server_cert = ty_ca_cert;
	cred.x509_server_cert_len = strlen(ty_ca_cert);

	/* 创建1个HTTP客户端实例并内部初始化默认参数 */
	http_handle = aiot_http_init();

	/* 配置服务器域名 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_HOST, (void *)host);
	/* 配置服务器端口 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_PORT, (void *)&port);
	/* 配置设备安全凭证 */
	aiot_http_setopt(http_handle, AIOT_HTTPOPT_NETWORK_CRED, &cred);

	/*
	* TODO: 以下代码如果不被注释, 会演示用TCP(而不是TLS)连接云平台
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
		/* 如果认证失败, 就销毁实例, 回收资源, 结束程序退出 */
		LOG("aiot_http_auth failed, res = -0x%04X\r\n\r\n", -res);
		aiot_http_deinit(&http_handle);
	}

	/* TODO: 以下代码如果不被注释, 会演示上报 1 次数据到物联网平台 */
	/*
	demo_http_post_lightswitch(http_handle);
	LOG("\r\n");
	*/

	/* 销毁HTTP实例 */
	aiot_http_deinit(&http_handle);
	LOG("program exit as normal return\r\n");
	LOG("\r\n");

	return res;
}




