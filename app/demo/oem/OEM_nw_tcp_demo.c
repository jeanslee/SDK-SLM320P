

#define LOG_TAG OSI_MAKE_LOG_TAG('B', 'L', 'N', 'W')
#define OSI_LOG_TAG  LOG_TAG

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "mg_callback_api.h"
#include "sockets.h"

void prvRrcordThread(void *param);
void KeyPad_ThreadEntry(void *param);
void show_csq_value(void);
#include "oem.h"
#define YOUR_PORT 9101

#define MEIG_TCP_TEST_IP      "104.224.176.31"
#define MEIG_TCP_TEST_PORT     9101
#define MEIG_UDP_TEST_IP      "104.224.176.31"
#define MEIG_UDP_TEST_PORT     9102
#define TCP_PER_TIME_MS  20//200
#define TCP_TOTAL_TIME_MS  200//200
#define NET_CHECK_TIME_MS  4000//200
#define NET_RECOVERY_TIME_MS  20000//200

#define  EVENT_TIME_FOR_NET_IND     1000
//#define  EVENT_TIME_FOR_NET_IND     1001
//#define  EVENT_TIME_FOR_NET_IND     1002

static int mg_data_call = 0;
static int connfd = -1;
static int connfd1 = -1;
static int mg_socket_connect_flag=0;
static osiTimer_t *g_tcp_timer=NULL;
static int Send_Data_200ms = 0;
static osiTimer_t *g_net_check_timer=NULL;
osiThread_t * meig_Net_IND_Handle=NULL;
osiThread_t * meig_TCP_Handle=NULL;
int g_net_total_time_ms=0;
int g_tcp_time_ms_total=0;

void baolan_fota_update(void)
{
    #define FOTA_FILE_NAME "/pacmd5.pack"

	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg update start");

    int fd = vfs_open(FOTA_FILE_NAME,0);
    if(fd>0)
	{
        vfs_close(fd);
		if(!MG_updateFirmwareMd5(FOTA_FILE_NAME))
		{
			MG_osiTracePrintf(LOG_TAG, "mg update failed");
		}
		MG_osiThreadSleep(1000);
		MG_osiTracePrintf(LOG_TAG, "mg update end");
    }else
    {
		MG_osiTracePrintf(LOG_TAG, "mg update file not exist");
    }
}

void meig_play_ttsCB(void)
{
    //to do something
    MG_osiTracePrintf(LOG_TAG, "TTS:doing something here\n");
}

int meig_play_tts_text(void)
{
    char *str = "已收到UDP数据"; //"hello world";
    int len = strlen(str);

    OSI_LOGI(LOG_TAG, "tts string len = %d", len);
    MG_osiThreadSleep(1000);
    if (!MG_ttsPlayerInit())
    {
    	MG_osiTracePrintf(LOG_TAG, "tts init fail");
    }	
    if (MG_ttsIsPlaying())
    {
		MG_osiTracePrintf(LOG_TAG, "tts is busy");
		return -1;
    }
    if (!MG_ttsPlayText(str, len, meig_play_ttsCB, NULL))
    {
    	MG_osiTracePrintf(LOG_TAG, "tts play fail");
    }
	return 0;
}

void simQuery(int *simStatus, int *registerStatus, char name[THE_APN_MAX_LEN])
	{
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
		
		MG_osiThreadSleep(1000);
		
	
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
		
		//get register	status
		
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
		
		MG_osiThreadSleep(1000);
	
		//get  signal level
		uint8_t rat = 0, biterr = 0;uint8_t siglevel=0;
		if(MG_NwGetSignalQuality(&rat, &siglevel, &biterr))
		{
			MG_osiTracePrintf(LOG_TAG, "network singnal is %d.",siglevel);
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "fail to get network singnal .");
		}
		
	}


//modify the pdp ctxt , atctive pdp, 
void int_net(void)
{
 	bool active = false;

	if(mg_data_call ==1)
	{
		return;
	}
	active = MG_NwHasActivePdp(0);
	while(!active){
		MG_osiThreadSleep(1000);
		active = MG_NwHasActivePdp(0);
		MG_osiTracePrintf(LOG_TAG, "no active pdp context,check again");	
	}
	MG_osiTracePrintf(LOG_TAG, "active pdp context sucess");
	show_service_state(true);
	mg_data_call = 1;
}

void baolan_tcp_timer_callback(void *param)
{
	g_tcp_time_ms_total += TCP_PER_TIME_MS;
	if(g_tcp_time_ms_total >= TCP_TOTAL_TIME_MS)
	{
		Send_Data_200ms=1;
		g_tcp_time_ms_total=0;
	}
	//MG_osiTimerStart(g_tcp_timer, TCP_PER_TIME_MS);
	
}


