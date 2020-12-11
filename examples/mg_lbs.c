#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
//#include "./../../components/cfw/include/cfw.h"
//#include "./../../components/atr/include/at_cfg.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{
    MG_osiThreadSleep(40000);
    
    uint8_t nsim = 0;
    uint8_t *mnc = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *mcc = (uint8_t *)malloc(sizeof(uint8_t));
    uint16_t *tac = (uint16_t *)malloc(sizeof(uint16_t));
    uint32_t *cid = (uint32_t *)malloc(sizeof(uint32_t));
    
    MG_osiTracePrintf(LOG_TAG, "-----mg lbs-----");
    if(MG_getLBS(nsim, mnc, mcc, tac, cid))
    {               
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg lbs mnc %x%x%x", *mnc, *(mnc + 1), *(mnc + 1));
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg lbs mcc %x%x", *mcc, *(mcc + 1));
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg lbs tac %d", *tac);
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg lbs cid %d", *cid);
    }
    else
    {
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg getlbs failed");
    }
    
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg lbs end");
    
    free(mcc);
    free(mnc);
    free(tac);
    free(cid);
    MG_osiThreadExit();
    
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
    MG_osiThreadCreate("mg_lbs", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg_lbs exit");
}
