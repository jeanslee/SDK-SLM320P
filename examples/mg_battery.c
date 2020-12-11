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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_chip.h"

static void prvThreadEntry(void *param)
{
	 if(MG_drvChargerIsPresent())
	 {	 	
	 	if(MG_drvVbatIsOT())
			MG_osiTracePrintf(LOG_TAG, "mg battery over temperature.");
		if(MG_drvCheckchargerIsOV())
			MG_osiTracePrintf(LOG_TAG, "mg battery over voltage.");		
		MG_drvChargerEnable();
		MG_osiTracePrintf(LOG_TAG, "mg: charge voltage %d(mV),charging current %d(mA)",
		                                MG_drvChgGetVol(),MG_drvChargerGetChgCurrent());
		MG_osiTracePrintf(LOG_TAG, "mg: battery voltage %d(mV),battery temperture=%d,battery capacity %d",
		                            MG_drvChargerGetBatteryVoltage(),MG_drvGetVbatTemperature(),
		                            MG_drvChargerVoltagePercentum(MG_drvChargerGetBatteryVoltage(),
		                            MG_drvChargerIsPresent(),false));
		MG_drvChargerDisable();			
	 }
	 MG_osiTracePrintf(LOG_TAG, "mg battery example enter..........");
   
   MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgbattery", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
