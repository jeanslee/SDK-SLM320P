#ifndef __TY_SC_ADPT_H__
#define __TY_SC_ADPT_H__

#include "stdint.h"

typedef void*  sc_handle;

typedef sc_handle (*ty_sc_open_channel_cb_t)(char * aid);

typedef int (*ty_sc_transmit_cb_t)(sc_handle handle, char *inData, char *outData,int *ioLen);

typedef void (*ty_sc_close_channel_cb_t)(sc_handle handle);



#define LPA_AID_LEN_MAX 64
typedef struct
{
    int channel;
    char AID[LPA_AID_LEN_MAX];
}T_TyApduCtx;

typedef enum{
    ApduChannelErr = -1,
    ApduChannelClosed,
    ApduMemErr,
    ApduParamErr,
    ApduTransmitErr,
    ApduCmdErr,
    ApduTimeout,
    ApduUnknown,
}apdu_result_t;

sc_handle Ty_ScOpenLogicChannel( char *aid);
int Ty_ScGetLogicChannelNo( sc_handle hande);
int Ty_ScTransimt(sc_handle handle,  char *inData, char *outData,int *ioLen);
int Ty_ScTransimtAutoFetch(sc_handle handle, char *inData, char **outData);
int Ty_ScCloseLogicChannel(sc_handle handle);


#endif
