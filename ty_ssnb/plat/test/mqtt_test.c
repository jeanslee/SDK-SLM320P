#include "ty_mqtt_adpt.h"
#include "ty_sc_adpt.h"
#include "ty_ssl_adpt.h"
#include "ty_os_adpt.h"
//mqtt param

#define MQTT_ADDR "yfzx.whty.com.cn"

#define MQTT_TLS_PORT 3710
#define MQTT_PORT 3709

#define USE_SSL 1


char* topic1 = "iot/61141560afa5426b822139563e0dc724/000000000000000/user/cloudmsg/down";
char* topic2 = "iot/61141560afa5426b822139563e0dc724/000000000000000/user/cloudmsg/report_reply";
char* ClientID= "61141560afa5426b822139563e0dc724.000000000000000|_ss=1,_v=sdk-c-4.0.0,securemode=3,signmethod=hmacsha256,ext=3,|";
char* username = "000000000000000&61141560afa5426b822139563e0dc724";
char* password = "C5C0571DAF2CD94C9B914E1CAFCFABCCB0076B6A19C73F1CC6C6B0EE018E47E8" ;


const char *ty_ca_cert1 = \
{
 "-----BEGIN CERTIFICATE-----\r\n"
 "MIIC/zCCAeegAwIBAgIJANgv9VIfdmV3MA0GCSqGSIb3DQEBCwUAMBYxFDASBgNV\r\n" \
 "BAMMC3d3dy5lbXF4LmlvMB4XDTIwMDgwNzA5NTI0NVoXDTMwMDgwNTA5NTI0NVow\r\n" \
 "FjEUMBIGA1UEAwwLd3d3LmVtcXguaW8wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAw\r\n" \
 "ggEKAoIBAQDM/y0CrQvRd74nD0bsV15y62YAons9uWHH4s9tHUlS4ufZchxT+0zo\r\n" \
 "ny+cfMtZYsSpH3NajMH+zuRn8xuN3B3p6ifJcoKSH5Z8k1uQRS3q01Z44o/FCSX1\r\n" \
 "hRGtwMsNptilNFpZc9KQZsj2mHyRt4fHIFf5UFCSn0bM1+OfbaSu33zkSDMj/8xD\r\n" \
 "vYE4bz5DtXNH73oY5PKvvL88saOWpt7eH1WRdPBtU9+K48r0yvKZOX2+O3B0KAXJ\r\n" \
 "njnQ8TEatYdznaCKUtAskvEvuDp+HwbdjFUxzctsfD4falWaRAy6e3lDHIvo2NFi\r\n" \
 "MIIY+AOnxiBFzc9ZMZ8xfMLV14rQNyE/AgMBAAGjUDBOMB0GA1UdDgQWBBT4YM/H\r\n" \
 "43V9pNy50pnR2geKHf8mHDAfBgNVHSMEGDAWgBT4YM/H43V9pNy50pnR2geKHf8m\r\n" \
 "HDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQB5dfa1+8xa8QXdkl/E\r\n" \
 "M/dVnxT1GGBwC77xx5Dj/7moNTOC0fAhvBvgswVxdn6Qo46vIQMTZFtLgIe7CqW0\r\n" \
 "IzpVTjX+dAH7ZuLFW1JojZYPmx67yjtDb85AQW1P/ZbYG/So5A/6kMtBGOdwGaf+\r\n" \
 "3h9fdNiLnHWDQyXaFEfruS96r4NjJ3I03MrHAfrRaO/sbggayv1M6QF4cjEiUYL+\r\n" \
 "N80o8xkJWpiJKWwm0LSiRaSd9kUP/BxQzko5zbb3lP41lXgMQM5WKMdA/vPSLmRS\r\n" \
 "Rm8vHpoQav8TqKr3EsoJjOHzs/v3gLKqG3/SXoMcqms1hhpTt7ZSNGeHdqGbfPbu\r\n" \
 "pSp2\r\n" \
 "-----END CERTIFICATE-----"
};
extern void ty_adpt_mqtt_test(void);
extern void mqtt_recv_submsg(int msgID, const char * topic, int topicLen, int msgLen, const char * message);

