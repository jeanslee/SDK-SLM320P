#include "softap_api.h"
#include "ty_sc_adpt.h"
#include "ty_os_adpt.h"

/********************************************************************************************/
/**************used for request AT from ext MCU,if use,please connect XinYi FAE**************/
/********************************************************************************************/

#define MAKEWORD(high, low)		(((unsigned short)high << 8) | low)

struct t_user_sc_apdu_msg {
	int 	_unused;	/** FIFO uses first 4 bytes itself, reserve space */
	int 	msg_id;
    int 	channel_id;//
	int 	size;
	char	*data;
    char	*response;
    int 	resp_size;
    int status;
};
enum USER_SC_APDU_MSG {
	USER_SC_APDU_OPEN_CHANNEL = 0,
    USER_SC_APDU_TRANSMIT = 1,
    USER_SC_APDU_CLOSE_CHANNEL = 2,
        
};

T_TyApduCtx ctx = {0};

int send_2_sc_ctl_msg(struct t_user_sc_apdu_msg *msg) //, char *buff,int len,char *out_buff,int *out_len
{
    return at_lpa_esim_apdu_msg_handle(msg);
}

static char* at_err_test_info(char *msg)
{
    char *buff = lpa_malloc(strlen(msg)+1);
    memcpy(buff,msg,strlen(msg));
    buff[strlen(msg)] = 0;
    return buff;
}

int at_lpa_esim_apdu_msg_handle(struct t_user_sc_apdu_msg *msg)
{
	int ret = 0;
    static char *at_cmd = NULL;
	LOG("%s  %d \r\n",__FUNCTION__,__LINE__);
    if(at_cmd == NULL)
        at_cmd = lpa_malloc(600);
    if(at_cmd == NULL)
    {
        LOG("%s  %d malloc ERROR\r\n",__FUNCTION__,__LINE__);
        return ApduMemErr;
    }
    memset(at_cmd,0,600);
	switch(msg->msg_id)
	{
		case USER_SC_APDU_OPEN_CHANNEL:
			
			//LOG("USER_SC_APDU_OPEN_CHANNEL\n");
            if(msg->data == NULL || !msg->size )
            {
                msg->status = ApduParamErr;
                break;
            }
            snprintf(at_cmd,600,"AT+CCHO=%s\r\n",msg->data);
            void *p1[1];
            p1[0] = &msg->channel_id;
            if ((ret=at_ReqAndRsp_to_ps(at_cmd, "+CCHO:%d",  p1, 10))!=0)
            {
                LOG("+CCHO error=%d,%s\r\n",ret,at_cmd);
                msg->status = ApduTransmitErr;

                break;
            }
            LOG("msg->channel_id=%d\r\n",msg->channel_id);
			break;
        case USER_SC_APDU_TRANSMIT:
                        
            //LOG("USER_SC_APDU_TRANSMIT\n");
            if(msg->data == NULL || !msg->size || msg->response == NULL || !msg->resp_size)
            {
                LOG("+CGLA param error\n");
                msg->status = ApduParamErr;
                break;
            }
            //初始化为0
            msg->resp_size = 0;
            void *p2[2];
            p2[0] = &msg->resp_size;
            p2[1] = msg->response;
            snprintf(at_cmd,600,"AT+CGLA=%d,%d,%s\r\n",msg->channel_id,msg->size,msg->data);
            //+CGLA:204,bf2061....
            if ((ret=at_ReqAndRsp_to_ps(at_cmd, "+CGLA:%d,%s",  p2, 10))!=0)
            {
                LOG("+CGLA  error=%d atLen=%d apduLen=%d,%d\r\n",ret,strlen(at_cmd),strlen(msg->data),msg->size);
                ty_printf_hexbuf (at_cmd,strlen(at_cmd));
                msg->status = ApduTransmitErr;
                break;
            }
            //LOG("msg->resp_size=%d\n",msg->resp_size);
            msg->response[msg->resp_size] = 0;
            //LOG("response=%d\n",strlen(msg->response));
            break;
        case USER_SC_APDU_CLOSE_CHANNEL:
            snprintf(at_cmd,600,"AT+CCHC=%d\r\n",msg->channel_id);
            if ((ret=at_ReqAndRsp_to_ps(at_cmd, NULL,  NULL, 10))!=0)
            {
                LOG("+CCHC  error=%d channel_id=%d\r\n",ret,msg->channel_id);
                msg->status = ApduTransmitErr;
                break;
            }
            break;

		default:
			break;
	}
    //lpa_free(at_cmd);
    //at_cmd = NULL;
    LOG("%s  %d \r\n",__FUNCTION__,__LINE__);
    return 0;
}


int Ty_ScGetLogicChannelNo( sc_handle handle)
{
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;

    if(ctx == NULL)
        return -1;
    return ctx->channel;
    
}