int baolan_send_tcp_data()
{
	static unsigned int timer=0;
	char buf[56] = {0};

	if(Send_Data_200ms)
	{
		MG_osiTracePrintf(LOG_TAG, "timer_callback");
		timer++;
		memset(buf,0,56);
		
		snprintf(buf,56,"%d,%s",timer,"TomCat is cat, you are very smart and cute!\r\n");
		if(lwip_send(connfd, buf, strlen(buf), 0) == -1)
		{
			MG_osiTracePrintf(LOG_TAG, "write failed");
			if(mg_socket_connect_flag)
			{
				lwip_close(connfd);
				mg_socket_connect_flag = 0;
				return -1;
			}
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "write success");
		}
		Send_Data_200ms=0;
	}
	return 0;	
}

int baolan_create_tcp_connect()
{
	int ret=0;
	struct sockaddr_in	nDestAddr;
	struct sockaddr_in *to4 = &nDestAddr;
	uint32_t ip_v4 =0;

	ip_v4 = inet_addr(MEIG_TCP_TEST_IP);
	to4->sin_addr.s_addr=ip_v4;
	to4->sin_len = sizeof(struct sockaddr_in);
	to4->sin_family = AF_INET;
	to4->sin_port = lwip_htons(MEIG_TCP_TEST_PORT);

	connfd = lwip_socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(connfd==-1)
	{
		MG_osiTracePrintf(LOG_TAG, "fail to apply socket !");
		return -1;
	}
	MG_osiTracePrintf(LOG_TAG," MG_CFW_TcpipSocket , connfd:%d!",connfd);
	ret=lwip_connect(connfd,(const struct sockaddr *)&nDestAddr,sizeof(nDestAddr));
	
	if (ret != 0)
	{
		MG_osiTracePrintf(LOG_TAG, "fail to lwip_connect server!");
		lwip_close(connfd);
		return -1;
	}
	return 0;
}

//connect the remote server 


void OEM_sendTcpThread(void *param)
{
	g_tcp_timer= MG_osiTimerCreate(NULL, baolan_tcp_timer_callback, NULL);	//create timer
	if(g_tcp_timer == NULL)
	{
		MG_osiTracePrintf(LOG_TAG, "create timer failed");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "create timer success");
		
		if(MG_osiTimerSetPeriod(g_tcp_timer, TCP_PER_TIME_MS,true ))		//set periodic=true
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer failed");
		}

		MG_osiTimerStartLast(g_tcp_timer, TCP_PER_TIME_MS);
	}
	while(1)
	{
		if(mg_data_call == 0)
		{
			MG_osiTracePrintf(LOG_TAG, "wait for call !");
			show_tcpdata_state(false);
			MG_osiThreadSleep(1000);
			continue;
		}
		else if(mg_socket_connect_flag == 0)
		{
		   if(-1==baolan_create_tcp_connect())
		   	{
			   MG_osiThreadSleep(10);
			   continue;
		   	}
			mg_socket_connect_flag=1;
		}
		MG_osiThreadSleep(10);
		baolan_send_tcp_data();
	}
	MG_osiTimerStop(g_tcp_timer);
	MG_osiTimerDelete(g_tcp_timer);
    MG_osiThreadExit();
}

void baolan_send_udp_data()
{
	char buf[56] = {0};
	struct sockaddr_in  stLocalAddr_l = {0};
	struct sockaddr_in  stLocalAddr_r = {0};	
	socklen_t addrLen= sizeof(stLocalAddr_r);
	uint32_t ip_v4 = 0;
	
	ip_v4 = inet_addr(MEIG_UDP_TEST_IP);	 		 
	stLocalAddr_l.sin_len = 0;
	stLocalAddr_l.sin_family = AF_INET;
	stLocalAddr_l.sin_port = htons(MEIG_UDP_TEST_PORT);
	stLocalAddr_l.sin_addr.s_addr =INADDR_ANY;
		 
	stLocalAddr_r.sin_len = 0;
	stLocalAddr_r.sin_family = AF_INET;
	stLocalAddr_r.sin_port = htons(MEIG_UDP_TEST_PORT);
	stLocalAddr_r.sin_addr.s_addr=ip_v4;

	if(connfd1==-1)
	{
		connfd1 = lwip_socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(connfd1==-1)
		{
		   MG_osiTracePrintf(LOG_TAG, "fail to apply socket !");
		   return;
		}
				
		lwip_bind(connfd1, (const struct sockaddr *)&stLocalAddr_l, sizeof(stLocalAddr_l));
	}
	snprintf(buf,56,"%s","hello baolan\r\n");
	MG_osiTracePrintf(LOG_TAG, "udp connfd1=%d",connfd1);	
	if(lwip_sendto(connfd1, buf, strlen(buf), 0,(struct sockaddr *)&stLocalAddr_r,addrLen) == -1)
	{
		MG_osiTracePrintf(LOG_TAG, "write failed");
		lwip_close(connfd1);
		return;
	}
	else
	{		
		MG_osiTracePrintf(LOG_TAG, "write success");
	}
}

