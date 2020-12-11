#include "ty_devinfo.h"
#include "ty_mqtt_basic_info.h"
#include "ty_ss_cloud.h"
#include "md5.h"
#include "sha256.h"
#include "ty_os_adpt.h"
#include "ty_http_adpt.h"
#include "http_client.h"


extern const char *ty_ca_cert; //mqtt
extern const char *ty_http_ca_cert;//http


int ty_LoadHttpCACert(char**cert,int *cert_len)
{
    *cert = ty_http_ca_cert;
    *cert_len  = strlen(ty_http_ca_cert);
    return 0;

}

int ty_LoadMqttCACert(char**cert,int *cert_len)
{
    *cert = ty_ca_cert;
    *cert_len  = strlen(ty_ca_cert);
    return 0;
}

int ty_LoadMqttCntInfo(ty_mqtt_cnt_info *info)
{
    int ret;
    //char clientId [150] = {0};
    //char username [64] =  {0};
    //char password [65] =  {0};
    char deviceSecret [ 64 ] = {0};
    char IMEI_ID [ 20 ] = {0};
    
    char *cert = NULL;
    int cert_len = 0;
    ty_LoadMqttCACert(&cert,&cert_len);
    at_imei (IMEI_ID);
    
    ret = ty_LoadDeviceSecret (deviceSecret);
    if(ret != 0)
        return ret;
    LOG("MQTT_PRODUCT_KEY=%s\r\n",MQTT_PRODUCT_KEY);
    LOG("IMEI_ID=%s\r\n",IMEI_ID);
    //LOG("deviceSecret=%s\r\n",deviceSecret);
    ty_CalcMqttSignInfo (MQTT_PRODUCT_KEY,IMEI_ID,deviceSecret,info->ClientID,info->user,info->passwd);

    info->url = TY_CLOUD_MQTT_SERVER;
    info->port = TY_CLOUD_PORT;
    info->CleanSession = TY_CLOUD_MQTT_CLEANSESSION;
    info->usetls = TY_CLOUD_MQTT_TLS;
    info->keepalive = TY_CLOUD_MQTT_KEEPALIVE*3;
    info->tls_ca.ca_cert = cert;
    info->tls_ca.cert_len = cert_len;
    //info->ClientID = info->ClientID;
    //info->user = info->user;
    //info->passwd = info->passwd;

    #ifdef TY_MQTT_NO_TLS
    info->usetls = 0;
    info->port = 3709;
    #endif
    
    return 0;
}

char *http_post_util(const char *host,const char* uri,char* params,const char *cert,int content_type,int *code)
{
    int i;
    char *resp = NULL;
    ty_http_cfg config = {0};
    config.host = host;
    config.cert = cert;
    config.keepalvie = false;
    ty_http_handle_t handle = ty_http_setup(&config);
    if (!handle)
    {
        return NULL;
    }
    for(i = 0;i< 2;i++)
    {
        resp = ty_http_post_request (handle, uri, params,10000);
        if(code)
        {
            if(resp)
            {
                *code = 0;
                break;
            }
            else
                *code = -1;
        }
    }
    ty_http_close(handle);
    return resp;
    
}

char *http_post(const char *host,const char* uri,char* params,const char *cert)
{
    int code = -1;
    return http_post_util (host, uri,  params,cert, HTTP_Content_Type_JSON,&code);
}

char *http_post_dynreg(const char *cert,int cert_len,const char* host, const char* uri, char* param)
{
    int code = -1;
    return http_post_util (host, uri,  param,cert, HTTP_Content_Type_X_Form,&code);
}

#ifdef ALIOS_HTTP
//application/x-www-form-urlencoded
//application/json
extern uint8_t rsp_buf[RSP_BUF_SIZE];
extern uint8_t req_buf[REQ_BUF_SIZE];

