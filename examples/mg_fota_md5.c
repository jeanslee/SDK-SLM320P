#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include "./../../components/fs/include/vfs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg update start");

    for(int i=0;i<5;i++)
    {
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg update wait %d",i);
    }
    
    
    char *fota1 = "/pacmd5.pac";

	
	if(!MG_updateFirmwareMd5(fota1))
	{
		MG_osiTracePrintf(LOG_TAG, "mg update failed");
	}

  
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg update end");


 
    int i = 0;
    while(1)
    {
        i++;
        MG_osiThreadSleep(2000);
        MG_osiTracePrintf(LOG_TAG, "mg update success! %d ", i);
        if (i == 30)
        {
               
        }
    }
 
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_fota_old", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 2024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_fota_old exit");
}
