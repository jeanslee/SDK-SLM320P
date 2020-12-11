/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "sockets.h"
#include "cfw.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern struct netif *MG_NwGetGprsNetIf(uint8_t nSim, uint8_t nCid);
extern int MG_CFW_TcpipSocket(uint8_t nDomain, uint8_t nType, uint8_t nProtocol);
extern int MG_CFW_TcpipSocketSetsockopt(int nSocket, int level, int optname, const void *optval, int optlen);
extern uint32_t MG_CFW_TcpipSocketBind(SOCKET nSocket, CFW_TCPIP_SOCKET_ADDR *pName, uint8_t nNameLen);
extern int MG_CFW_TcpipSocketClose(int nSocket);
extern uint32_t MG_CFW_TcpipSocketConnect(int nSocket, CFW_TCPIP_SOCKET_ADDR *pName, uint8_t nNameLen);
extern int MG_CFW_TcpipSocketSend(int nSocket, uint8_t *pData, uint16_t nDataSize, uint32_t nFlags);
extern uint32_t MG_CFW_TcpipSocketRecv(int nSocket, uint8_t *pData, uint16_t nDataSize, uint32_t nFlags);
extern bool MG_NwHasActivePdp(uint8_t nSim);

static void PING_AsyncEventProcess(void *param)
{
    osiEvent_t *osiEv = (osiEvent_t *)param;
    osiTracePrintf(LOG_TAG, "enter PING_AsyncEventProcess");
    osiTracePrintf(LOG_TAG, "PING_AsyncEventProcess Got %d,0x%x,0x%x,0x%x",
             osiEv->id, osiEv->param1, osiEv->param2, osiEv->param3);
}

static void pingThreadEntry(void *param)
{
    bool active = false;
    int cnt = 0;
    while(cnt<30){
        MG_osiThreadSleep(5000);
        MG_osiTracePrintf(LOG_TAG, "waiting for network");
        cnt ++;
    }

    uint8_t nType, nProtocol;
    nType = SOCK_STREAM ;
    nProtocol = IPPROTO_TCP;
    uint8_t nSim = 0;
    uint8_t nCid = 1;
    ip_addr_t AT_PING_IP;
    int iResult;
    int keepalive = 0;
    int usock = 0;
    int iResultBind = ERR_OK;
    SOCKET socketfd = INVALID_SOCKET;
    struct netif *netif_default =lwip_getDefaultNetif();

    osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    uint32_t nReturnValue = MG_CFW_Gethostbyname("www.baidu.com", &AT_PING_IP, nCid, nSim);
    socketfd = MG_CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
    if (netif_default != NULL)
    {
        ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(netif_default);
        struct sockaddr_in stLocalAddr = {0};
        stLocalAddr.sin_len = sizeof(struct sockaddr_in);
        stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
        stLocalAddr.sin_addr.s_addr = ip_addr->addr;
        osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
        iResultBind = MG_CFW_TcpipSocketBind(socketfd, (CFW_TCPIP_SOCKET_ADDR *)&stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    }
    if (SOCKET_ERROR == iResultBind)
    {
        if (socketfd > 0)
            MG_CFW_TcpipSocketClose(socketfd);
        socketfd = INVALID_SOCKET;
    }

    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    SOCKET AT_PING_SOCKET = socketfd;
    //Get TTL
    int len = 1;
    uint8_t AT_PING_TTL = 0;
    if (0 != MG_CFW_TcpipSocketGetsockopt(socketfd, CFW_TCPIP_IPPROTO_IP, IP_TTL, &AT_PING_TTL, &len))
    {
        MG_osiTracePrintf(LOG_TAG, "AT_TCPIP_CmdFunc_PING: Using DEFAULT TTL");
        AT_PING_TTL = 255;
    }
    int16_t AT_PING_PACKET_LEN = 16;
    struct sockaddr_storage to;
    uint8_t ipData[1500] = {0};
    struct icmp_echo_hdr *idur;
    idur = (struct icmp_echo_hdr *)ipData;

    ICMPH_CODE_SET(idur, 0);
    idur->id = 1; 
    idur->seqno = 1;
    // in 16384Hz ticks.
    *((uint32_t *)(ipData + 8)) = (uint32_t)MG_osiUpTime();
    *((uint32_t *)(ipData + 12)) = socketfd;
    for (uint16_t i = 0; i < AT_PING_PACKET_LEN - 16; i++)
    {
        ipData[i + 16] = i;
    }
    idur->chksum = 0;
    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    MG_CFW_TcpipSocketSetParam(socketfd, PING_AsyncEventProcess, 0);
    if (IP_IS_V4(&AT_PING_IP))
    {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&to;
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&AT_PING_IP));
        ICMPH_TYPE_SET(idur, ICMP_ECHO);
        idur->chksum = inet_chksum(idur, AT_PING_PACKET_LEN);
    }

    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    if (-1 == MG_CFW_TcpipSocketSendto(socketfd, ipData, AT_PING_PACKET_LEN, 0, (CFW_TCPIP_SOCKET_ADDR *)&to, sizeof(to)))
    {
        MG_CFW_TcpipSocketClose(AT_PING_SOCKET);
        AT_PING_SOCKET = INVALID_SOCKET;
    }
    while(true){
        osiEvent_t event;
        event.id = OSI_EVENT_ID_NONE;
        MG_osiEventWait(osiThreadCurrent(), &event);   
        MG_osiTracePrintf(LOG_TAG, "got event");
        if (event.id == OSI_EVENT_ID_QUIT)
            break;
    }


    MG_osiThreadExit();
}


