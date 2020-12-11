#include "ty_ssl_adpt.h"
#include "ty_sc_adpt.h"
#include "ty_sock_test.h"
//ssl param begin

#define NAME "yfzx.whty.com.cn"
#define PORT 443
char ca_cert[] = "\
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

char *send_buff ="\
    GET / HTTP/1.1\r\nHOST: \
    yfzx.whty.com.cn/dev-iot-rq/services/iot-fota/api/fotaModuleResponse/receive\r\n\r\n";

//https://yfzx.whty.com.cn/dev-iot-rq/services/iot-fota/api/fotaModuleResponse/receive

//char *send_buff =   "GET / HTTP/1.1\r\nConnection: keep-alive\r\nUser-Agent: 8910\r\nHOST: 104.224.176.31:8080\r\n\r\n";

//ssl param end 


void ty_adpt_sslsock_test(void* param)
{
    int fd_ret = -1;
    int *handle = (int*)ty_ssl_new();
    
    char * ipstr = getipstrbyname(NAME);
    LOG("%s: %d handle: %x",__func__, __LINE__, handle);
    fd_ret = ty_ssl_cert_load(handle, ca_cert, strlen(ca_cert) + 1);
    fd_ret = -1;
    fd_ret = ty_ssl_connect(handle, ipstr, PORT);
    if (fd_ret != 0) {
            LOG("MG_SSL_socket_connect fail");
            ty_ssl_close(handle);
            return;
    }
#if 1   
    if(ty_ssl_send(handle, send_buff, strlen(send_buff) + 1, 0) < 0){
        LOG("MG_socket_write fail");
        ty_ssl_close(handle);
    }else{
        LOG("MG_socket_write success");
    }
    
    char tmpbuff[1500] = {0};
    int len = 1500;
    int readlen = 0;
    int prereadLen = len;
    while(readlen < prereadLen ){
        len = prereadLen-readlen;
        int recvLen = ty_ssl_recv(handle, (unsigned char *)tmpbuff+readlen, len, 0);
        if(recvLen <=0)
            break;
        else
            readlen+=recvLen;
    }
    LOG("22222");
    if (readlen <= 0) {
        LOG("MG_socket_read fail");
        ty_ssl_close(handle);
    }

    LOG("[recvdata:%s]\n", tmpbuff);
#endif 

    LOG("connect fd_ret = %d\r\n", fd_ret);
    LOG("%s: %d handle: %x",__func__, __LINE__, handle);
    ty_ssl_close(handle);
    LOG("%s end\r\n",__FUNCTION__);
    
    while(1){
        ty_sleep(5000);
        LOG(".....");
    }
      
}