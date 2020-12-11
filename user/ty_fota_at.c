#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http.h"
#include "md5.h"
#include "sha256.h"
#include "ty_fota.h"
#include "ty_ss_cloud.h"
#include "ty_devinfo.h"
#include "ty_os_adpt.h"
#include "ty_fota_adpt.h"


static int fota_at_get_next_int(char **line,int *value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;

    }
    ret = at_tok_nextint(line, value); // 
    if (ret < 0)
    {
        return ret;
    } 

    return 0;
}

static int fota_at_get_next_str(char **line,char **value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;
    }
    ret  = at_tok_nextstr(line, value); // ""
    if (ret < 0)
    {
        return -2;
    }

    return 0;
}


int ty_fata_at(char *at_params )
{
    int ret  ;
    ty_task_fota fota = {0};
    ret =  fota_task_init();
    char *line = at_params;
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    ret = fota_at_get_next_str (&line, &fota.fota_url);
    if(ret)
        return ret;
    fota.sign_type = 1;

    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    ret = fota_at_get_next_str (&line, &fota.sign_value);
    if(ret)
        return ret;

    LOG("%s %d \r\n",__FUNCTION__,__LINE__);

    return fota_http_download (&fota);
    
}