static void prvThreadEntry(void *param)
{
    bool active = false;
    while(!active){
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "no active pdp context,check again");
        active = MG_NwHasActivePdp(0);
    }

    uint8_t nType, nProtocol;
    nType = SOCK_STREAM ;
    nProtocol = IPPROTO_TCP;
    uint8_t nSim = 0;
    uint8_t nCid = 1;
    struct netif *pnetif = NULL;
    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    int iResult;
    int keepalive = 0;
    int usock = 0;

    uint16_t uPort = 5000;
    ip4_addr_t nIpAddr;
    nIpAddr.addr = PP_HTONL(LWIP_MAKEU32(139,196,39,160));

    struct sockaddr_in  nDestAddr;
    struct sockaddr_in *to4 = &nDestAddr;
    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);

    pnetif = lwip_getDefaultNetif();
    to4->sin_len = sizeof(struct sockaddr_in);
    to4->sin_family = AF_INET;
    to4->sin_port = uPort;

    inet_addr_from_ip4addr(&to4->sin_addr, &nIpAddr);

    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    iResult = MG_CFW_TcpipSocket(AF_INET, nType, nProtocol);
    if (-1 == iResult)
    {
        MG_osiTracePrintf(LOG_TAG,"AT_TCPIP_Connect(), CFW_TcpipSocket() failed\n");
        MG_osiThreadExit();
        return; 
    }
    usock = iResult;
    int n = 1;
    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    /*
    MG_CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_REUSEADDR, (void *)&n, sizeof(n));
    ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(pnetif);

    CFW_TCPIP_SOCKET_ADDR stLocalAddr = {0};
    stLocalAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
    stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
    stLocalAddr.sin_port = 0;
    stLocalAddr.sin_addr.s_addr = ip_addr->addr;

    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);

    int ret = MG_CFW_TcpipSocketBind(iResult, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    */

    ip4_addr_t *ip_addr = (ip4_addr_t *)netif_ip4_addr(pnetif);
    CFW_TCPIP_SOCKET_ADDR stLocalAddr = {0,};
    stLocalAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);;
    stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
    stLocalAddr.sin_port = 0;

    stLocalAddr.sin_addr.s_addr = ip_addr->addr;

    MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
    int ret = MG_CFW_TcpipSocketBind(usock, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    if (-1 == ret)
    {
        MG_osiTracePrintf(LOG_TAG, "AT_TCPIP_Connect(), CFW_TcpipSocketBind() failed\n");
        int iRetValue;
        iRetValue = MG_CFW_TcpipSocketClose(iResult);
        MG_osiTracePrintf(LOG_TAG, "AT_TCPIP_Connect(): close ");
    }else{
        usock = iResult;
        MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
        MG_CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));

        MG_osiTracePrintf(LOG_TAG, "%s--%d",__func__,__LINE__);
        iResult = MG_CFW_TcpipSocketConnect(usock, (CFW_TCPIP_SOCKET_ADDR *)&nDestAddr, sizeof(nDestAddr));
        if(iResult == -1){
            MG_osiTracePrintf(LOG_TAG, "connect error");
        }else{
            char *buf="it is a test";
            uint8_t data[128]={0};
            MG_osiTracePrintf(LOG_TAG, "connect ok");
            if(MG_CFW_TcpipSocketSend(usock,(uint8_t*)buf,strlen(buf),0) != -1){
                MG_osiTracePrintf(LOG_TAG, "send ok");
                MG_osiThreadSleep(1);
                if(MG_CFW_TcpipSocketRecv(usock,data,sizeof(data),0) != -1)
                    MG_osiTracePrintf(LOG_TAG, "receive %s",data);
                else
                    MG_osiTracePrintf(LOG_TAG, "receive err");
            }else{
                MG_osiTracePrintf(LOG_TAG, "send err");
            }
        }

        MG_CFW_TcpipSocketClose(usock);
    }

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgsock", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