char *http_post_util(const char *host,const char* uri,char* params,const char *cert,int content_type,int *code)
{
    int fd;
    char hdr[HTTP_UP_HDR_SIZE] = {0};
    int32_t ret;
    http_rsp_info_t rsp_info;

    httpc_handle_t httpc_handle = 0;
    httpc_connection_t settings;

    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_client_initialize();
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    /* create socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG("alloc socket fd fail\n");
        goto exit;
    }
    memset(&settings, 0, sizeof(settings));
    settings.socket       = fd;
    settings.server_name  = host;
    settings.req_buf      = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    settings.ca_cert = cert;
    char *accpt_type = NULL;
    char *request_type = NULL;
    /* httpc initialization */
    if ((httpc_handle = httpc_init(&settings)) == 0) {
        LOG("http session init fail\r\n");
        close(fd);
        goto exit;
    }
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    /* construct httc header: set accept content type */
    if(content_type == HTTP_Content_Type_JSON)
    {
        accpt_type = "application/json";
        request_type = "application/json";
    }
    else if(content_type == HTTP_Content_Type_X_Form)
    {
        accpt_type = "text/xml,text/javascript,text/html,application/json";
        request_type = "application/x-www-form-urlencoded";
    }
    
    if ((httpc_construct_header(
            hdr, HTTP_UP_HDR_SIZE, "Accept",
            accpt_type)) < 0) { //text/xml,text/javascript,text/html,
        LOG("http construct header fail\n");
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    /* send get request */
    if ((httpc_send_request(httpc_handle, HTTP_POST, uri, hdr,
                            request_type, params, strlen(params))) != HTTP_SUCCESS) {
        LOG("httpc_send_request fail\r\n");
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    /* get response */
    if ((httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info,
                             5000)) < 0) {
        LOG("httpc_recv_response fail\r\n");
        if(code)
            *code = -1;
        goto exit;
    }

    //LOG("recv rsp_buf %s:\n",rsp_buf);

    //LOG("http session %x, buf size %d bytes, recv %d bytes data", httpc_handle,
    //    RSP_BUF_SIZE, rsp_info.rsp_len);
    // if (rsp_info.rsp_len > 0) {
    //     LOG("%s", rsp_buf);
    // }
    
    if (rsp_info.message_complete) {
        // LOG("message_complete");
        if(code)
            *code = 0;
        LOG("%s %d post success\r\n",__FUNCTION__,__LINE__);
        close(settings.socket);
        httpc_deinit(httpc_handle);
        if(rsp_info.body_start)
        {
           int bdlen = rsp_info.rsp_len - (rsp_info.body_start -  rsp_buf);
           LOG("bdlen=%d\n",bdlen);
           char *data =  lpa_malloc(bdlen+1);
           if(data){
                memset(data,0,bdlen+1);
                memcpy(data,rsp_info.body_start,bdlen);
                LOG("resp=%s\n",data);
                ty_printf_hexbuf (data, bdlen);
                return data;
            }
        }
        else
        {
            LOG("resp no body start\r\n");
        }
        return NULL;
    }
 LOG("%s %d\r\n",__FUNCTION__,__LINE__);
exit:
    close(settings.socket);
    httpc_deinit(httpc_handle);
    return NULL;
}


httpc_handle_t ty_http_setup(ty_http_cfg *cfg)
{
    int fd;
    char hdr[HTTP_UP_HDR_SIZE] = {0};
    int32_t ret;
    http_rsp_info_t rsp_info;

    httpc_handle_t httpc_handle = 0;
    httpc_connection_t settings;

    http_client_initialize();
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            LOG("alloc socket fd fail\n");
            goto exit;
    }
    memset(&settings, 0, sizeof(settings));
    settings.socket       = fd;
    settings.server_name  = cfg->host;
    settings.req_buf      = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    settings.ca_cert = cfg->cert;
    settings.keep_alive = cfg->keepalvie;
    /* httpc initialization */
    if ((httpc_handle = httpc_init(&settings)) == 0) {
        LOG("http session init fail\n");
        close(fd);
        goto exit;
    }
    return httpc_handle;

exit:
    return NULL;
}

