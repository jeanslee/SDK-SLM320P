
#include "ty_socket_adpt.h"
#include "softap_api.h"
#include "http.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"


struct hostent *httpc_wrapper_gethostbyname(const char *name)
{
    return gethostbyname(name);
}

int ty_socket_connect(char* ipaddr, int port)
{
	int type = SOCK_STREAM;
	int rc = -1;
    struct sockaddr_in addr;
    struct hostent *host_entry;
    int socket_fd;
    xy_printf("%s,%d",__FUNCTION__, __LINE__);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            xy_printf("alloc socket fd fail\n");
            return -1;
    }
    xy_printf("%s,%d",__FUNCTION__, __LINE__);

    host_entry = httpc_wrapper_gethostbyname(ipaddr);
    if (host_entry == NULL) {
            xy_printf("%s, get host name fail", __func__);
            close(socket_fd);
            return -1;
    }
    xy_printf("%s,%d",__FUNCTION__, __LINE__);

    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
    addr.sin_port = htons(port);
    xy_printf("%s,%d",__FUNCTION__, __LINE__);

    rc = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    xy_printf("connect %d",rc);
    if(rc != 0)
    {
        close(socket_fd);
        return -2;
    }
    xy_printf("%s,%d",__FUNCTION__, __LINE__);
	return socket_fd;
}


int ty_socket_send(int32_t socket, const unsigned char *buf, uint32_t size,uint32_t timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = timeout_ms/1000;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	int	rc = write(socket, buf, size);
    //xy_printf( "linux_write %d  rc=%d",len,rc);
    return rc;
}

int32_t ty_socket_recv(int32_t socket, uint8_t *data, uint32_t size, uint32_t timeout_ms)
{

    struct timeval tv;
    fd_set sets;
    int32_t ret;
    int32_t recv_len;
    uint64_t ts_end;
    uint64_t ts_left;
    struct timeval recv_to;
    int32_t parsz =0;

    ts_end = xy_GetTickCount() + timeout_ms;
    recv_len = 0;

    /* set rx timeout to elliminate the impacts when Wi-FI AP is disconnected */
    recv_to.tv_sec = (timeout_ms / 1000);
    recv_to.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_to, sizeof(recv_to));
    
    do {
        ts_left = 0;
        if (ts_end > xy_GetTickCount()) {
            ts_left = ts_end - (uint64_t)xy_GetTickCount();
        }

        if (ts_left == 0 && timeout_ms != 0) {
            ret = HTTP_ETIMEOUT;
            break;
        }

        FD_ZERO(&sets);
        FD_SET(socket, &sets);
        tv.tv_sec = ts_left / 1000;
        tv.tv_usec = (ts_left % 1000) * 1000;
        ret = select(socket + 1, &sets, NULL, NULL, (ts_left == 0)? NULL: &tv);

        if (ret > 0) {
            if (FD_ISSET(socket, &sets)) {
                    ret = recv(socket, data + recv_len, size - recv_len, 0);
                if (ret > 0) {
                    recv_len += ret;
                } else if (ret == 0) {
                    xy_printf("%s, fd %d closed", __func__, socket);
                    ret = HTTP_ECLSD;
                } else {
                    if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
                        (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
                        continue;
                    }
                    ret = HTTP_ERECV;
               }
            }
        } else if (ret == 0) {
            ret = MBEDTLS_ERR_SSL_WANT_READ ;
        } else {
            if (EINTR == errno || ret == MBEDTLS_ERR_SSL_WANT_READ)
            {
                continue;
            }
            ret = HTTP_ERECV;
        }

        if (ret < 0) {
            break;
        }
    } while (recv_len < size);
    
    return (recv_len > 0? recv_len: ret);
}

void ty_socket_close(int32_t socket)
{
    close(socket);
}

int mbedtls_tx(void *context, const unsigned char *buf, uint32_t size)
{
    int *socket = (int*)context;
    return ty_socket_send (*socket,  buf,  size, 30*1000);
}

int mbedtls_rx(void *context, unsigned char *buf, uint32_t size)
{
    int *socket = (int*)context;
    int ret = 0;
    ret = recv(*socket, buf, size, 0);
    if (ret < 0) {
        ret = MBEDTLS_ERR_NET_RECV_FAILED;
    }
    return ret;
}

int mbedtls_connect(char* ipaddr, int port)
{
    return ty_socket_connect (ipaddr,port);
}

int mbedtls_close(void* context)
{
    if(!context)
        return -1;
    int *socket = (int*)context;
    ty_socket_close(*socket);
    return 0;
}

int32_t mbedtls_rx_timeout(void *context, uint8_t *data, uint32_t size, uint32_t timeout)
{
    int *socket = (int*)context;
    return ty_socket_recv (*socket ,  data,  size,  timeout);
}


