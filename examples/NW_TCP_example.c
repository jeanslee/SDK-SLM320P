

#define LOG_TAG OSI_MAKE_LOG_TAG('B', 'L', 'N', 'W')

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "mg_callback_api.h"
#include "sockets.h"


#define YOUR_PORT 9101

static int mg_data_call = 0;
static int connfd = -1;
static int mg_socket_connect_flag=0;

//get sim card status , get apn , get register  status,
void simQuery(int *simStatus, int *registerStatus, char name[THE_APN_MAX_LEN])
{
	int len = 8;
	//get sim status
	while(1)
	{
		*simStatus = MG_cfwGetSimStatus(0);
		if(*simStatus == CFW_SIM_ABSENT) 
			{
				MG_osiTracePrintf(LOG_TAG, "NO SIM");
				MG_osiThreadSleep(1000);
			}
		else
			{
				MG_osiTracePrintf(LOG_TAG, "SIM Ready");
				//MG_osiThreadSleep(1000);
				break;
			}
	}
	
	MG_osiThreadSleep(10000);
	

	//get apn
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
	MG_osiTracePrintf(LOG_TAG, "Operators : %x,%x,%x,%x,%x",name[0],name[1],name[2],name[3],name[4]);
	MG_osiTracePrintf(LOG_TAG, "Operators : %s",name);
	MG_osiThreadSleep(5000);

	
	//get register  status
	
	u8_t status;
	
	uint32_t uErrCode;
	while(1)
	{
		uErrCode=MG_CFW_GetGprsAttState(&status,0);
		if(uErrCode == 0)
		{
			*registerStatus = status;
			MG_osiTracePrintf(LOG_TAG, "register status: %d", *registerStatus);
			if(status==1)
			{
				MG_osiTracePrintf(LOG_TAG, "nw register succeed!");
				break;
			}
		}
		else
		{
			*registerStatus = -1;
		}
		MG_osiThreadSleep(1000);
	}
	
	MG_osiThreadSleep(10000);

	//get  signal level
	uint8_t rat = 0, biterr = 0;uint8_t siglevel=0;
	if(MG_NwGetSignalQuality(&rat, &siglevel, &biterr)){
	MG_osiTracePrintf(LOG_TAG, "network singnal is %d.",siglevel);
	
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "fail to get network singnal .");
	}
	
}

//modify the pdp ctxt , atctive pdp, 
void int_net(const char *pApn)
{
	uint8_t cid = 1;
	if(MG_NwModifyPdpContext(0,cid,1,pApn))
	{
		MG_osiTracePrintf(LOG_TAG, "modify pdp ctxt success");
		MG_osiThreadSleep(10000);
		if(MG_NwActivePdpContext(0,cid))
		{
			MG_osiThreadSleep(20000);
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
		
		mg_data_call = 1;
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "fail to modify pdp context!");
	}
	
    return ;
}