void OEM_TcpThread(void *param)
{
	int maxfd ;//mg_socket_connect_flag=0;
	fd_set read_set;
	struct timeval time_counter = {.tv_sec=10,.tv_usec=0};
	char data[100]={0};
	int select_val = 0;

	while(1)
	{
		if(mg_socket_connect_flag == 0)
		{
			   MG_osiTracePrintf(LOG_TAG, "wait TCP connect!");
			   MG_osiThreadSleep(1000);
			   continue;
		}
		FD_ZERO(&read_set);
		FD_SET(connfd, &read_set);
		FD_SET(connfd1, &read_set);
		if(connfd1 > connfd)
		{
			maxfd = connfd1 + 1;
		}else
		{
			maxfd = connfd + 1;
		}
		//select check read_set
		time_counter .tv_sec=3;
		time_counter.tv_usec=0;
		select_val = select(maxfd,&read_set,NULL,NULL,&time_counter);
		MG_osiTracePrintf(LOG_TAG, "select_val = %d, read_set = %d", select_val, read_set);
		if(select_val < 0)
		{
			MG_osiTracePrintf(LOG_TAG, "select error!");
		}
		else if (select_val == 0)
		{
			show_tcpdata_state(false);
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
					show_tcpdata_state(true);
					MG_osiTracePrintf(LOG_TAG, "TCP data read");
					//char buffer[20]="TCP data read";
					//MG_SendMessage_to_AT_Serial(buffer,strlen(buffer));										
				}
			}
			else if(FD_ISSET(connfd1, &read_set))
			{
				memset(data,0,100);
				if(lwip_recv(connfd1, data, 20, 0) == -1)
				{
					MG_osiTracePrintf(LOG_TAG, "read failed");
					lwip_close(connfd1);
				}
				else
				{
					meig_play_tts_text();
					baolan_fota_update();
					show_tcpdata_state(true);
					MG_osiTracePrintf(LOG_TAG, "UDP data read : %s", data);
					char buffer[20]="UDP data read";
					MG_SendMessage_to_AT_Serial(buffer,strlen(buffer));	
				}
			}
		}
	}
    MG_osiThreadExit();
}



void Register_Net(void)
{
	osiEvent_t Mess_event = {.id = EV_PS_DOMAIN_REGISTER};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
}

void NO_Register_Net(void)
{
	osiEvent_t Mess_event = {.id = EV_PS_DOMAIN_NO_REGISTER};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
	show_service_state(false); 	
}


void Auto_pdp_act(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_AUTO_ACTIVATION};
	
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
}

void Auto_pdp_deact(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_AUTO_DEACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
}

void pdp_act(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_ACTIVE_ACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
}

void pdp_deact(void)
{
	osiEvent_t Mess_event = {.id = EV_PDP_ACTIVE_DEACTIVATION};
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
	show_service_state(false);   			
}

void baolan_net_recovery()
{
	MG_osiTracePrintf(LOG_TAG, "baolan_net_recovery");
	MG_SetFlightMode(0,0);
	MG_osiThreadSleep(500);
	MG_SetFlightMode(0,1);
}

static void net_check(void *ctx)
{
	OSI_LOGI(0, "show_net_check");
	if((mg_socket_connect_flag==0)&&(CFW_SIM_ABSENT!=MG_cfwGetSimStatus(0)))
	{
		g_net_total_time_ms += NET_CHECK_TIME_MS;
	}else
	{
		g_net_total_time_ms = 0;
	}
	if(g_net_total_time_ms >= NET_RECOVERY_TIME_MS)
	{ 
		g_net_total_time_ms = 0;
		baolan_net_recovery();
	}
}



