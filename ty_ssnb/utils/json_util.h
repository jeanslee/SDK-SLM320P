#ifndef _JSON_UTILS_H_
#define _JSON_UTILS_H_

#include "cJSON.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

bool cjson_item_int(cJSON *root,const char*item,int *value);
bool cjson_item_str(cJSON *root,const char*item,char **value);
bool cjson_item_array(cJSON *root,const char*item,cJSON **array);

#ifdef __cplusplus
}
#endif

#endif


