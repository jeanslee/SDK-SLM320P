/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 *All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_WATCHDOG_H_
#define _DRV_WATCHDOG_H_

#include <stdbool.h>

bool MG_drvWatchDogStart(float dutyCycle, float freq);

void MG_drvWatchDogStop(void);

#endif/*__DRV_WATCHDOG_H__*/
