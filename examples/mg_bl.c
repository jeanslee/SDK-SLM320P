/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "osi_log.h"
#include "osi_api.h"

static void prvThreadEntry(void *param)
{
    show_text("你好ABC",20,20);
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    bl_init();

     osiThreadCreate("mgbl", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024,4);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
