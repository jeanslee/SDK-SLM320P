
#include "ty_socket_adpt.h"
#include "sockets.h"
#include <netdb.h>

/* http error code */
enum {
    HTTP_SUCCESS  =  0,
    HTTP_ENOBUFS  = -1,  /* buffer error     */
    HTTP_EARG     = -2,  /* illegal argument */
    HTTP_ENOTSUPP = -3,  /* not support      */
    HTTP_EDNS     = -4,  /* DNS fail         */
    HTTP_ECONN    = -5,  /* connect fail     */
    HTTP_ESEND    = -6,  /* send packet fail */
    HTTP_ECLSD    = -7,  /* connect closed   */
    HTTP_ERECV    = -8,  /* recv packet fail */
    HTTP_ETIMEOUT = -9,  /* timeout          */
};


#define MBEDTLS_ERR_SSL_WANT_READ                         -0x6900
#define MBEDTLS_ERR_NET_RECV_FAILED                       -0x004C  /**< Reading information from the socket failed. */


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
    LOG("%s,%d,ipaddr=%s,port=%d\r\n",__FUNCTION__, __LINE__,ipaddr,port);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            LOG("alloc socket fd fail\r\n");
            return -1;
    }
    LOG("%s,%d\r\n",__FUNCTION__, __LINE__);

    host_entry = httpc_wrapper_gethostbyname(ipaddr);
    if (host_entry == NULL) {
            LOG("%s, get host name fail\r\n", __func__);
            close(socket_fd);
            return -1;
    }
    LOG("%s,%d\r\n",__FUNCTION__, __LINE__);

    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
    addr.sin_port = htons(port);
    LOG("%s,%d\r\n",__FUNCTION__, __LINE__);

    rc = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    LOG("connect %d\r\n",rc);
    if(rc != 0)
    {
        close(socket_fd);
        return -2;
    }
    LOG("%s,%d\r\n",__FUNCTION__, __LINE__);
	return socket_fd;
}


int ty_socket_send(int32_t socket, const unsigned char *buf, uint32_t size,uint32_t timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = timeout_ms/1000;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors
    //LOG( "%s socket=%d size=%x\r\n",__func__,socket,size);
	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	int	rc = send(socket, buf, size,0);
    //LOG( "%s socket=%d size=%x rc=%x\r\n",__func__,socket,size,rc);
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

    ts_end = ty_get_tick_ms() + timeout_ms;
    recv_len = 0;

    /* set rx timeout to elliminate the impacts when Wi-FI AP is disconnected */
    recv_to.tv_sec = (timeout_ms / 1000);
    recv_to.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_to, sizeof(recv_to));
    
    do {
        ts_left = 0;
        if (ts_end > ty_get_tick_ms()) {
            ts_left = ts_end - (uint64_t)ty_get_tick_ms();
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
                    LOG("%s, recv=%d\r\n", __func__, ret);
                    recv_len += ret;
                } else if (ret == 0) {
                    LOG("%s, fd %d closed\r\n", __func__, socket);
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

//int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )

int mbedtls_tx(void *context, const unsigned char *buf, size_t size)
{
    int *socket = (int*)context;
    //LOG("%s, *socket=%d,buf=%p,size=%x\r\n", __func__, *socket,buf,size);
    return ty_socket_send (*socket,  buf,  size, 30*1000);
}

int mbedtls_rx(void *context, unsigned char *buf, uint32_t size)
{
    int *socket = (int*)context;
    int ret = 0;
    //LOG("%s, size=%d\r\n", __func__, size);
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


