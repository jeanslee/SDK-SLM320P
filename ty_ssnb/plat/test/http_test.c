#include "ty_http_adpt.h"
#include "ty_sc_adpt.h"
#include "ty_ssl_adpt.h"
#include "ty_os_adpt.h"
#include "ty_sock_test.h"
//http param

#define MQTT_ADDR "yfzx.whty.com.cn"
#define PORT 443


//const char *y_http_ca_cert = NULL;

const char *y_http_ca_cert = "\
-----BEGIN CERTIFICATE-----\r\n\
MIIEqjCCA5KgAwIBAgIQAnmsRYvBskWr+YBTzSybsTANBgkqhkiG9w0BAQsFADBh\r\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n\
QTAeFw0xNzExMjcxMjQ2MTBaFw0yNzExMjcxMjQ2MTBaMG4xCzAJBgNVBAYTAlVT\r\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n\
b20xLTArBgNVBAMTJEVuY3J5cHRpb24gRXZlcnl3aGVyZSBEViBUTFMgQ0EgLSBH\r\n\
MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALPeP6wkab41dyQh6mKc\r\n\
oHqt3jRIxW5MDvf9QyiOR7VfFwK656es0UFiIb74N9pRntzF1UgYzDGu3ppZVMdo\r\n\
lbxhm6dWS9OK/lFehKNT0OYI9aqk6F+U7cA6jxSC+iDBPXwdF4rs3KRyp3aQn6pj\r\n\
pp1yr7IB6Y4zv72Ee/PlZ/6rK6InC6WpK0nPVOYR7n9iDuPe1E4IxUMBH/T33+3h\r\n\
yuH3dvfgiWUOUkjdpMbyxX+XNle5uEIiyBsi4IvbcTCh8ruifCIi5mDXkZrnMT8n\r\n\
wfYCV6v6kDdXkbgGRLKsR4pucbJtbKqIkUGxuZI2t7pfewKRc5nWecvDBZf3+p1M\r\n\
pA8CAwEAAaOCAU8wggFLMB0GA1UdDgQWBBRVdE+yck/1YLpQ0dfmUVyaAYca1zAf\r\n\
BgNVHSMEGDAWgBQD3lA1VtFMu2bwo+IbG8OXsj3RVTAOBgNVHQ8BAf8EBAMCAYYw\r\n\
HQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8C\r\n\
AQAwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdp\r\n\
Y2VydC5jb20wQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQu\r\n\
Y29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDBMBgNVHSAERTBDMDcGCWCGSAGG\r\n\
/WwBAjAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BT\r\n\
MAgGBmeBDAECATANBgkqhkiG9w0BAQsFAAOCAQEAK3Gp6/aGq7aBZsxf/oQ+TD/B\r\n\
SwW3AU4ETK+GQf2kFzYZkby5SFrHdPomunx2HBzViUchGoofGgg7gHW0W3MlQAXW\r\n\
M0r5LUvStcr82QDWYNPaUy4taCQmyaJ+VB+6wxHstSigOlSNF2a6vg4rgexixeiV\r\n\
4YSB03Yqp2t3TeZHM9ESfkus74nQyW7pRGezj+TC44xCagCQQOzzNmzEAP2SnCrJ\r\n\
sNE2DpRVMnL8J6xBRdjmOsC3N6cQuKuRXbzByVBjCqAA8t1L0I+9wXJerLPyErjy\r\n\
rMKWaBFLmfK/AHNF4ZihwPGOc7w6UHczBZXH5RFzJNnww+WnKuTPI0HfnVH8lg==\r\n\
-----END CERTIFICATE-----";  

char *http_host = "https://yfzx.whty.com.cn/";
char *post_buf = "https://yfzx.whty.com.cn/dev-iot-rq/services/iot-fota/api/fotaModuleResponse/receive";
char *get_buf = "https://yfzx.whty.com.cn/dev-iot-rq/group1/M00/01/BD/CggoNl9SBACAPdCuAAA2XxiNLU4712.bin?attname=xyDelta.bin";
char *post_uri = "/dev-iot-rq/services/iot-fota/api/fotaModuleResponse/receive";
char *post_param = "{\"task_no\":\"abc\",\"imei\":\"0000000 00000000\",\"err_code\":3}";
char *get_uri = "/dev-iot-rq/group1/M00/01/BD/CggoNl9SBACAPdCuAAA2XxiNLU4712.bin?attname=xyDelta.bin";


#define TMP_BUFFER_SIZE 20000
#define DOWNLOAD_LENTH 1000

void ty_adpt_http_test(void* param)
{
    ty_http_handle_t http_handle = NULL;
    char * rep_cont = NULL;
    uint32_t read_len = 0;
    uint32_t recvLen = 0;
    uint32_t total_len = 0;
    ty_http_cfg * config = lpa_malloc(sizeof(ty_http_cfg));
    if(config == NULL)
        {
            LOG("malloc failed!");
            return;
        }

    if(y_http_ca_cert != NULL){
            config->cert = (const char*)lpa_malloc(strlen(y_http_ca_cert) + 1);
                if(config->cert == NULL)
                {
                    LOG("malloc failed!");
                    return;
                }
            strncpy(config->cert, y_http_ca_cert, strlen(y_http_ca_cert) + 1);
        }else{
            config->cert = NULL;
        }
    config->host = (char*)lpa_malloc(strlen(http_host) + 1);
            if(config->host == NULL)
            {
                LOG("malloc failed!");
                return;
            }
    strncpy(config->host, http_host, strlen(http_host) + 1);
    
    http_handle = ty_http_setup(config);
    ty_http_post_request(http_handle, post_uri, post_param, 1000);
    
    do
        {
        ty_http_get_request(http_handle, get_uri, &rep_cont, &read_len, &total_len, recvLen , recvLen + DOWNLOAD_LENTH - 1, 1000);

        if(read_len < 0 || rep_cont == NULL)
            break;
        else
            recvLen += read_len;
        LOG("rep_cont = %s\r\n", rep_cont);
        
        LOG("rep_len = %d, recvLen = %d, total_len = %d", read_len, recvLen, total_len);
 
        lpa_free(rep_cont);
        rep_cont = NULL;
        
        
    }while(recvLen< total_len);

   ty_http_close(http_handle);

   lpa_free(config->host);
   lpa_free(config->cert);
   lpa_free(config);
    
    while(1)
        {

            LOG("%s\r\n", __func__);

            ty_sleep(10000);
        }
}