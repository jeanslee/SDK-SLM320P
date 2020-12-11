#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "socket.h"
#include "osi_log.h"
#include "osi_api.h"
#include "mg_sslsock_api.h"

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'S', 'S')

#define CACERT_PATH "/caCrt.pem"
#define CLICERT_PATH "/clientCrt.pem"
#define CLIPRIVATEKEY_PATH "/privateKey.pem"

mUpnpSocket *g_sock = NULL;
#define IPADDR "104.224.176.31"
#define PORT 4430

char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIICGTCCAYICCQCeyNbbdpgEvzANBgkqhkiG9w0BAQsFADBRMQswCQYDVQQGEwJD\r\n"
    "TjETMBEGA1UECAwKU29tZS1TdGF0ZTEUMBIGA1UECgwLTWVpR2VTZXJ2ZXIxFzAV\r\n"
    "BgNVBAMMDjEwNC4yMjQuMTc2LjMxMB4XDTIwMDcwNjA2NDgzOVoXDTIwMDgwNTA2\r\n"
    "NDgzOVowUTELMAkGA1UEBhMCQ04xEzARBgNVBAgMClNvbWUtU3RhdGUxFDASBgNV\r\n"
    "BAoMC01laUdlU2VydmVyMRcwFQYDVQQDDA4xMDQuMjI0LjE3Ni4zMTCBnzANBgkq\r\n"
    "hkiG9w0BAQEFAAOBjQAwgYkCgYEAvG0ETfLYj2UuINqZTGMsXYN6hARQiGJJGBZx\r\n"
    "MgWbLdfeC6CIScR7q8ietMDKaxL71IsNmcWWMEI62gaF19lpPg60H9o1VrQDGuGk\r\n"
    "YVEVXlYy0CV39IF/wMdIWZHyi7FjzdNhB3AmtfFIiYjfOxd0LGPNlGaMKHEeE9h7\r\n"
    "+OegEmMCAwEAATANBgkqhkiG9w0BAQsFAAOBgQChg6fToeTAbUdCYK8oVdeh2iPH\r\n"
    "sQby01VBoBZv8b4SlXhwRxbsH4Q7iY7MXwqMuG41qhpHYZMBY46RJhJ11A0F+aqe\r\n"
    "izSENqQXCBbqG0ZJf8VIbqm3NvPcYnPZsfvbJgP10TLn5cHFmrmc02TBJ5zlCr0Z\r\n"
    "jPYy06xLccvxNbeFWg==\r\n"
    "-----END CERTIFICATE-----\r\n";

char *buff =
    "GET / HTTP/1.1\r\nConnection: keep-alive\r\nUser-Agent: 8910\r\nHOST: 104.224.176.31:8080\r\n\r\n";


#if 0
int read_cert2SaveSys(int CrtType, char *filename)
{
    uint8_t *crtPem = NULL;
    int crtLen = 0;

    if (MG_ReadCrt_from_file(CrtType, filename, &crtPem, &crtLen) == false) {
        MG_osiTracePrintf(LOG_TAG, "MG_ReadCrt_from_file fail");
        return -1;
    }

    if (MG_saveCrttoSysFile(CrtType, crtPem, crtLen) == false) {
        MG_osiTracePrintf(LOG_TAG, "MG_saveCrttoSysFile fail");
        return -1;
    }

    return 0;
}
#endif
static void prvThreadEntry(void *param)
{
    int CrtType = -1;
    g_sock = MG_socket_ssl_new();

    if (g_sock == NULL) {
        MG_osiTracePrintf(LOG_TAG, "new socket fail");
        return;
    }

    char tmpbuff[1500] = {0};

#if 0
    //ca cert
    CrtType = CG_HTTPS_CA_CRT;

    if (read_cert2SaveSys(CrtType, CACERT_PATH) < 0) {
        MG_osiTracePrintf(LOG_TAG, "read_cert2SaveSys fail");
        return;
    }

    //client ca
    CrtType = CG_HTTPS_CLIENT_CRT;

    if (read_cert2SaveSys(CrtType, CLICERT_PATH) < 0) {
        MG_osiTracePrintf(LOG_TAG, "read_cert2SaveSys fail");
        return;
    }

    //client key
    CrtType = CG_HTTPS_CLIENT_PRIVATE_KEY;

    if (read_cert2SaveSys(CrtType, CLIPRIVATEKEY_PATH) < 0) {
        MG_osiTracePrintf(LOG_TAG, "read_cert2SaveSys fail");
        return;
    }

#endif
    CrtType = CG_HTTPS_CA_CRT;

    if (MG_saveCrttoSysFile(ca_cert, strlen(ca_cert) + 1, CrtType) == 0) {
        MG_osiTracePrintf(LOG_TAG, "MG_saveCrttoSysFile fail");
        return;
    }

    //MG_set_ssl_seclevel(CG_HTTPS_NEED_CERT); //set sec level
    if (MG_socket_ssl_connect(g_sock, IPADDR, PORT) == false) {
        MG_osiTracePrintf(LOG_TAG, "MG_SSL_socket_connect fail");
        MG_socket_ssl_close(g_sock);
    }

    if (MG_socket_ssl_write(g_sock, buff, strlen(buff) + 1) <= 0) {
        MG_osiTracePrintf(LOG_TAG, "MG_socket_write fail");
        MG_socket_ssl_close(g_sock);
    }

    if (MG_socket_ssl_read(g_sock, tmpbuff, 1500) <= 0) {
        MG_osiTracePrintf(LOG_TAG, "MG_socket_write fail");
        MG_socket_ssl_close(g_sock);
    }

    MG_osiTracePrintf(LOG_TAG, "[recvdata:%s]\n", tmpbuff);
}
int  appimg_enter()
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgssl", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}
void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
