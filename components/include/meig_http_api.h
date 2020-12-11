#ifndef __MEIG_HTTP_API__
#define __MEIG_HTTP_API__

#include<stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//struct
typedef struct _HttpInfoSet{
    char url [1025];
    char username [255];
    char password [255];
    char HandData [512];
    char SendData [512];
    uint32_t cid;
    uint32_t StatusCode;
    char* RecvData;
    uint32_t recvlen;
} HttpInfoSet;



//function
uint8_t MG_HTTP_GET (HttpInfoSet *HttpPara);
uint8_t MG_HTTP_POST (HttpInfoSet *HttpPara);
uint8_t MG_HTTP_HEAD (HttpInfoSet *HttpPara);
uint8_t MG_HTTP_ParaClear (HttpInfoSet *HttpPara);

#ifdef __cplusplus
}
#endif

#endif //__MEIG_HTTP_API__