void baolan_net_check_timer_callback(void *param)
{
	//osiEvent_t Mess_event = {.id =EVENT_TIME_FOR_NET_IND };
	//MG_osiTracePrintf(OSI_LOG_TAG,"notify the receive thread :0x%x");
   	//MG_osiEventSend((osiThread_t *)meig_Net_IND_Handle, &Mess_event);
	/*if((mg_socket_connect_flag==0)&&(CFW_SIM_ABSENT!=MG_cfwGetSimStatus(0)))
	{
		g_net_total_time_ms += NET_CHECK_TIME_MS;
	}else
	{
		g_net_total_time_ms = 0;
	}
	if(g_net_total_time_ms >= NET_RECOVERY_TIME_MS)
	{ 
		g_net_total_time_ms = 0;
		baolan_net_recovery();
	}*/
	//MG_osiTimerStartLast(g_net_check_timer, NET_CHECK_TIME_MS);
	osiThreadCallback(meig_Net_IND_Handle,net_check,NULL);
	
}

void baolan_net_check_timer(void)
{
	//g_net_check_timer= MG_osiTimerCreate(MG_osiThreadCurrent(), baolan_net_check_timer_callback, NULL);	//create timer
	g_net_check_timer= MG_osiTimerCreate(NULL, baolan_net_check_timer_callback, NULL);	//create timer
	if(g_net_check_timer == NULL)
	{
		MG_osiTracePrintf(LOG_TAG, "create timer failed");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "create timer success");
		
		if(MG_osiTimerSetPeriod(g_net_check_timer, NET_CHECK_TIME_MS,true ))		//set periodic=true
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer failed");
		}
		MG_osiTimerStartLast(g_net_check_timer, NET_CHECK_TIME_MS);
	}
}





//add by zx 20200916 
osiTimer_t * g_csq_timer =NULL;

static void csq_cb(void *ctx)
{
	OSI_LOGI(0, "show_csq_value");
	show_csq_value();
}

#define EVENT_FOR_SHOW_CSQ       2
void lcd_csq_timer_callback(void *param)
{
	
	osiThreadCallback(meig_Net_IND_Handle,csq_cb,NULL);
	
}
void lcd_start_timer_show_csq(void)
{
	MG_osiTracePrintf(LOG_TAG, "lcd_start_timer_show_csq");
#if 1
	//g_csq_timer= MG_osiTimerCreate(MG_osiThreadCurrent(), lcd_csq_timer_callback, NULL);	//create timer
	g_csq_timer= MG_osiTimerCreate(NULL, lcd_csq_timer_callback, NULL);	//create timer
	if(g_csq_timer == NULL)
	{
		MG_osiTracePrintf(LOG_TAG, "create timer failed");
	}
	else
	{
		MG_osiTracePrintf(LOG_TAG, "create timer success");
		
		if(MG_osiTimerSetPeriod(g_csq_timer, 2000,true ))		//set periodic=true
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer success");
		}
		else
		{
			MG_osiTracePrintf(LOG_TAG, "set periodic  timer failed");
		}
		MG_osiTimerStartLast(g_csq_timer, 2000);
	}
#endif
}
//end of add by zx 20200916 

void OEM_NetThread(void *param)
{

	baolan_net_check_timer();
	lcd_start_timer_show_csq();
	show_service_state(false);
	MG_registCallback(EV_PS_DOMAIN_REGISTER,Register_Net);//注测网络
	MG_registCallback(EV_PS_DOMAIN_NO_REGISTER,NO_Register_Net);//no 注测网络
	MG_registCallback(EV_PDP_AUTO_ACTIVATION,Auto_pdp_act);//自动激活pdp
	MG_registCallback(EV_PDP_AUTO_DEACTIVATION,Auto_pdp_deact);//自动去激活pdp
	MG_registCallback(EV_PDP_ACTIVE_ACTIVATION,pdp_act);//手动激活pdp
	MG_registCallback(EV_PDP_ACTIVE_DEACTIVATION,pdp_deact);//手动去激活pdp
	
	while(1)
	{
		osiEvent_t event = {0};
		MG_osiEventWait(MG_osiThreadCurrent(), &event);
		switch(event.id)
		{
			case EV_PS_DOMAIN_REGISTER://
				MG_osiTracePrintf(LOG_TAG,"notify the Attach succeed!");
				int_net();
				break;
			case EV_PS_DOMAIN_NO_REGISTER://
				MG_osiTracePrintf(LOG_TAG,"notify DE Attach!");
				//event_to.id=80001;
				//MG_osiEventSend((osiThread_t *)meig_TCP_Handle, &event_to);
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
				//MG_osiEventSend((osiThread_t *)meig_TCP_Handle, &event_to);
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
				//MG_osiEventSend((osiThread_t *)meig_TCP_Handle, &event_to);
				//int_net(name);
				if(mg_socket_connect_flag==1)
				{
					mg_socket_connect_flag=0;
					lwip_close(connfd);
				}
				mg_data_call = 0;
				break;
			case EVENT_TIME_FOR_NET_IND:
				
				break;
			
			default:
			
				break;
			
		}		
	}
}




