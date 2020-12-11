#include <stdint.h>
#include <sys/time.h>
#include "ty_mqtt_basic_info.h"
#include "ty_ss_cloud.h"
#include "memleak.h"


ty_http_post_cb_t g_HttpPostCallback = NULL;

void ty_mqtt_RegHttp(ty_http_post_cb_t post) {
	g_HttpPostCallback = post;
}

int32_t ty_uint2str(uint32_t input, char *output, uint8_t *output_len)
{
    uint8_t i = 0, j = 0;
    char temp[10] = {0};

    do {
        temp[i++] = input % 10 + '0';
    } while ((input /= 10) > 0);

    do {
        output[--i] = temp[j++];
    } while (i > 0);

    if (output_len) {
        *output_len = j;
    }

    return 0;
}

int32_t ty_sprintf(char **dest, char *fmt, char *src[], uint8_t count, char *module_name)
{
    char *buffer = NULL, *value = NULL;
    uint8_t idx = 0, percent_idx = 0;
    uint32_t buffer_len = 0;
    buffer_len += strlen(fmt) - 2 * count;
    for (percent_idx = 0; percent_idx < count; percent_idx++) {
        value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
        buffer_len += strlen(value);
    }

    buffer = lpa_malloc(buffer_len + 1);
    if (buffer == NULL) {
        return -1;
    }
    memset(buffer, 0, buffer_len + 1);

    for (idx = 0, percent_idx = 0; idx < strlen(fmt);) {
        if (fmt[idx] == '%' && fmt[idx + 1] == 's') {
            value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
            memcpy(buffer + strlen(buffer), value, strlen(value));
            percent_idx++;
            idx += 2;
        } else {
            buffer[strlen(buffer)] = fmt[idx++];
        }
    }

	*dest = buffer;
    return 0;
}


void ty_sysdep_rand(uint8_t *output, uint32_t output_len)
{
    uint32_t idx = 0, bytes = 0, rand_num = 0;
    struct timeval time;

    memset(&time, 0, sizeof(struct timeval));
    gettimeofday(&time, NULL);

    srand((unsigned int)(time.tv_sec * 1000 + time.tv_usec / 1000) + rand());

    for (idx = 0; idx < output_len;) {
        if (output_len - idx < 4) {
            bytes = output_len - idx;
        } else {
            bytes = 4;
        }
        rand_num = rand();
        while (bytes-- > 0) {
            output[idx++] = (uint8_t)(rand_num >> bytes * 8);
        }
    }
}

int ty_mqtt_device_reg(ty_mqtt_dev_reg_info *dev, char deviceSecret[64]){
    int ret;
    uint32_t randomNum = 0;
    char random[11] = {0};
    char signStr[65] = {0};
    char *content = NULL;
    cJSON *json = NULL;
    char *content_fmt = "productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=hmacsha256";

	if (NULL == dev || NULL == dev->host || NULL == dev->uri || NULL == dev->productKey
		|| NULL == dev->deviceName || NULL == dev->productSecret || NULL == dev->cert){
		return -1;
	}
	LOG("%s :%d", __func__, __LINE__);
    char *content_src[] = {dev->productKey, dev->deviceName, (char *)random, signStr};
	
	ty_sysdep_rand((uint8_t*)&randomNum, 4);
	ty_uint2str(randomNum, random, NULL);
LOG("%s :%d", __func__, __LINE__);
	int res = _dynreg_sign(dev->productKey, dev->deviceName, dev->productSecret, (char *)random, signStr);
LOG("%s :%d", __func__, __LINE__);
	res = ty_sprintf(&content, content_fmt, content_src, sizeof(content_src) / sizeof(char *),
                           "MQTT");
	//printf("ty_iot_http_dyn_reg content is %s\n", content);
    //ty_printf_hexbuf (content, strlen(content));
	
	//发送内容为contet的https请求
	//char *rsp = http_post(dev->host, dev->uri, content, dev->cert);
	LOG("%s :%d", __func__, __LINE__);
	do
    {
    	if(g_HttpPostCallback == NULL)
        {
            ret = -1;
            LOG("%s :%d", __func__, __LINE__);
            break;
        }   
    LOG("%s :%d", __func__, __LINE__);
    	char *rsp = g_HttpPostCallback(dev->cert,dev->cert_len, dev->host, dev->uri, content);
    	if (rsp != NULL){
            
    		cJSON *json_value;
    		json = cJSON_Parse(rsp);
    		lpa_free(rsp);
    		if (NULL == json){
                ret = -2;
                LOG("%s :%d", __func__, __LINE__);
                break;

    		}
    		LOG("%s :%d", __func__, __LINE__);
    		json_value = cJSON_GetObjectItem(json, "code");
    		if (NULL == json_value){
                LOG("%s :%d", __func__, __LINE__);
                ret = -3;
                break;

    		}
    		LOG("%s :%d", __func__, __LINE__);
    		if (json_value->type == cJSON_Number && 200 == json_value->valueint){
    			cJSON *data = cJSON_GetObjectItem(json, "data");
    			if (NULL == data){
    				
                    ret = -4;
                    LOG("%s :%d", __func__, __LINE__);
                    break;

    			}
    			LOG("%s :%d", __func__, __LINE__);
    			if (data->type == cJSON_Object){
    				cJSON *jsonDs = cJSON_GetObjectItem(data, "deviceSecret");
    				if (NULL == jsonDs){
                        ret = -5;
                        LOG("%s :%d", __func__, __LINE__);
                        break;

    				}
    				LOG("%s :%d", __func__, __LINE__);
    				if (jsonDs->type == cJSON_String){
    					memcpy(deviceSecret, jsonDs->valuestring, strlen(jsonDs->valuestring));
    					ret = 0;
                        LOG("%s :%d", __func__, __LINE__);
                        break;
    				}
    			}
    		}
    	}
    }while(0);
    if(json)
        cJSON_Delete(json);
    if(content)
        lpa_free(content);
    LOG("%s :%d ret = %d", __func__, __LINE__, ret);
	return ret;
}
