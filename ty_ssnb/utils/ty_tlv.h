#ifndef _TY_TLV_H_
#define _TY_TLV_H_

#include"stdio.h"
#include"stdlib.h"
#include"hex_util.h"
#include"ty_os_adpt.h"
#include "ty_sc_adpt.h"

int toTLVByTag(UCHAR* tag, int tagLen, UCHAR*data, int dataLen, UCHAR** dest);
int toTLV(UCHAR*data, int len, UCHAR** dest);

int indexOfByTag(UCHAR* data,int dataLen,UCHAR* tag,int tagLen,int offset);
int determineTLV(UCHAR* data,int dataLen,UCHAR* beginTag,int beginLen,UCHAR* endTag,int endLen,UCHAR** dest);
#endif