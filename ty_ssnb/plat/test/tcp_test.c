#include "ty_sc_adpt.h"
#include "ty_socket_adpt.h"
#include "ty_sock_test.h"

#define ADDR "104.224.176.31"
#define PORT 9101
extern void ty_adpt_tcpsock_test(void);
void ty_adpt_tcpsock_test(void)
{

    int ret = -1;
    char *buf = "TCP SEND AND RECV TEST";
    char rec_buf[30]={0};
    int sock = ty_socket_connect(ADDR, PORT);
    while(1){
        ret = ty_socket_send(sock, buf, strlen(buf), 3000);
			LOG("ty_socket_send size = %d,send_buf= %s", ret, buf);

        ret = -1;
        ret = ty_socket_recv(sock, rec_buf, sizeof(rec_buf), 3000);
        LOG("ty_socket_recv size = %d, rec_buf= %s\r\n", ret, rec_buf);
        ty_sleep(1000);
    }
    
}