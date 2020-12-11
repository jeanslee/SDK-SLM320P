
#include "ty_socket_adpt.h"
#include "ty_os_adpt.h"
//#include "softap_api.h"
//#include "http.h"
//#include "mbedtls/net_sockets.h"
//#include "mbedtls/debug.h"
//#include "mbedtls/x509.h"
//#include "mbedtls/x509_crt.h"

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "mg_os_api.h"
#include "osi_log.h"
#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include "sockets.h"
#include "lwip/netdb.h"
char ipstr[32] = {0};

char* getipstrbyname(const char *name)
{
	char **pptr = NULL;
	struct hostent *hptr = NULL;
       char *IP = NULL;
       if(name == NULL)
            return NULL;   
       
	if ((hptr = gethostbyname(name)) == NULL)
	{   
            LOG("gethostbyname error for host:%s\n", name);   
            return NULL;
	}
#if ADPT_DEBUG
	LOG("official hostname:%s\n",hptr->h_name);	
#endif
	switch(hptr->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
                        pptr=hptr->h_addr_list;
                        IP = inet_ntop(hptr->h_addrtype, hptr->h_addr, ipstr, sizeof(ipstr));
                        break;
		default:
			break;
    }
    return IP;
}
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
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            LOG("alloc socket fd fail\n");
            return -1;
    }

    host_entry = httpc_wrapper_gethostbyname(ipaddr);
    if (host_entry == NULL) {
            LOG("%s, get host name fail", __func__);
            close(socket_fd);
            return -1;
    }
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
    addr.sin_port =  htons(port);
#if ADPT_DEBUG
    LOG("%s,%d",__FUNCTION__, __LINE__);
#endif 
    rc =  connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
#if ADPT_DEBUG
    LOG("connect %d",rc);
#endif 
    if(rc != 0)
    {
        close(socket_fd);
        return -2;
    }
#if ADPT_DEBUG
   LOG("%s,%d",__FUNCTION__, __LINE__);
#endif 
	return socket_fd;
	
}


int ty_socket_send(int32_t socket, const unsigned char *buf, uint32_t size,uint32_t timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = timeout_ms/1000;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	int	rc =  write(socket, buf, size);
//       LOG("ty_socket_send %d  rc=%d", size, rc);
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

    ts_end = MG_osiEpochTime() + timeout_ms;
    recv_len = 0;

    /* set rx timeout to elliminate the impacts when Wi-FI AP is disconnected */
    recv_to.tv_sec = (timeout_ms / 1000);
    recv_to.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_to, sizeof(recv_to));
    
    do {
        ts_left = 0;
        if (ts_end > MG_osiEpochTime()) {
            ts_left = ts_end - (uint64_t)MG_osiEpochTime();
        }

        if (ts_left == 0 && timeout_ms != 0) {
            ret = TCP_MBEDTLS_ERR_SSL_TIMEOUT;
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
                    LOG("%s, fd %d closed", __func__, socket);
                    ret = TCP_MBEDTLS_ERR_SSL_CONN_EOF;
                } else {
                    if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
                        (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
                        continue;
                    }
                    ret = TCP_MBEDTLS_ERR_NET_RECV_FAILED;
               }
            }
        } else if (ret == 0) {
            ret = TCP_MBEDTLS_ERR_SSL_WANT_READ ;
        } else {
            if (EINTR == errno || ret == TCP_MBEDTLS_ERR_SSL_WANT_READ)
            {
                continue;
            }
            ret = TCP_MBEDTLS_ERR_NET_RECV_FAILED;
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
