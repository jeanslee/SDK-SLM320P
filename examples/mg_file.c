/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "vfs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define EXAMPLE_FILE_NAME "/nvm/test.txt"
static void prvThreadEntry(void *param)
{
    uint8_t buf[64];

    MG_osiThreadSleep(3000);
    int fd = vfs_creat(EXAMPLE_FILE_NAME,664);
    for(uint8_t i=0;i<sizeof(buf);i++){
        buf[i]='a'+i%26;
    }

    MG_osiTracePrintf(LOG_TAG, "write %s",(char*)buf);
    if(fd>0){
        vfs_write(fd, buf, sizeof(buf));
        vfs_close(fd);
    }

    struct stat st = {};
    int file_size = 0 ;
    fd = vfs_open(EXAMPLE_FILE_NAME,O_RDONLY);
    if(fd>0){
        vfs_fstat(fd, &st);
        file_size = st.st_size;

        MG_osiTracePrintf(LOG_TAG, "file size %d",file_size);
        buf[0]='\0';
        int len = vfs_read(fd,buf,sizeof(buf));
        if(len >0){
            MG_osiTracePrintf(LOG_TAG, "read %s",buf);
        }
        vfs_close(fd);
    }

    fd = vfs_open(EXAMPLE_FILE_NAME,O_RDONLY);
    if(fd>0){
        memset(buf,0,sizeof(buf));
        off_t pos = vfs_lseek(fd,file_size/2,O_RDONLY);

        MG_osiTracePrintf(LOG_TAG, "seek pos:%x",pos);
        int len = vfs_read(fd,buf,sizeof(buf));
        MG_osiTracePrintf(LOG_TAG, "read %d",len);
        if(len >0){
            MG_osiTracePrintf(LOG_TAG, "after seek read %s",(char*)buf);
        }
        vfs_close(fd);
    }
    if(vfs_unlink(EXAMPLE_FILE_NAME) == 0)
        MG_osiTracePrintf(LOG_TAG, "delete success");
    else
        MG_osiTracePrintf(LOG_TAG, "fail to del");

    if(vfs_mkdir("/test",664) == 0){
        MG_osiTracePrintf(LOG_TAG, "mkdir success");
    }else{
        MG_osiTracePrintf(LOG_TAG, "mkdir failure");
    }

    MG_osiThreadSleep(1000);
    if(vfs_rmdir("/test") == 0)
        MG_osiTracePrintf(LOG_TAG, "rmdir success");
    else
        MG_osiTracePrintf(LOG_TAG, "rmdir failure");

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgfile", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
