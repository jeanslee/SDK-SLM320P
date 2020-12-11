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
#include "at_engine.h"
#include "drv_led.h"
/***************
parameter
color:

led_type:
	R_LED
	G_LED
	B_LED

current:
*0~63*
min:1.68ma
step: 0.84ma
****************/
static void prvThreadEntry(void *param)
{
	uint8_t current = 5;
	atDevice_t *leddevice = MG_atDeviceLedsCreate(R_LED);
	MG_atDeviceOpen(leddevice);
	MG_atDeviceWrite(leddevice,&current,sizeof(current));
	osiTracePrintf(LOG_TAG, "mg led example enter..........");
    osiThreadExit();
}

int appimg_enter(void *param)
{
    osiTracePrintf(LOG_TAG, "mg example enter");
    osiThreadCreate("mg-gpio", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024,1);
    return 0;
}

void appimg_exit(void)
{
    osiTracePrintf(LOG_TAG, "mg example exit");
}


