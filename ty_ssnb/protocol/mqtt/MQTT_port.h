/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
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
 *******************************************************************************/

#if !defined(__MQTT_LINUX_)
#define __MQTT_LINUX_

#if defined(WIN32_DLL) || defined(WIN64_DLL)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#elif defined(LINUX_SO)
  #define DLLImport extern
  #define DLLExport  __attribute__ ((visibility ("default")))
#else
  #define DLLImport
  #define DLLExport
#endif
#define NULL ((void *)0)
typedef struct Timer
{
    int xTicksToWait;
	long xTimeOut;
} Timer;

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

typedef struct Network
{
    int tls; // 1 TLS,0 no TLS
    int psk_or_cert;
    union
    {
        struct
        {
            const unsigned char *psk;
            int psk_len;
            const unsigned char *psk_identity;
        }tls_psk;    
        struct
        {
            const unsigned char *ca_cert;
            int cert_len;
        }tls_ca;
    }auth;
	int my_socket;
	int (*mqttread) (struct Network*, unsigned char*, int, int);
	int (*mqttwrite) (struct Network*, unsigned char*, int, int);
} Network;

int linux_read(Network*, unsigned char*, int, int);
int linux_write(Network*, unsigned char*, int, int);

DLLExport void NetworkInit(Network*);
DLLExport int NetworkConnect(Network*, char*, int);
DLLExport void NetworkDisconnect(Network*);

#endif
