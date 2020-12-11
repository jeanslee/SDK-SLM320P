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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('A', 'P', 'L', 'Y')

#include "osi_api.h"
#include "osi_log.h"
#include "osi_pipe.h"
#include "audio_player.h"
#include "audio_recorder.h"
#include "audio_decoder.h"
#include "audio_reader.h"
#include "audio_writer.h"
#include "audio_tonegen.h"
#include "vfs.h"
#include <string.h>
#include <stdlib.h>




static void prvAudioPlayEntry(void *param)
{

    MG_auTonePlayCustom(9000, 9000, 10000);
    osiThreadSleep(1000);
    osiThreadExit();
}

int appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);
    osiThreadCreate("auplay", prvAudioPlayEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 4);
    return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
