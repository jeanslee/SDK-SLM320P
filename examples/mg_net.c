#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "mg_net_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void CellInfo()
{
    //MG_test();
    MG_osiThreadSleep(6000);
    MG_osiTracePrintf(LOG_TAG, "mg NET start");
    
    NeighborCellInfo *nCellinfo = (NeighborCellInfo *)malloc(sizeof(NeighborCellInfo));
    CurrentCellInfo *cCellinfo = (CurrentCellInfo *)malloc(sizeof(CurrentCellInfo));
    bool nRet = 0;
    
    while(1)
    {
        memset(nCellinfo, 0, sizeof(NeighborCellInfo));
        MG_osiTracePrintf(LOG_TAG, "mg----------------------------mg");
        nRet = MG_NwGetNeighborCellInfo(nCellinfo, 10000);
        if (nRet == true)
        {
            MG_osiThreadSleep(1000);
            MG_osiTracePrintf(LOG_TAG, "mg get neighbor cell info success num is %d", nCellinfo->nCellNum);
            for(int i = 0; i < nCellinfo->nCellNum; i++)
            {
                MG_osiTracePrintf(LOG_TAG, "MG %d cid %d | lac %d", i, nCellinfo->cellId[i], nCellinfo->lac[i]);
            }
        }
        else
        {
            MG_osiThreadSleep(1000);
            MG_osiTracePrintf(LOG_TAG, "mg get neighbor cell info false");
        }

        memset(cCellinfo, 0, sizeof(CurrentCellInfo));
        nRet = MG_NwGetCurrentCellInfo(cCellinfo);
        if (nRet == true)
        {
            MG_osiThreadSleep(500);
            MG_osiTracePrintf(LOG_TAG, "mg get current cell info success");
            
            MG_osiThreadSleep(500);
            MG_osiTracePrintf(LOG_TAG, "mg cid %d, lac %d, mnc %d%d%d, mcc %d%d%d, rsrp %d, rsrq %d",
                                        cCellinfo->cid,
                                        cCellinfo->lac,
                                        cCellinfo->mnc[0],cCellinfo->mnc[1],cCellinfo->mnc[2],
                                        cCellinfo->mcc[0],cCellinfo->mcc[1],cCellinfo->mcc[2],
                                        cCellinfo->rsrp,
                                        cCellinfo->rsrq);
        }
        else
        {
            MG_osiThreadSleep(1000);
            MG_osiTracePrintf(LOG_TAG, "mg get current cell info false");
        }

        MG_osiThreadSleep(6000);    
    }
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg thread exit");
    MG_osiThreadExit();
}

static void prvThreadEntry(void *param)
{
    MG_osiThreadSleep(40000);
    
    uint8_t nsim = 0;
    char *plmn = (char *)malloc(sizeof(char));
    
    MG_osiTracePrintf(LOG_TAG, "-----mg lbs-----");
    MG_getPlmn(0, plmn);
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg get plmn: %d%d%d %d%d", *plmn, *(plmn + 1), *(plmn + 2), *(plmn + 3), *(plmn + 4));
    
    uint8_t *rssi = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *ber = (uint8_t *)malloc(sizeof(uint8_t));
    
    if(MG_getCsq(0, rssi, ber) == 0)
    {               
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg get csq rssi: %d, ber: %d", *rssi, *ber); 
    }    
    else
    {
        MG_osiThreadSleep(1000);
        MG_osiTracePrintf(LOG_TAG, "mg get csq failed");
    }
    
    osiEvent_t event = {1};
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg event id: %d", event.id);
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg event name: %s", MG_cfwEventName(event.id));
    
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg get net rat: %d", MG_cfwNWGetRat(0));
    
    uint8_t p_state1 = 0;
    uint8_t p_state2 = 0;
    
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg get gprs att: %d, state: %d", MG_cfwGetGprsAttState(&p_state1, 0), p_state1);
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg get gprs act: %d, state: %d", MG_cfwGetGprsActState(&p_state2, 0), p_state2);
    
    char ip4addr[80] = {0};
    MG_getLocalAddr(0, ip4addr);
    MG_osiTracePrintf(LOG_TAG, "mg get ip4 addr %s", ip4addr); 
    
    free(plmn);
    free(rssi);
    free(ber);
    MG_osiThreadSleep(1000);
    MG_osiTracePrintf(LOG_TAG, "mg plmn end");
    
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiTracePrintf(LOG_TAG, "mg----------------mg");
    //MG_osiThreadCreate("mg_lbs", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);	
    MG_osiThreadCreate("mg_lbs", CellInfo, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg_lbs exit");
}