sc_handle Ty_ScOpenLogicChannel(char *aid)
{
    int ret = ApduUnknown;
    struct t_user_sc_apdu_msg msg = {0};
	//msg = lpa_malloc(sizeof(struct t_user_sc_apdu_msg) );
    msg.msg_id = USER_SC_APDU_OPEN_CHANNEL;
	msg.size = strlen(aid);
    msg.data = (char*)aid;
  
    msg.response = NULL;
    msg.resp_size = 0;
    msg.status = 0;
    T_TyApduCtx *ctx = NULL;
    do
    {
        ret = send_2_sc_ctl_msg (&msg);
        if(ret == 0)
        {
            if(msg.status)
            {
                LOG("sc response sem Take fail");
        		ret = msg.status;
                break;

            }
            ctx = lpa_malloc(sizeof(T_TyApduCtx));
            ctx->channel = msg.channel_id;
            ret = 0;
            break;
        }
        LOG("send_2_sc_ctl_msg fail");
    }while(0);
    //lpa_free(msg);
    //msg = NULL;
    return (sc_handle)ctx;
}

int Ty_ScTransimt(sc_handle handle,  char *inData, char *outData,int *ioLen)
{
    int ret = ApduUnknown;
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL)
        return ApduParamErr;
    struct t_user_sc_apdu_msg msg = {0};
    static char *apduData = NULL;
	//msg = lpa_malloc(sizeof(struct t_user_sc_apdu_msg) );
    //if(msg == NULL)
    //{
    //    LOG("%s  %d Malloc Error\r\n",__FUNCTION__,__LINE__);
    //    return ApduMemErr;
    //}
    msg.msg_id = USER_SC_APDU_TRANSMIT;
	msg.size = strlen(inData);
    msg.data = (char*)inData;

    msg.channel_id = ctx->channel;
    if(apduData == NULL)
        apduData = lpa_malloc(2048);
    int apduLen = 2048;
    int offset = 0;
    if(apduData == NULL)
    {
        LOG("%s  %d Malloc Error\r\n",__FUNCTION__,__LINE__);
        //lpa_free(msg);
        //msg = NULL;
        return ApduMemErr;
    }

    do
    {
        memset(apduData,0,apduLen);
        msg.response = apduData;//outData;
        msg.resp_size = apduLen;//*ioLen;
        if(ctx->channel == 0)
        {
            ret = ApduChannelErr;
            break;
        }
        ret = send_2_sc_ctl_msg (&msg);
        if(ret == 0)
        {
            if(msg.status)
            {
                LOG("sc response sem Take fail\r\n");
        		ret =  msg.status;
                break;

            }
            //LOG(apduData);
            //LOG("\n");
            apduLen = msg.resp_size;
            if(apduLen < 4)
            {
                LOG("sc response len error ,apduLen=%d\r\n",apduLen);
                ret =  ApduTransmitErr;
                break;
            }
            //61XX
            if(0 != strncmp(apduData+(apduLen-4),"9000",4))
            {
                //通道关闭
                if(0 == strncmp(apduData+(apduLen-4),"6881",4))
                {
                    ctx->channel = 0;
                    ret = ApduChannelClosed;
                    break;
                }
                if(0 == strncmp(apduData+(apduLen-4),"61",2))
                {
                    if(offset+apduLen-4 > *ioLen)
                    {
                        LOG("sc buffer overflow");
                        ret = ApduMemErr;
                        break;

                    }
                    memcpy(outData+offset,apduData,apduLen-4);
                    offset += (apduLen-4);
                    char autofetch[24] = {0};
                    snprintf(autofetch,sizeof(autofetch),"%02dC00000%s",ctx->channel,apduData+(apduLen-2));//61XX
                    msg.size = strlen(autofetch);
                    msg.data = (char*)autofetch;
                    LOG(autofetch);
                    LOG("\n");
    
                }
                else
                {
                    // LOG("sc transmit fail [%s]",apduData);
                    // ret =  ApduCmdErr;
                    memcpy(outData+offset,apduData,apduLen);
                    offset += apduLen;
                    *ioLen = offset;
                    ret = 0;
                    break;
                }
            }
            else //9000
            {
                memcpy(outData+offset,apduData,apduLen);
                offset += apduLen;
                *ioLen = offset;
                ret = 0;
                break;
            }
        }
        else
        {
            LOG("send_2_sc_ctl_msg fail");
            break;
        }
    }while(1);
    //lpa_free(msg);
    //msg = NULL;
    //lpa_free(apduData);
    //apduData = NULL;
    return ret;
}


int Ty_ScCloseLogicChannel(sc_handle handle)
{
    int ret = ApduUnknown;
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;

    if(ctx == NULL)
        return ApduParamErr;

    struct t_user_sc_apdu_msg msg  = {0};
	//msg = lpa_malloc(sizeof(struct t_user_sc_apdu_msg) );
    msg.msg_id = USER_SC_APDU_CLOSE_CHANNEL;
	msg.size = 0;
    msg.data = NULL;

    msg.channel_id = ctx->channel;
    msg.response = NULL;
    msg.resp_size = 0;
    do
    {
        if(ctx->channel == 0)
        {
            ret = ApduChannelErr;
            break;
        }
        ret = send_2_sc_ctl_msg (&msg);
        if(ret == 0)
        {
            if(msg.status)
            {
                LOG("sc response sem Take fail");
        		ret =  msg.status;
                break;

            }
            ret = 0;
            break;

        }
        LOG("send_2_sc_ctl_msg fail");
    }while(0);
    //lpa_free(msg);
    //msg = NULL;
    lpa_free(ctx);
    
    return ret;
}

//can call by  user_app_init
void Ty_Sc_AdptInit()
{

}
