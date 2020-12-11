#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg update start");

    MG_osiThreadSleep(70000);
    
    char *fota = "update.pack";
    if(!MG_updateFirmware(fota))
    {
        MG_osiTracePrintf(LOG_TAG, "mg update failed");
    }
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg update end");


    char *path1 = "/fota/fota.stage";
    char *path2 = "/fota/fota.pack";
    char *path3 = "/fota/fota.tmp";
    char *path = "/fota";
    int i = 0;
    while(1)
    {
        i++;
        MG_osiThreadSleep(2000);
        MG_osiTracePrintf(LOG_TAG, "mg update success! %d ", i);
        if (i == 30)
        {
                vfs_unlink(path1);
                vfs_unlink(path2);
                vfs_unlink(path3);
                vfs_rmdir(path);
        }
    }
 
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_fota_old", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_fota_old exit");
}
