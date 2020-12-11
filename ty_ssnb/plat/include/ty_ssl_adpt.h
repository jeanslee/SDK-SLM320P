#ifndef __TY_SSL_ADPT_H__
#define __TY_SSL_ADPT_H__

#include "stdint.h"

typedef void* ty_ssl_handle_t;

//ssl²ãÊÊÅä½Ó¿Ú

ty_ssl_handle_t ty_ssl_new();

int ty_ssl_cert_load(ty_ssl_handle_t handle,const char* cert, int cert_len);

int ty_ssl_connect(ty_ssl_handle_t handle,char* ipaddr, int port);

int ty_ssl_send(ty_ssl_handle_t handle, const unsigned char *buf, uint32_t size,uint32_t timeout_ms);

int32_t ty_ssl_recv(ty_ssl_handle_t handle, uint8_t *data, uint32_t size, uint32_t timeout_ms);

void ty_ssl_close(ty_ssl_handle_t handle);

#endif

