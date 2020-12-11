
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "cJSON.h"
#include "json_util.h"
#include <stdio.h>
#include "ty_os_adpt.h"

#define LOG_TAG "http_util"




bool cjson_item_str(cJSON *root,const char*item,char **value)
{
    cJSON *jobj = NULL;
    if(!root)
        return false;
    if(!cJSON_HasObjectItem(root,item))
    {
        LOG("cJSON_HasObjectItem no item=%s\r\n",item);
        return false;
    }
    jobj = cJSON_GetObjectItem(root, item);
    if(!jobj)
        return false;
    if(cJSON_IsString(jobj))
    {
        LOG("cjson_item=[%s],value=[%s]\r\n", item,jobj->valuestring);
        *value = jobj->valuestring;
    }
    else
    {
        LOG("item %s  not string\r\n",item);
        return false;
    }
    return true;
}

bool cjson_item_int(cJSON *root,const char*item,int* value)
{
    cJSON *jobj = NULL;
    if(!root)
        return false;
    if(!cJSON_HasObjectItem(root,item))
    {
        LOG("cJSON_HasObjectItem no item=%s\r\n",item);
        return false;
    }
    jobj = cJSON_GetObjectItem(root, item);
    if(!jobj)
        return false;
    if(!cJSON_IsNull(jobj) && cJSON_IsNumber(jobj))
    {
        LOG("cjson_item=[%s],value=[%d]\r\n", item,jobj->valueint);
        *value = jobj->valueint;
    }
    else
    {
        LOG("item %s  not int\r\n",item);
        return false;
    }
    return true;
}

bool cjson_item_array(cJSON *root,const char*item,cJSON **array)
{
    cJSON *jobj = NULL;
    if(!root)
        return false;
    if(!cJSON_HasObjectItem(root,item))
    {
        LOG("cJSON_HasObjectItem no item=%s\r\n",item);
        return false;
    }
    jobj = cJSON_GetObjectItem(root, item);
    if(!jobj)
        return false;
    LOG("cjson_item_array root type = %d\r\n",(jobj->type & 0xFF));
    if(cJSON_IsArray(jobj))
    {
        LOG("cjson_item=[%s],value=[%s]\r\n", item,jobj->valuestring);
        *array = jobj;
    }
    
    return false;
}