//connect the remote server 
static void Connect_ThreadEntry(void *param)
{
	//
	int ret=0;

	ip4_addr_t nIpAddr;
	nIpAddr.addr = PP_HTONL(LWIP_MAKEU32(104,224,176,31));//your server ip

	struct sockaddr_in  nDestAddr;
	struct sockaddr_in *to4 = &nDestAddr;

	to4->sin_len = sizeof(struct sockaddr_in);
	to4->sin_family = AF_INET;
	to4->sin_port = lwip_htons(YOUR_PORT);//your server port

	inet_addr_from_ip4addr(&to4->sin_addr, &nIpAddr);

		

    	//select
	int maxfd ;//mg_socket_connect_flag=0;
	fd_set read_set;
	fd_set write_set;
	struct timeval time_counter = {.tv_sec=10,.tv_usec=0};
	char buf[56] = {0};
    	char data[100]={0};
	int select_val = 0;
	static unsigned int counter=0;
	while(1)
	{

		if(mg_data_call == 0)
		{
			MG_osiTracePrintf(LOG_TAG, "wait for call !");
			MG_osiThreadSleep(1000);
	    		continue;
		}
		else if(mg_socket_connect_flag == 0)
		{
			connfd = lwip_socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(connfd==-1)
			{
				MG_osiTracePrintf(LOG_TAG, "fail to apply socket !");
				continue;
			}
			MG_osiTracePrintf(LOG_TAG," MG_CFW_TcpipSocket , connfd:%d!",connfd);
			ret=lwip_connect(connfd,(const struct sockaddr *)&nDestAddr,sizeof(nDestAddr));

			if (ret != 0)
			{
				MG_osiTracePrintf(LOG_TAG, "fail to lwip_connect server!");
				lwip_close(connfd);
				MG_osiThreadSleep(500);
				
				continue;
			}
			maxfd = connfd + 1;
			mg_socket_connect_flag=1;
		}

	
		MG_osiThreadSleep(500);
		counter++;
		memset(buf,0,56);
		
		snprintf(buf,56,"%d,%s",counter,"TomCat is cat, you are very smart and cute!\r\n");
		if(lwip_send(connfd, buf, strlen(buf), 0) == -1)
		{
			MG_osiTracePrintf(LOG_TAG, "write failed");
			if(mg_socket_connect_flag)
			{
				lwip_close(connfd);
				mg_socket_connect_flag = 0;
			}
			
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "write success");
		}

		FD_ZERO(&read_set);
       		FD_SET(connfd, &read_set);
        	
       		 //
        		time_counter .tv_sec=10;
        		time_counter.tv_usec=0;
		select_val = select(maxfd,&read_set,NULL,NULL,&time_counter);
		MG_osiTracePrintf(LOG_TAG, "select_val = %d, read_set = %d", select_val, read_set);
		if(select_val < 0)
		{
			MG_osiTracePrintf(LOG_TAG, "select error!");
		}
		else if (select_val == 0)
		{
			MG_osiTracePrintf(LOG_TAG, "select timeout!");
		}
		else
		{
			if(FD_ISSET(connfd, &read_set))
			{
				memset(data,0,100);
				if(lwip_recv(connfd, data, 20, 0) == -1)
				{
					MG_osiTracePrintf(LOG_TAG, "read failed");
					if(mg_socket_connect_flag)
					{
						lwip_close(connfd);
						mg_socket_connect_flag = 0;
					}
				}
				else
				{
					MG_osiTracePrintf(LOG_TAG, "read : %s", data);
					MG_SendMessage_to_AT_Serial(data,strlen(data));
					
				}
				
			}
			
		}
		
	}
    

    MG_osiThreadExit();
}


/*static void  SerialPort_ThreadEntry(void *param)
{
	
	while(1)
	{
		MG_osiThreadSleep(2000);
	}
}*/


osiThread_t * Net_IND_Handle=NULL;
osiThread_t * TCP_Handle=NULL;

void Register_Net(void)
{
	osiEvent_t Mess_event = {.id = EV_PS_DOMAIN_REGISTER};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
}

void NO_Register_Net(void)
{
	osiEvent_t Mess_event = {.id = EV_PS_DOMAIN_NO_REGISTER};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
   	
}


void Auto_pdp_act(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_AUTO_ACTIVATION};
	
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
}

void Auto_pdp_deact(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_AUTO_DEACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
}

void pdp_act(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_ACTIVE_ACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
}

void pdp_deact(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_ACTIVE_DEACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)Net_IND_Handle, &Mess_event);
}

