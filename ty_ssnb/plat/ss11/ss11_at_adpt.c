#include "ty_at_adpt.h"


int at_sim_reset()
{
    LOG("%s\r\n",__FUNCTION__);
    return 0;
}

int at_imei(char *IMEI)
{
    LOG("%s\r\n",__FUNCTION__);
    opencpu_get_imei (IMEI);
    //memcpy(IMEI,"864948041208104",strlen("864948041208104"));
    return 0;
}
/*
+CEREG:0,1

OK

*/
int at_creg()
{
    LOG("%s\r\n",__FUNCTION__);
    return 1;
}


int ty_send_rsp_str_to_ext (char *rsp)
{
     LOG("%s\r\n",__FUNCTION__);
     return 0;
}

int ty_get_radio_signal_strength()
{
    LOG("%s\r\n",__FUNCTION__);
    uint8 rssi;
    uint8 rxqual;
    opencpu_csq(&rssi,&rxqual);
    return (int)rssi;
}


void ty_log_uart_set_baud (int baud)
{
   LOG("%s\r\n",__FUNCTION__);
}

void ty_register_app_at_req (char * at_prefix, ty_ser_req_func func)
{
    LOG("%s\r\n",__FUNCTION__);
}

