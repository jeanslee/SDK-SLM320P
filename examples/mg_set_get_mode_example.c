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

static void prvThreadEntry(void *param)
{
	MG_osiThreadSleep(3000);
//get flight mode test
	// uint8_t nSim = 0;
	// uint8_t mode;
	// while(1)
	// {
		// MG_osiThreadSleep(2000);
		// mode = MG_GetFlightMode(nSim);
		// MG_osiTracePrintf(LOG_TAG, "The flight mode is %d",mode);
	// }
	
//set flight mode test
	uint8_t nSim = 0;
	uint8_t fun  = 0;	
	if(MG_SetFlightMode(nSim,fun))
	{
		MG_osiThreadSleep(2000);
		MG_osiTracePrintf(LOG_TAG, "The flight mode set succeed");	
	}
	else
	{
		MG_osiThreadSleep(2000);
		MG_osiTracePrintf(LOG_TAG, "The flight mode set failed");	
	}

	MG_osiThreadSleep(2000);
	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg set_get_flight_mode example enter");
    MG_osiThreadCreate("mgflightmode", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg set_get_flight_mode example exit");
}
