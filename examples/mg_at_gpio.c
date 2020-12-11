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
#include "drv_gpio.h"


static void prvThreadEntry(void *param)
{
	uint8_t output_low = 0;
	uint8_t output_high = 1;

	atDevice_t *gpiodevice = MG_atDeviceGpioCreate(8,DRV_GPIO_OUTPUT);
	atDeviceOpen(gpiodevice);
	MG_atDeviceWrite(gpiodevice,&output_low,sizeof(output_low));
	MG_osiTracePrintf(LOG_TAG, "mg gpio example enter..........");
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-gpio", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}


