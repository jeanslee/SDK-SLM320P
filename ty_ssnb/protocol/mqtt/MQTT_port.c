/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - return codes from linux_read
 *******************************************************************************/

#include "MQTT_port.h"
#include "ty_ssl_adpt.h" //用于适配TLS+MQTT
#include "ty_socket_adpt.h"//用于适配TCP
#define XY_MBEDTLS
void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
	timer->xTicksToWait = timeout_ms ;/// portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	timer->xTimeOut = ty_get_tick_ms ();
//      LOG( "%s:%d command_timeout_ms=%d xTimeOut = %d",__FUNCTION__,__LINE__,timer->xTicksToWait, timer->xTimeOut);
	//vTaskSetTimeOutState(&timer->xTimeOut); /* Record the time at which this function was entered. */
}


void TimerCountdown(Timer* timer, unsigned int timeout) 
{
    //nwy_ext_echo("\r\n TimerCountdown timeout=%d\r\n",timeout);
	TimerCountdownMS(timer, timeout * 1000);
}


int TimerLeftMS(Timer* timer) 
{
    if(ty_get_tick_ms () - timer->xTimeOut > timer->xTicksToWait)
        return 0;
	//xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait); /* updates xTicksToWait to the number left */
	 //(timer->xTicksToWait +  - );

    long time = ty_get_tick_ms () - timer->xTimeOut; //消耗的ms
    time = (timer->xTicksToWait - time );
    if(time <0)
        return 0;
    else return time;
}

char TimerIsExpired(Timer* timer)
{
//    LOG("%s:%d TimerIsExpired cosume=%d, xticksto wait = %d\r\n",__func__, __LINE__, ty_get_tick_ms() - timer->xTimeOut, timer->xTicksToWait);
    if(ty_get_tick_ms () - timer->xTimeOut > timer->xTicksToWait)
        return 1;
    else return 0;
	//return xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait) == pdTRUE;
}


void TimerInit(Timer* timer)
{
	timer->xTicksToWait = 0;
	memset(&timer->xTimeOut, '\0', sizeof(timer->xTimeOut));
}

#if defined(XY_MBEDTLS) || defined(TY_MBEDTLS_CA)
int tls_linux_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
    /*
    if(!n->shakehand)
    {
        LOG("%s mqtt no shakehand",__FUNCTION__);
        return 0;
    }
    */
//    LOG("%s len=%d",__FUNCTION__,len);
	int rc = ty_ssl_recv(n->my_socket, buffer,  len,  timeout_ms);
    return rc;
}

int tls_linux_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
//        LOG("%s len=%d",__FUNCTION__,len);
	int rc =  ty_ssl_send (n->my_socket, buffer,  len,timeout_ms);
        // LOG("%s  tm=%d rc=%d",__FUNCTION__,timeout_ms,rc);
    return rc;
}

void tls_NetworkInit(Network* n)
{
	n->my_socket = NULL;
	n->mqttread = tls_linux_read;
	n->mqttwrite = tls_linux_write;
}

int tls_NetworkConnect(Network* n, char* addr, int port)
{
//     LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    int rc = -1;
//    int * mysock = ty_ssl_new ();
    n->my_socket = (int)ty_ssl_new ();
//    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
//    LOG("%s: ty_ssl_new mysock: %x",__func__, mysock);
    if(n->my_socket == NULL)
    {
        return rc;
    }
    rc = ty_ssl_cert_load (n->my_socket, n->auth.tls_ca.ca_cert, n->auth.tls_ca.cert_len);
    if(rc != 0)
    {
        return rc;
    }   
//     LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    return ty_ssl_connect (n->my_socket, addr, port);
}

void tls_NetworkDisconnect(Network* n)
{
//    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    if(n->my_socket)
	    ty_ssl_close(n->my_socket);
//    LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    n->my_socket = NULL;
}
#endif

int linux_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
    
	if(!n || !buffer)
        return -1;
    int rc =  ty_socket_recv (n->my_socket,buffer, len, timeout_ms);

    if(rc == TCP_MBEDTLS_ERR_SSL_TIMEOUT || rc == TCP_MBEDTLS_ERR_SSL_WANT_READ) //timeout
        rc = 0;
    return rc;
}

int linux_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
//    LOG("%s len=%d",__FUNCTION__,len);
	if(!n || !buffer)
        return -1;
	return ty_socket_send (n->my_socket, buffer, len,  timeout_ms);
}


void NetworkInit(Network* n)
{
    #if defined(XY_MBEDTLS) || defined(TY_MBEDTLS_CA)
    if(n->tls)
    {
        tls_NetworkInit (n);
        return;

    }
    #endif
	n->my_socket = 0;
	n->mqttread = linux_read;
	n->mqttwrite = linux_write;
}

int NetworkConnect(Network* n, char* addr, int port)
{
	int rc = -1;
#if defined(XY_MBEDTLS) || defined(TY_MBEDTLS_CA)
//     LOG("%s %d\r\n",__FUNCTION__,__LINE__);
    if(n->tls)
    {
//         LOG("%s %d\r\n",__FUNCTION__,__LINE__);
        return tls_NetworkConnect (n, addr,port);
    }
#endif    
	int socket = ty_socket_connect (addr,  port);
    if(socket < 0)
        return socket;
    else
        rc = 0;
    n->my_socket = socket;
	return rc;
}


void NetworkDisconnect(Network* n)
{
//#if defined(XY_MBEDTLS) || defined(TY_MBEDTLS_CA)

    if(n->tls)
    {
        tls_NetworkDisconnect (n);
        return ;
    }
//#endif
	ty_socket_close(n->my_socket);
}


void MutexInit(int *mtx)
{
}

void MutexLock(int *mtx)
{
}

void MutexUnlock(int *mtx)
{
}

int ThreadStart(int* thread, void (*fn)(void*), void* arg)
{
    return 0;
}


void tls_network_test(char* addr, int port)
{
    Network n = {0};
    n.tls = 1;
    NetworkInit(&n);
    NetworkConnect(&n, addr, port);

        while(1)
        {
            ty_sleep(2000);
            LOG("%s!\r\n", __func__);
        }
}

