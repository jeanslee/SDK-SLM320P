#ifndef __MG_SSLSOCK_API__
#define __MG_SSLSOCK_API__
#include "mupnp/net/socket.h"
#define CG_HTTPS_NO_CERT MBEDTLS_SSL_VERIFY_NONE
#define CG_HTTPS_CERT MBEDTLS_SSL_VERIFY_REQUIRED
#define CG_HTTPS_VERIFY_OPTIONAL MBEDTLS_SSL_VERIFY_OPTIONAL
#define CG_HTTPS_NEED_CERT CG_HTTPS_VERIFY_OPTIONAL

bool MG_saveCrttoSysFile(char *pemData, uint32_t pemLen, uint32_t pemtype);
bool MG_ReadSysCrt(uint32_t pemtype, char **crtPem, int *crtLen);
mUpnpSocket *MG_socket_ssl_new();
void MG_set_ssl_seclevel(int slevel);
int MG_socket_ssl_connect(mUpnpSocket *sock, const char *addr, int port);
ssize_t MG_socket_ssl_write(mUpnpSocket *sock, const char *cmd, size_t cmdLen);
ssize_t MG_socket_ssl_read(mUpnpSocket *sock, char *buffer, size_t bufferLen);
void MG_socket_ssl_close(mUpnpSocket *sock);
bool MG_ReadCrt_from_file(uint32_t pemtype, char *filename, char **crtPem, int *crtLen);
#endif
