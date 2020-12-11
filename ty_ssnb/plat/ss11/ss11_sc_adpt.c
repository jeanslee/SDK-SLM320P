#include "ty_sc_adpt.h"
#include "ty_os_adpt.h"
#include "opencpu_api.h"
/********************************************************************************************/
/**************used for request AT from ext MCU,if use,please connect XinYi FAE**************/
/********************************************************************************************/

#define MAKEWORD(high, low)		(((unsigned short)high << 8) | low)

T_TyApduCtx ctx = {0};


int Ty_ScGetLogicChannelNo( sc_handle handle)
{
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;

    if(ctx == NULL)
        return -1;
    LOG("%s\r\n",__FUNCTION__);
    return ctx->channel;
    
}

sc_handle Ty_ScOpenLogicChannel(char *aid)
{
    
    uint8_t channel_id ;
    if(aid  == NULL)
        return NULL;
    if(ERR_SUCCESS != esim_channel_open(aid, &channel_id))
    {
        opencpu_printf("open esim channel fail AID=[%s]\n",aid);
        return NULL;
    }

    T_TyApduCtx *ctx = lpa_calloc(1,sizeof(T_TyApduCtx));
    if(!ctx)
    {
        esim_channel_close ( channel_id);
        return NULL;
    }
    ctx->channel = channel_id;
    LOG("%s,channel=%d\r\n",__FUNCTION__,ctx->channel);
    memcpy(ctx->AID,aid,strlen(aid));
    return (sc_handle)ctx;
}

int Ty_ScTransimt(sc_handle handle,  char *inData, char *outData,int *ioLen)
{
    uint16_t timerout_ms = 10*1000;
    int ret = ApduUnknown;
    LOG("%s\r\n",__FUNCTION__);
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL)
        return ApduParamErr;
    ret = esim_channel_transmit ( (uint8_t)ctx->channel, inData,
        (uint16_t)strlen(inData), outData, ioLen, timerout_ms);
    ty_printf_hexbuf (outData, strlen(outData));
    return ret;
}

int Ty_ScCloseLogicChannel(sc_handle handle)
{
    int ret = ApduUnknown;  
    LOG("%s\r\n",__FUNCTION__);
    T_TyApduCtx *ctx = (T_TyApduCtx *)handle;
    if(ctx == NULL)
        return ApduParamErr;    
    ret = esim_channel_close ((uint8_t)ctx->channel);
    LOG("%s,channel=%d\r\n",__FUNCTION__,ctx->channel);
    lpa_free(ctx);
    return ret;
}

