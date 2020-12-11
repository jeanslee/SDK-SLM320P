#ifndef __TY_SOCK_TEST_H__
#define __TY_SOCK_TEST_H__
#include "rtos_api.h"
#include "ty_socket_adpt.h"

void ty_adpt_http_test(void* param);
void ty_adpt_sslsock_test(void* param);
void ty_adpt_mqtt_test(void* param);
void ty_adpt_tcpsock_test(void);
void ty_sc_channel_test();
#endif