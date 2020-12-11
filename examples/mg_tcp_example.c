#define LOG_TAG OSI_MAKE_LOG_TAG('B', 'L', 'N', 'W')

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#include "sockets.h"


#define YOUR_PORT 9101
#define LOCAL_PORT 9700

//获取apn，有内容返回false，没有返回true
bool getApn(char name[THE_APN_MAX_LEN]){
	CFW_GPRS_PDPCONT_DFTPDN_INFO PdnInfo = {0,};
	memset(&PdnInfo, 0, sizeof(CFW_GPRS_PDPCONT_DFTPDN_INFO));
	MG_CFW_GprsGetDefaultPdnInfo(&PdnInfo, 0);
	memcpy(name, PdnInfo.AccessPointName, PdnInfo.APNLen);
	for(int i = 0; ((i < PdnInfo.APNLen)&&(PdnInfo.AccessPointName[i]!='\0')); ++i)
	{
		if(PdnInfo.AccessPointName[i] == '.'){
		name[i] = '\0';
		break;
		}
		name[i] = PdnInfo.AccessPointName[i];
	}
	return (name[0]) ? false : true ;
}

//查询sim卡状态，注册状态，运营商名称
void simQuery(int *simStatus, int *registerStatus, char name[THE_APN_MAX_LEN])
{
	//获取sim卡状态
	int len = 8;
	*simStatus = MG_cfwGetSimStatus(0);
	if(simStatus == 0) return;
	MG_osiTracePrintf(LOG_TAG, "SIM status : %d", *simStatus);

	//获取运营商名称
	while(getApn(name)){
		MG_osiThreadSleep(1000);
	}
	MG_osiTracePrintf(LOG_TAG, "Operators : %s", name);

	//获取网络状态
	CFW_NW_STATUS_INFO sStatus;
	uint32_t uErrCode = MG_CFW_GprsGetstatus(&sStatus, 0);
	if(uErrCode == 0)
	{
		*registerStatus = sStatus.nStatus;
	}
	else
	{
		*registerStatus = -1;
	}
	MG_osiTracePrintf(LOG_TAG, "register status: %d", *registerStatus);

}

//注册网络并获取信号强度
uint8_t int_net(const char *pApn, uint8_t *siglevel)
{
	uint8_t cid = 1;
	if(MG_NwModifyPdpContext(0,cid,1,pApn))
	{
		MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt success");
		if(MG_NwActivePdpContext(0,cid))
		{
			MG_osiThreadSleep(10000);
			MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "activate pdp ctxt failure");
		}
		bool active = false;
		while(!active){
		MG_osiThreadSleep(1000);
		MG_osiTracePrintf(LOG_TAG, "no active pdp context,check again");
		active = MG_NwHasActivePdp(0);
		}
		
		uint8_t rat = 0, biterr = 0;
		if(MG_NwGetSignalQuality(&rat, siglevel, &biterr)){
		MG_osiTracePrintf(LOG_TAG, "have active pdp context.");
		return *siglevel;
		}
	}
	
    return -1;
}


int binder(char localip[24], int connfd){

	//devide ip
	int ip[4]={0};
    int ip_bit = 0, j = 0;
    for(int c = 0; c < 24; ++c){
        ip_bit = ((ip_bit*10)+(localip[c]-'0'));
        if(ip[c]=='.'){
            ip[j++] = ip_bit;
            MG_osiTracePrintf(LOG_TAG, "ip out : %d", ip_bit);
            ip_bit = 0;
        }
    }

	//local ip and port
	ip4_addr_t nIpAddr;
	nIpAddr.addr = PP_HTONL(LWIP_MAKEU32(ip[0],ip[1],ip[2],ip[3]));
	struct sockaddr_in  localAddr;
	struct sockaddr_in *to4 = &localAddr;
	to4->sin_len = sizeof(struct sockaddr_in);
	to4->sin_family = AF_INET;
	to4->sin_port = lwip_htons(LOCAL_PORT);
	inet_addr_from_ip4addr(&to4->sin_addr, &nIpAddr);


	return MG_CFW_TcpipSocketBind(connfd, (const struct sockaddr *)&localAddr, sizeof(localAddr));

}


