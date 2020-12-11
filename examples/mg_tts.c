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
#include "tts_player.h"
#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CODEPAGE_UTF8 65001

void _ttsFinishCB(void)
{
    //to do something
    MG_osiTracePrintf(LOG_TAG, "TTS:doing something here\n");
}

static void prvThreadEntry(void *param)
{
    char *str = "hello world";
    int len = strlen(str);
    OSI_LOGI(0, "tts string len = %d", len);
    MG_osiThreadSleep(1000);
    if (MG_ttsIsPlaying())
    {
        OSI_LOGI(0, "tts is busy");
        MG_osiThreadExit();
    }
    if (!MG_ttsPlayerInit())
    {
        OSI_LOGI(0, "tts init fail");
	MG_osiThreadExit();
    }
    //MG_ttsParamGet(1, 2, CODEPAGE_UTF8);
    if (!MG_ttsPlayText(str, len, _ttsFinishCB, NULL))
    {
    	OSI_LOGI(0, "tts play fail");
	MG_osiThreadExit();
    }

    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-tts", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}
