
#include "ty_http_adpt.h"

#if 1
//http  ≈‰Ω”ø⁄
ty_http_handle_t ty_http_setup(ty_http_cfg *cfg)
{
    return 0;
}

char *ty_http_post_request(ty_http_handle_t  handle,const char* uri,char* params, int timeout_ms)
{
        char *resp = NULL;
        uint32_t totalLen = 0;
        aiot_http_post((void*)handle,uri,params,&resp,&totalLen);
        return resp;
}

int ty_http_get_request(ty_http_handle_t  handle,const char* uri,uint8_t **response_content,uint32_t* totalLen, uint32_t* responseLen, 
    uint32_t startRange, uint32_t endRange,int timeout_ms)
{
    return -1;
}
    
void ty_http_close(ty_http_handle_t handle)
{
    
}
#endif