static void prvThreadEntry(void *param)
{
	//simStatus--->sim卡状态
	//registerStatus--->注册状态
	//name--->运营商名称
	//siglevel--->信号强度
	int simStatus = 0, registerStatus = 0;
	char name[THE_APN_MAX_LEN] = {0};
	uint8_t siglevel = 0;

	simQuery(&simStatus, &registerStatus, name);
	if(simStatus != 0){
	    if(-1 == int_net(name, &siglevel))
	        MG_osiTracePrintf(LOG_TAG, "register failed");
	    else
	        MG_osiTracePrintf(LOG_TAG, "signal quality : %d", siglevel);
	}else{
	    MG_osiTracePrintf(LOG_TAG, "sim error : %d", simStatus);
	}

	//创建套接字描述符
	int connfd;
	connfd = lwip_socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	MG_osiTracePrintf(LOG_TAG," MG_CFW_TcpipSocket , connfd:%d!",connfd);

    //设置心跳包
    int keepalive = 10;
    MG_CFW_TcpipSocketSetsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));

	//获取本地ip
	char localip[24] = {0};
	int sim = 0;
    if(MG_getLocalAddr(sim, localip)){
        MG_osiTracePrintf(LOG_TAG, "get local ip fail");
    }else{
        MG_osiTracePrintf(LOG_TAG, "local ip : %s", localip);
    }

	//绑定你要绑定的本地ip
	//ip格式为"x.x.x.x"
	if(binder(localip, connfd))
		MG_osiTracePrintf(LOG_TAG, "bind fail");
	else
		MG_osiTracePrintf(LOG_TAG, "bind success");

	
	//构造服务器地址
	ip4_addr_t nIpAddr;
	nIpAddr.addr = PP_HTONL(LWIP_MAKEU32(104,224,176,31));
	struct sockaddr_in  nDestAddr;
	struct sockaddr_in *to4 = &nDestAddr;
	to4->sin_len = sizeof(struct sockaddr_in);
	to4->sin_family = AF_INET;
	to4->sin_port = lwip_htons(YOUR_PORT);
	inet_addr_from_ip4addr(&to4->sin_addr, &nIpAddr);

	//连接
	int ret=0;
	ret=lwip_connect(connfd, (const struct sockaddr *)&nDestAddr, sizeof(nDestAddr));
	int err = CFW_TcpipGetLastError();
	MG_osiTracePrintf(LOG_TAG," connect , ret1:%d,err :%d!",ret,err);
	if(ret!=0)
	{
		MG_osiTracePrintf(LOG_TAG, "connect failed");
		goto Finish;
	}
	
    //设置select
	int maxfd = connfd + 1;
	fd_set read_set;
	fd_set write_set;
	struct timeval time_counter = {.tv_sec=10,.tv_usec=0};

	char *buf = "TomCat is cat!";
    char data[20]={0};
	int select_val = 0;
	while(1)
	{
		MG_osiThreadSleep(2000);
		if(lwip_send(connfd, buf, 14, 0) == -1){
			MG_osiTracePrintf(LOG_TAG, "write failed");
		}else{
			MG_osiTracePrintf(LOG_TAG, "write success");
		}
		FD_ZERO(&read_set);
       	FD_SET(connfd, &read_set);
		
       	//开启select
		time_counter.tv_sec=10;
		time_counter.tv_usec=0;
		select_val = select(maxfd,&read_set,NULL,NULL,&time_counter);
		MG_osiTracePrintf(LOG_TAG, "select_val = %d, read_set = %d", select_val, read_set);
		if(select_val < 0){
			MG_osiTracePrintf(LOG_TAG, "select error!");
		}else if (select_val == 0){
			MG_osiTracePrintf(LOG_TAG, "select timeout!");
		}else{
			if(FD_ISSET(connfd, &read_set)){
				if(lwip_recv(connfd, data, 20, 0) == -1){
					MG_osiTracePrintf(LOG_TAG, "read failed");
					
				}else{
					MG_osiTracePrintf(LOG_TAG, "read : %s", data);
				}
			}
		}
	}
    
Finish:
    //发生错误，关闭
    MG_CFW_TcpipSocketClose(connfd);
    MG_osiThreadExit();
}


int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "baolan example enter");
    MG_osiThreadCreate("baolan", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 8*1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "baolan example exit");
}