char *ty_http_post_request(httpc_handle_t httpc_handle,const char* uri,char* params,int timeout_ms)
{
    int fd;
    char hdr[HTTP_UP_HDR_SIZE] = {0};
    int32_t ret;
    http_rsp_info_t rsp_info;
    int content_type = HTTP_Content_Type_JSON;
    char *accpt_type = NULL;
    char *request_type = NULL;
  
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    /* construct httc header: set accept content type */
    if(content_type == HTTP_Content_Type_JSON)
    {
        accpt_type = "application/json";
        request_type = "application/json";
    }
    else if(content_type == HTTP_Content_Type_X_Form)
    {
        accpt_type = "text/xml,text/javascript,text/html,application/json";
        request_type = "application/x-www-form-urlencoded";
    }
    
    if ((httpc_construct_header(
            hdr, HTTP_UP_HDR_SIZE, "Accept",
            accpt_type)) < 0) { //text/xml,text/javascript,text/html,
        LOG("http construct header fail\n");
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    /* send get request */
    if ((httpc_send_request(httpc_handle, HTTP_POST, uri, hdr,
                            request_type, params, strlen(params))) != HTTP_SUCCESS) {
        LOG("httpc_send_request fail\n");
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    /* get response */
    if ((httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info,
                             timeout_ms)) < 0) {
        LOG("httpc_recv_response fail\n");
        goto exit;
    }

    if (rsp_info.message_complete) {
        // LOG("message_complete");
        LOG("%s %d post success",__FUNCTION__,__LINE__);
        if(rsp_info.body_start)
        {
           int bdlen = rsp_info.rsp_len - (rsp_info.body_start -  rsp_buf);
           LOG("bdlen=%d\n",bdlen);
           char *data =  lpa_malloc(bdlen+1);
           if(data){
                memset(data,0,bdlen+1);
                memcpy(data,rsp_info.body_start,bdlen);
                LOG("resp=%s\n",data);
                ty_printf_hexbuf (data, bdlen);
                return data;
            }
        }
        else
        {
            LOG("resp no body start\n");
        }
        return NULL;
    }
 LOG("%s %d\n",__FUNCTION__,__LINE__);
exit:
    return NULL;
}
void ty_http_close(httpc_handle_t handle)
{
    httpc_t *http_session = (httpc_t *)handle;
    close(http_session->socket);
    httpc_deinit(handle);
}

extern int FOTA_PACKET_SZ;

/* send http get request */
int http_get_tsm_cap(const char *host,const char* fotaURI,int put,int sigalg,const char *sig,const char* params)
{
    int fd;
    static char hdr[HTTP_UP_HDR_SIZE] = {0};
    static char rang[128] = {0};
    unsigned char hash[32] = {0};
    unsigned char hashDld[32] = {0};
    unsigned char hashStr[65] = {0};
    int32_t ret;
    http_rsp_info_t rsp_info;
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    http_client_initialize();
    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    MD5_CTX md5;
    iot_sha256_context context;
     
    MD5Init(&md5);  

    httpc_handle_t httpc_handle = 0;
    httpc_connection_t settings;

    
    /* perform inner SHA */
    utils_sha256_init(&context);                                      /* init context for 1st pass */
    utils_sha256_starts(&context);    

    int isigalg = sigalg;
    
    UINT32 TickCount =  ty_get_tick_ms();
    /* create socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG("alloc socket fd fail\r\n");
        goto exit;
    }
    
    memset(&settings, 0, sizeof(settings));
    settings.socket       = fd;
    settings.server_name  = host;
    settings.req_buf      = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    settings.keep_alive = 1;
    settings.ca_cert = ty_http_ca_cert;

    
    /* httpc initialization */
    if ((httpc_handle = httpc_init(&settings)) == 0) {
        LOG("http session init fail\r\n");
        close(fd);
        goto exit;
    }
    LOG("%s %d\n",__FUNCTION__,__LINE__);
    int fw_offset = 0;
    int fwTotalSz = 0;
    int fwsz;

    while(1)
    {
        retry:
        memset(rang,0,sizeof(rang));
        snprintf(rang,sizeof(rang),"bytes=%d-%d",fw_offset,fw_offset+FOTA_PACKET_SZ-1);
        memset(hdr,0,sizeof(hdr));
        if ((httpc_construct_header(hdr, HTTP_UP_HDR_SIZE, "Range",rang )) < 0) {
            LOG("http construct header fail\r\n");
            goto exit;
        }
            int start = 0,end = 0;
            //Range: bytes=60322-60833
            ty_cloud_ext_notify ("\r\n+TSM CAP DOWNLOADING\r\n");
            /* send get request */
            if ((httpc_send_request(httpc_handle, HTTP_POST, fotaURI, hdr,
                                    "application/json", params, 0)) != HTTP_SUCCESS) {
                LOG("httpc_send_request fail\r\n");
                goto exit;
            }
            /* get response */
            memset(&rsp_info,0,sizeof(rsp_info));
            memset(rsp_buf,0,sizeof(rsp_buf));
            if ((httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info,
                                     20000)) < 0) {
                LOG("httpc_recv_response fail\r\n");
                goto exit;
            }

            char *prefix = strstr(rsp_buf,"Content-Range: bytes ");
            if(prefix != NULL)
            {
               sscanf(prefix+strlen("Content-Range: bytes "),"%d-%d/%d\r\n",&start,&end,&fwTotalSz);
               LOG("%d %d fwTotalSz=%d\n",start,end,fwTotalSz);
               if( start != fw_offset)
               {
                    LOG("download err,start=%d fw_offset=%d \r\n",start,fw_offset);
                    //goto retry;
                    break;
               }
               fwsz = end-start+1;
               fw_offset += fwsz;
               if(1 == isigalg)
                    MD5Update(&md5,(const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz);
               else utils_sha256_update(&context, (const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz);

               if((fwsz < FOTA_PACKET_SZ || fw_offset == fwTotalSz))
               {    
                    if(1 == isigalg)
                        MD5Final(&md5,hash);
                    else
                        utils_sha256_finish(&context,hash);
                    int destlen = asc2hex (hashDld, sig, strlen(sig));

                    bytes2hexstr (hash, sizeof(hash),hashStr,sizeof(hashStr));
                    LOG("hash=%s",hashStr);
                    LOG("hashDld=%s",sig);
                    if(0 != memcmp(hash,hashDld,destlen))
                    {
                        goto exit;
                    }
                    LOG("ota file check hash success\r\n");
               }
               if(put)
                    put_fota_packet ((const uint8_t *)rsp_buf+(rsp_info.rsp_len-fwsz),fwsz,(fwsz < FOTA_PACKET_SZ || fw_offset == fwTotalSz));
            }
            else
            {
                LOG("httpc_send_request fail ,exit put last pkg\r\n");
                goto exit;
            }
            if(fwsz < 512 || fw_offset == fwTotalSz)
                    break;
    }

    LOG("ota dowload %d bytes,time=%dms",fwTotalSz,ty_get_tick_ms()-TickCount);
    
    if(fwTotalSz == fw_offset)
    {
        LOG("%s %d fota dowload success\r\n",__FUNCTION__,__LINE__);
    }
    else
        LOG("%s %d fota dowload fail\r\n",__FUNCTION__,__LINE__);
    
    close(settings.socket);
    httpc_deinit(httpc_handle);
    return 0;

 LOG("%s %d\n",__FUNCTION__,__LINE__);
exit:
    if(put)
        put_fota_packet ((const uint8_t *)rsp_buf,1,true);//D∩℅?o車辰?～邦那y?Y車?車迆赤?3?D∩FLASH豕???
    LOG("%s %d fota dowload error=%d\n",__FUNCTION__,__LINE__);
    close(settings.socket);
    httpc_deinit(httpc_handle);
    return -1;
}


#endif
int http_get_tsm_cap(const char *host,const char* fotaURI,int put,int sigalg,const char *sig,const char* params)
{
    return -1;
}


int httpc_parser_url(char *url,char *host,int hlen ,char *uri,int ulen)
{
    uint8_t index;
    httpc_param_t param;
    bool ret;
    uint16_t server_name_len = 0;
    uint16_t server_name_offset = 0;
    char *port_ptr;
    bool https = false;

    server_name_len = strlen(url);
    ret = http_str_search(url, ":", 0, strlen(url), &param);
    if (ret == false) {
        LOG("%s, no : in server name\r\n", __func__);
        goto exit;
    }

    server_name_offset += param.len;

    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
            https = true;
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == false) {
        LOG("%s, no http or https in server name\r\n", __func__);
        goto exit;
    }

    if (http_str_search(url, "://", server_name_offset, 3, NULL) == false) {
        LOG("%s, server name format error\r\n", __func__);
        goto exit;
    }
    server_name_offset += 3;

    /* get host name */
    if (http_str_search(url, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        strncpy(host, url, strlen(url));
		uri[0] = '/';
		uri[1] = 0;
		return 0;
    }
    server_name_offset += param.len;
    if(hlen<server_name_offset+1)
        return -1;
    strncpy(host, url, server_name_offset);
    host[server_name_offset] = 0;
	//printf("h=%s\n",host);

    if(ulen < strlen(url+server_name_offset)+1)
        return -2;
	memcpy(uri,url+server_name_offset,strlen(url+server_name_offset));
    return 0;
exit:
    return -3;
}

int http_url_paser_host_port(char *server_name,char *host ,int hlen, int *port,int *secure_tls)
{
    uint8_t index;
    httpc_param_t param;
    bool ret;
    uint16_t server_name_len = 0;
    uint16_t server_name_offset = 0;
    char *port_ptr;
    
    if (server_name == NULL ||
        strlen(server_name) > CONFIG_HTTPC_SERVER_NAME_SIZE) {
        LOG("%s, server name is NULL or too long (max %d)", __func__, CONFIG_HTTPC_SERVER_NAME_SIZE);
        goto exit;
    }

    if (host == NULL || port == NULL || secure_tls == NULL)
    {
        goto exit;
    }

    server_name_len = strlen(server_name);
    ret = http_str_search(server_name, ":", 0, strlen(server_name), &param);
    if (ret == false) {
        LOG("%s, no : in server name", __func__);
        goto exit;
    }

    server_name_offset += param.len;
    *port = 80;
    *secure_tls = 0;
    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
        *secure_tls = 1;
        *port = 443;
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == false) {
        LOG("%s, no http or https in server name", __func__);
        goto exit;
    }

    if (http_str_search(server_name, "://", server_name_offset, 3, NULL) == false) {
        LOG("%s, server name format error", __func__);
        goto exit;
    }
    server_name_offset += 3;


    /* get host name */
    if (http_str_search(server_name, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        if ((server_name_len + 1) >= 64) {
            LOG("%s, server name no buffer space for padding /", __func__);
            goto exit;
        }
        server_name[server_name_len] = '/';
        if (http_str_search(server_name, "/",
                            server_name_offset, server_name_len + 1 - server_name_offset, &param) == false) {
            LOG("%s, server name format error", __func__);
            goto exit;
        }
    }


    memset(host, 0, hlen);
    strncpy(host, param.param, param.len + 1);

    if (http_str_search(host, ":", 0, param.len, &param) == true) {

        if ((strlen(host) - param.len) <= 7) {
            param.param += (param.len + 1);
            port_ptr = param.param;
            while (*port_ptr && port_ptr++) {
                if (*port_ptr == '/') {
                    *port_ptr = 0;
                    *port = (uint16_t)atol(param.param);
                    break;
                }
            }
        } else {
            LOG("%s, server name port too large", __func__);
            goto exit;
        }
        memset(host + param.len, 0, strlen(host) - param.len);
    } else {
        memset(host, 0, hlen);
        strncpy(host, param.param, param.len);
    }
    return 0;
exit:
    return -1;
}


