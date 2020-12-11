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
#include "drv_keypad.h"
#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
 * @brief set key event handler
 *
 * @param key       the key indicator
 * @param evt       key current state
 * @param p         caller context
 */
void _keypadCB(keyMap_t key, keyState_t evt, void *p)
{
    MG_osiTracePrintf(LOG_TAG, "keyid:%d, key_state:%d KEYPAD:doing something here", key, evt);
}

static void prvThreadEntry(void *param)
{
    int ret;
    uint32_t mask = KEY_STATE_PRESS | KEY_STATE_RELEASE;

    MG_drvKeypadInit();
    MG_drvKeypadSetCB(_keypadCB, mask, NULL);

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-keypad", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
