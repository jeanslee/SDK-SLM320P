#ifndef _TY_UTIL_H_
#define _TY_UTIL_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include"stdio.h"
#include"stdlib.h"
#include"ty_os_adpt.h"
#include <ctype.h>
#include"hex_util.h"

typedef struct {
    char **str;     //the PChar of string array
    size_t num;     //the number of string
}IString;
 
typedef struct 
{
    char* data;
}TString;

typedef struct 
{
    int size;
    TString* mArray;
}TArrayList;

#define MAX_SIGN_DATALEN 1024*2

int split(char* src,char* separator ,TArrayList* apduList);
int numRandom(int len, char* dest);
int randomToByte(int len, UCHAR* dest);
char* parseIccid(char* iccid);
void getTimeStamp(char* nowTime);

int sort(TArrayList* mList);
int getSort(TArrayList* mList , char* key,char** dest);
int enctypt_md5(unsigned char* data,char** dest);

int addStringList(char* data,TArrayList* mList);
void freeStringList(TArrayList* list);

int c2i(char ch);
int hex2dec(char *hex);
int indexOf(char *str1,char *str2);
int lastIndexOf(char *str1,char *str2);
void substring(char *dest,char *src,int start,int end);


#if defined(__cplusplus)
}
#endif

#endif