void mqtt_recv_submsg(int msgID, const char * topic, int topicLen, int msgLen, const char * message)
{
    LOG("%s:%d\r\n",__func__, __LINE__);  
    LOG("Message arrived on topic %.*s: %.*s\r\n", topicLen, topic, msgLen, message);
}
void ty_adpt_mqtt_test(void)
{    
	const char* sendmsg = "hello mqtt test";
    int ret = -1;
    int *mqtt_handle = (int*)lpa_malloc(sizeof(int));

	if(mqtt_handle == NULL)
	{
		LOG("%s:%d malloc failed!", __func__, __LINE__);
	}
    ty_mqtt_cnt_info* mqtt_info = (ty_mqtt_cnt_info*)lpa_malloc(sizeof(ty_mqtt_cnt_info));
	if(mqtt_info == NULL)
	{
		LOG("%s:%d malloc failed!", __func__, __LINE__);
	}
    mqtt_info->url = (const char*)lpa_malloc(strlen(MQTT_ADDR)+1);
	if(mqtt_info->url == NULL)
	{
		LOG("%s:%d malloc failed!", __func__, __LINE__);
	}
    strncpy(mqtt_info->url, MQTT_ADDR,strlen(MQTT_ADDR)+1);
//    LOG("%s:%d :mqtt server: %s, port: %d", __func__, __LINE__, TY_CLOUD_MQTT_SERVER, TY_CLOUD_PORT);
//    strncpy(mqtt_info->url, TY_CLOUD_MQTT_SERVER,strlen(TY_CLOUD_MQTT_SERVER)+1);

    mqtt_info->port = MQTT_PORT;
#if defined(USE_SSL)
     mqtt_info->port = MQTT_TLS_PORT;
//   mqtt_info->port =  TY_CLOUD_PORT;
#else
    mqtt_info->port = MQTT_PORT;
#endif    
    
    //mqtt_info->user = (const char*)lpa_malloc(strlen(username)+1);
    strncpy(mqtt_info->user, username, strlen(username)+1);
    
    //mqtt_info->passwd = (const char*)lpa_malloc(strlen(password)+1);
    strncpy(mqtt_info->passwd, password, strlen(password)+1);
    
    //mqtt_info->ClientID = (const char*)lpa_malloc(strlen(ClientID)+1);
    strncpy(mqtt_info->ClientID, ClientID, strlen(ClientID)+1);
    mqtt_info->keepalive = 30;
    mqtt_info->CleanSession = 0;
    mqtt_info->usetls = 0;
#if defined(USE_SSL)   
    mqtt_info->usetls = 1;
    mqtt_info->tls_ca.ca_cert= (const char*)lpa_malloc(strlen(ty_ca_cert1)+1);
	if(mqtt_info->tls_ca.ca_cert == NULL)
	{
		LOG("%s:%d malloc failed!", __func__, __LINE__);
	}
	
    strncpy(mqtt_info->tls_ca.ca_cert, ty_ca_cert1, strlen(ty_ca_cert1)+1);
    
    mqtt_info->tls_ca.cert_len = strlen(ty_ca_cert1) +1;
#endif    

    ret = ty_mqtt_connect(mqtt_info, mqtt_recv_submsg, &mqtt_handle);
    LOG("%s:%d connect ret = %d\r\n",__func__, __LINE__, ret);
    ret = ty_mqtt_status(mqtt_handle);
    LOG("%s:%d status ret = %d\r\n",__func__, __LINE__, ret);
    
#if 1
    ret = ty_mqtt_sub(mqtt_handle, topic2, 2);
    LOG("%s:%d subscribe ret = %d\r\n",__func__, __LINE__, ret);
    
    ret = ty_mqtt_pub(mqtt_handle, topic2, 0, 0, sendmsg);
    ret = ty_mqtt_pub(mqtt_handle, topic2, 1, 0, sendmsg);
    ret = ty_mqtt_pub(mqtt_handle, topic2, 2, 0, sendmsg);
    LOG("%s:%d publish ret = %d\r\n",__func__, __LINE__, ret);


    ty_sleep(1000);
    
    ret = ty_mqtt_unsub(mqtt_handle, topic2);
    LOG("%s:%d unsubscribe ret = %d\r\n",__func__, __LINE__, ret);

   ty_mqtt_close(&mqtt_handle);
    ret = ty_mqtt_status(mqtt_handle);
    LOG("%s:%d status ret = %d\r\n",__func__, __LINE__, ret);
    
    //just make sure we can connect again;
    ret = ty_mqtt_connect(mqtt_info, mqtt_recv_submsg, &mqtt_handle);
    LOG("%s:%d connect ret = %d\r\n",__func__, __LINE__, ret);

    ret = ty_mqtt_status(mqtt_handle);
    LOG("%s:%d status ret = %d\r\n",__func__, __LINE__, ret);
#if 0
  ty_mqtt_close(&mqtt_handle);

#if defined(USE_SSL)   
    lpa_free(mqtt_info->tls_ca.ca_cert);
#endif     

    lpa_free(mqtt_info->url);
    lpa_free(mqtt_info);
#endif     
#endif 
    while(1)
        {

            ret  = ty_mqtt_yield(mqtt_handle, 1000);
            if(ret == -1){

                    ret = ty_mqtt_status(mqtt_handle);
                    LOG("%s:%d status ret = %d\r\n",__func__, __LINE__, ret);
            }
#if defined(USE_SSL)
            LOG("%s:use SSL!\r\n", __func__);
#else
            LOG("%s:don't use SSL\r\n", __func__);
#endif    
            ty_sleep(10000);
        }
}