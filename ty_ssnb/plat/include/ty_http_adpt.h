#ifndef __TY_HTTP_ADPT_H__
#define __TY_HTTP_ADPT_H__

#include "stdint.h"
#include "stdbool.h"

typedef void* ty_http_handle_t;

typedef struct 
{
	char *host;
    const char *cert;
    bool keepalvie;
}ty_http_cfg;


//http  ≈‰Ω”ø⁄
ty_http_handle_t ty_http_setup(ty_http_cfg *cfg);

char *ty_http_post_request(ty_http_handle_t  handle,const char* uri,char* params, int timeout_ms);

int ty_http_get_request(ty_http_handle_t  handle,const char* uri,uint8_t **response_content,uint32_t* totalLen, uint32_t* responseLen, 
    uint32_t startRange, uint32_t endRange,int timeout_ms);
    
void ty_http_close(ty_http_handle_t handle);


#endif


