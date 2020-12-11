#ifndef _FTP_H_
#define _FTP_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief server callback function
 * The size of the file is returned by the function CB
 *
 *
 * \param buf	  	  buffer of data
 * \param buflen	  length of buffer of data
 *
 * \return
 */
typedef int32_t (*MG_read_t)(uint8_t *buf, uint32_t buflen);

/**
 * \brief FTP client login
 *
 *
 * \param url	    FTP server location
 * \param usrname	FTP client user name
 * \param passwd	FTP client user password
 * \return
 *      - 0 on success
 *      - Negative on invalid parameter
 */
int MG_ftpLogin(const char*url,const char*usrname,const char*passwd);


/**
 * \brief FTP client logout
 *
 *
 * \return
 *      - 0 on success
 *      - Negative on invalid parameter
 */
int MG_ftpLogout();
/**
 * \brief get file data on the server
 * The data of the file is returned by the function CB
 *
 *
 * \param remoteFilePath	file location
 * \param cb				file data callback
 *
 *
 * \return
 *      - 0 on success
 *      - Negative on invalid parameter
 */
int MG_ftpGet(const char* remoteFilePath,MG_read_t cb);


/**
 * \brief upload local files to the server
 *
 *
 * \param dstRemoteFilePath	  server files
 * \param srcLocalFilePath	  location file 
 *
 *
 * \return
 *      - 0 on success
 *      - Negative on invalid parameter
 */
int MG_ftpPut(const char* dstRemoteFilePath,const char* srcLocalFilePath);

/**
 * \brief get server file size
 * The size of the file is returned by the function CB
 *
 *
 * \param dstRemoteFilePath	  server files
 *
 *
 * \return
 *      - Return a positive number at the right time
 *      - Negative on invalid parameter
 */
int MG_ftpFileSize(const char* remoteFilePath);


#endif //_FTP_H_

