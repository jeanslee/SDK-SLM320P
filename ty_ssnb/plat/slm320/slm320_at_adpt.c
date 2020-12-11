#include "ty_at_adpt.h"
#include "sci_types.h"
#include "mg_net_api.h"
#include "cfw.h"
int at_sim_reset()
{
    LOG("%s\r\n",__FUNCTION__);
    return 0;
}
int at_iccid(char *iccid)
{
    
    LOG("%s\r\n",__FUNCTION__);
    uint8 len = 32;
    if(iccid == NULL)
    {
        LOG("%s : param error", __func__);
        return -1;
    }
    if(MG_getSimIccid(0,iccid,&len))//AT+CCID
    {
        LOG("iccid:%s",iccid);
        return 0;
    }else{
        LOG("get iccid err");
        return -1;
    }
}
int at_imei(char *IMEI)
{
    if(IMEI == NULL)
    {
        LOG("%s : param error", __func__);
        return -1;
    }
    LOG("%s\r\n",__FUNCTION__);
//    uint8_t len =strlen(IMEI);
//   if(MG_getSimImei(0,IMEI,&len))//AT+CGSN=1
//        {
//            LOG("imei:%s",IMEI);
//            return 0;
//        }
//    else{
//        LOG("get imei err");
//        return -1;
//        }
//    memcpy(IMEI,"864948041208104",strlen("864948041208104"));
    memcpy(IMEI,"862211043125145",strlen("862211043125145"));
    return 0;
}
/*
+CEREG:0,1

OK

*/
int at_creg()
{
    LOG("%s\r\n",__FUNCTION__);
    CFW_NW_STATUS_INFO sStatus;
    int registerStatus = 0;
    uint32_t uErrCode = MG_CFW_GprsGetstatus(&sStatus, 0);
    if(uErrCode == 0)
    {
        registerStatus = sStatus.nStatus;
    }
    else
    {
        registerStatus = -1;
#if ADPT_DEBUG        
     LOG("register status: %d", registerStatus);
#endif            
    }

    return registerStatus;
}


int ty_send_rsp_str_to_ext (char *rsp)
{
    if(rsp == NULL)
    {
        LOG("%s : param error", __func__);
        return -1;
    }
    LOG("%s\r\n",__FUNCTION__);
    LOG("%s", rsp);
    return 0;
}

int ty_get_radio_signal_strength()
{
    LOG("%s\r\n",__FUNCTION__);
    uint8_t siglevel;
    uint8_t rat = 0, biterr = 0;
    if(MG_NwGetSignalQuality(&rat, &siglevel, &biterr)){
    LOG("have active pdp context.");
    return siglevel;
    }

    return 0;

}


void ty_log_uart_set_baud (int baud)
{
   LOG("%s\r\n",__FUNCTION__);
}

int at_imsi(char *imsi)
{
    int  len = 16;
    if(imsi == NULL)
    {
        LOG("%s : param error", __func__);
        return -1;
    }
    if(MG_getSimImsi(0,imsi,&len)) //AT+CIMI
    {
        LOG("imsi:%s",imsi);
        return 0;
    }
    else
        LOG("get imsi err");
    return -1;
}


//void ty_register_app_at_req (char * at_prefix, ty_ser_req_func func)
//{
//    LOG("%s\r\n",__FUNCTION__);
//}