static void Net_Ind_ThreadEntry(void *param)
{
	osiEvent_t event_to = {.id = EV_PDP_AUTO_DEACTIVATION};

   	//simStatus--->
	//registerStatus--->
	//name--->
	//siglevel--->
	int simStatus = 0, registerStatus = 0;
	char name[THE_APN_MAX_LEN] = {0};
	uint8_t siglevel = 0;

	simQuery(&simStatus, &registerStatus, name);
	if(simStatus != 0)
	{
		int_net(name);
	   	MG_osiTracePrintf(LOG_TAG, "init net succeed");
	}
	else
	{
	    	MG_osiTracePrintf(LOG_TAG, "sim error : %d", simStatus);
	}

	MG_registCallback(EV_PS_DOMAIN_REGISTER,Register_Net);//注测网络
	MG_registCallback(EV_PS_DOMAIN_NO_REGISTER,NO_Register_Net);//no 注测网络
	MG_registCallback(EV_PDP_AUTO_ACTIVATION,Auto_pdp_act);//自动激活pdp
	MG_registCallback(EV_PDP_AUTO_DEACTIVATION,Auto_pdp_deact);//自动去激活pdp
	MG_registCallback(EV_PDP_ACTIVE_ACTIVATION,pdp_act);//手动激活pdp
	MG_registCallback(EV_PDP_ACTIVE_DEACTIVATION,pdp_deact);//手动去激活pdp
	
	while(1)
	{
		osiEvent_t event = {0};
		osiThread_t *M_handle=MG_osiThreadCurrent();
		MG_osiEventWait(MG_osiThreadCurrent(), &event);
		switch(event.id)
		{
			case EV_PS_DOMAIN_REGISTER://
				MG_osiTracePrintf(LOG_TAG,"notify the Attach succeed!");
				int_net(name);
				break;
			case EV_PS_DOMAIN_NO_REGISTER://
				MG_osiTracePrintf(LOG_TAG,"notify DE Attach!");
				//event_to.id=80001;
				//MG_osiEventSend((osiThread_t *)TCP_Handle, &event_to);
				//simQuery(&simStatus, &registerStatus, name);
				//int_net(name);
				if(mg_socket_connect_flag==1)
				{
					mg_socket_connect_flag=0;
					lwip_close(connfd);
				}
				mg_data_call = 0;
				break;
			case EV_PDP_AUTO_ACTIVATION://
				MG_osiTracePrintf(LOG_TAG,"notify Auto_pdp_act!");
				break;
			case EV_PDP_AUTO_DEACTIVATION://
				MG_osiTracePrintf(LOG_TAG,"notify Auto_pdp_deact!");
				//event_to.id=80001;
				//MG_osiEventSend((osiThread_t *)TCP_Handle, &event_to);
				//int_net(name);
				if(mg_socket_connect_flag==1)
				{
					mg_socket_connect_flag=0;
					lwip_close(connfd);
				}
				mg_data_call = 0;
				break;
			case EV_PDP_ACTIVE_ACTIVATION://
				MG_osiTracePrintf(LOG_TAG,"notify pdp_act!");
				
				break;
			case EV_PDP_ACTIVE_DEACTIVATION://
				MG_osiTracePrintf(LOG_TAG,"notify pdp_deact!");
				//event_to.id=80001;
				//MG_osiEventSend((osiThread_t *)TCP_Handle, &event_to);
				//int_net(name);
				if(mg_socket_connect_flag==1)
				{
					mg_socket_connect_flag=0;
					lwip_close(connfd);
				}
				mg_data_call = 0;
				break;
			default:
			
				break;
			
		}
		
		//MG_osiThreadSleep(2000);
		
	}
	
}


int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "baolan example enter");
    //MG_osiThreadCreateE("baolanserial", SerialPort_ThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1*1024,10);
    Net_IND_Handle= MG_osiThreadCreateE("baolannet_ind", Net_Ind_ThreadEntry, NULL, OSI_PRIORITY_NORMAL, 2*1024,10);
    TCP_Handle=MG_osiThreadCreateE("baolan_connect", Connect_ThreadEntry, NULL, OSI_PRIORITY_NORMAL, 8*1024,10);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "baolan example exit");
}



