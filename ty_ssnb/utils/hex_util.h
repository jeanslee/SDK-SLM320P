#ifndef _HEX_UTIL_H_
#define _HEX_UTIL_H_

#include <string.h>
#include "ty_os_adpt.h"

typedef  unsigned char      UCHAR;
#ifdef __cplusplus
extern "C"{
#endif



int asc2hex(uint8_t *dest, const char *src, int srclen);
int hex2asc(uint8_t *src,int srcbytes, char *dest,int destlen);

void intToChar(int data, UCHAR* buf);
void uint16ToChar( char* buf,uint16_t data );
uint16_t charToUint16(char* buf);

unsigned int   charToUint32(char* buf);
void uint32ToChar( char* buf,unsigned int data);

char* byteToStr(UCHAR* src, int len);
int   byte2int(UCHAR* src , int len);

int  strToByte(char* src, UCHAR** dest);

#ifdef __cplusplus
}
#endif

#endif

