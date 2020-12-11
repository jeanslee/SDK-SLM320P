#ifndef __TY_SOCKET_ADPT_H__
#define __TY_SOCKET_ADPT_H__

#include "stdint.h"

//tcp适配层错误返回码
//为兼容和适配mbedtls协议库，错误返回码需要按以下定义返回
#define TCP_MBEDTLS_ERR_SSL_WANT_READ                         -0x6900
#define TCP_MBEDTLS_ERR_NET_RECV_FAILED                       -0x004C  /**< Reading information from the socket failed. */
#define TCP_MBEDTLS_ERR_SSL_TIMEOUT                           -0x6800  /**< The operation timed out. */
#define TCP_MBEDTLS_ERR_SSL_CONN_EOF                          -0x7280  /**< The connection indicated an EOF. */

//tcp socket层适配接口
int ty_socket_connect(char* ipaddr, int port);

int ty_socket_send(int32_t socket, const unsigned char *buf, uint32_t size,uint32_t timeout_ms);

int32_t ty_socket_recv(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout_ms);

void ty_socket_close(int32_t socket);

#endif

