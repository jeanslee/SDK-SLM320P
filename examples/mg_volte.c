#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
//#include "./../../components/cfw/include/cfw.h"
//#include "./../../components/cfw/include/cfw_dispatch.h"
//#include "./../../components/atr/include/at_cfg.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg volte start");
	
	uint8_t sim = 0;
	uint8_t setvolte = 1;
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg volte is : %d", MG_readVOLTE(sim));
	
	MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg set volte");
	MG_setVOLTE(sim, setvolte);
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg after set volte is : %d", MG_readVOLTE(sim));
	
	MG_osiThreadSleep(1000);
	MG_osiTracePrintf(LOG_TAG, "mg volte end");
	
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
	MG_osiTracePrintf(LOG_TAG, "mg example enter");
	MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
	MG_osiThreadCreate("mg_volte", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
	return 0;
}

void appimg_exit(void)
{
	MG_osiTracePrintf(LOG_TAG, "mg_volte exit");
}
