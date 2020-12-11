#include "ty_at_adpt.h"
#include "softap_api.h"

int at_sim_reset()
{
    if (at_ReqAndRsp_to_ps("AT+CFUN=0\r\n",NULL,NULL,20)!=0)
	{
		LOG("imei error!!!");
	}
    LOG("sim power off OK");
    if (at_ReqAndRsp_to_ps("AT+CFUN=1\r\n",NULL,NULL,20)!=0)
	{
		LOG("imei error!!!");
	}
    LOG("sim power on OK");
    return 0;
}

int at_imei(char *IMEI)
{
    #if 1
	void *p1[] = {IMEI};
	if (at_ReqAndRsp_to_ps("AT+CGSN=1\r\n","+CGSN:%s",p1,5)!=0)
	{
		LOG("imei error!!!");
	}
    #else
    strncpy(IMEI,"868020040149568",strlen("868020040149568"));
    #endif
    return 0;
}
/*
+CEREG:0,1

OK

*/
int at_creg()
{
    int n,status;
	void *p1[] = {(void *)&n,(void *)&status};
	if (at_ReqAndRsp_to_ps("AT+CEREG?\r\n","+CEREG::%d,%d",p1,5)!=0)
	{
		LOG("imei error!!!");
	}
    return status;
}


int ty_send_rsp_str_to_ext (char *rsp)
{
     send_rsp_str_to_ext(rsp);
     return 0;
}

int ty_get_radio_signal_strength()
{
    return xy_getRadioSignalStrength();
}


void ty_log_uart_set_baud (int baud)
{
    static bool setb = false;
    if(!setb)
        at_ReqAndRsp_to_ps("AT+UARTSET=115200,0\r\n",NULL,NULL,5);
    setb = true;
}

void ty_register_app_at_req (char * at_prefix, ty_ser_req_func func)
{
    register_app_at_req (at_prefix,  func);
}

