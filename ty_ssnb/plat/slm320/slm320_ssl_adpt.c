#include "ty_os_adpt.h"
#include "ty_ssl_adpt.h"
#include "osi_log.h"
#include "osi_api.h"
#include "mg_sslsock_api.h"

typedef struct
{
    mUpnpSocket *sock;
 //   dtls_establish_info_s shinfo;
}SS11_SSL_CTX;
//ssl²ãÊÊÅä½Ó¿Ú

ty_ssl_handle_t ty_ssl_new()
{
    SS11_SSL_CTX *ctx = lpa_calloc(1,sizeof(SS11_SSL_CTX));
    if(!ctx)
        return ctx;
//     LOG("%s, %d handle_sock: %x",__func__,__LINE__, ctx);
    return ctx;
}

int ty_ssl_cert_load(ty_ssl_handle_t handle,const char* cert, int cert_len)
{
    int CrtType = -1;
    SS11_SSL_CTX *ctx = (SS11_SSL_CTX *)handle;
    if(!ctx)
        return -1;
    ctx->sock= MG_socket_ssl_new();
    if(!ctx->sock)
        {
            LOG("new socket fail");
            return -1;
        }
    CrtType = CG_HTTPS_CA_CRT;
    ty_printf_hexbuf (cert,strlen(cert));
    if (MG_saveCrttoSysFile(cert, cert_len, CrtType) == 0) {
        LOG("MG_saveCrttoSysFile fail");
        return -1;
    }
    LOG("%s, %d\r\n", __func__, __LINE__);
    //MG_set_ssl_seclevel(CG_HTTPS_CERT);
    MG_set_ssl_seclevel(CG_HTTPS_VERIFY_OPTIONAL);
    if(!ctx->sock)
        return -1;
    
    return 0;
}

int ty_ssl_connect(ty_ssl_handle_t handle,char* ipaddr, int port)
{
    int rc;
    char strport[16] = {0};
    //char * ipstr = NULL;
    SS11_SSL_CTX *ctx = (SS11_SSL_CTX *)handle;
    if(!ctx || !ctx->sock)
        return -1;
#if ADPT_DEBUG
        LOG("%s -->ipstr :%s, port: %d", __FUNCTION__, ipaddr, port);
        //ipstr = "59.173.2.70";  //getipstrbyname(ipaddr);
        //LOG("%s -->ipstr :%s, port: %d", __FUNCTION__, ipstr, port);
#endif       
        rc = MG_socket_ssl_connect(ctx->sock, ipaddr, port);
        if (rc != 0) {
            LOG("MG_SSL_socket_connect fail");
            //MG_socket_ssl_close(ctx->sock);
            return rc;
        }else{
            LOG("MG_SSL_socket_connect success");
        }
    return rc;
}

int ty_ssl_send(ty_ssl_handle_t handle, const unsigned char *buf, uint32_t size,uint32_t timeout_ms)
{
    SS11_SSL_CTX *ctx = (SS11_SSL_CTX *)handle;
    if(!ctx || !ctx->sock)
        return -1;
 #if ADPT_DEBUG   
//     LOG("%s: handle_sock: %x %x",__func__,ctx, ctx->sock);
 #endif  
    return MG_socket_ssl_write(ctx->sock, buf, size);
}

int32_t ty_ssl_recv(ty_ssl_handle_t handle, uint8_t *data, uint32_t size, uint32_t timeout_ms)
{
    SS11_SSL_CTX *ctx = (SS11_SSL_CTX *)handle;
    if(!ctx || !ctx->sock)
        return -1;
#if ADPT_DEBUG   
//    LOG("%s: handle_sock: %x %x",__func__,ctx, ctx->sock);
#endif  
    int rc =  MG_socket_ssl_read (ctx->sock, data, size);
    if(rc == MBEDTLS_ERR_SSL_TIMEOUT ) //timeout
        rc = 0;
    return rc;
}

void ty_ssl_close(ty_ssl_handle_t handle)
{
    SS11_SSL_CTX *ctx = (SS11_SSL_CTX *)handle;
#if ADPT_DEBUG   
//     LOG("%s: handle_sock: %x %x",__func__,ctx, ctx->sock);
#endif  
    if(!ctx || !ctx->sock)
        return;
    MG_socket_ssl_close(ctx->sock);
    ctx->sock= NULL;
    lpa_free(ctx);
//    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
}


