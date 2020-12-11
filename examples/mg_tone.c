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

#define LOG_TAG OSI_MAKE_LOG_TAG('A', 'P', 'L', 'Y')

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
	unsigned level = 88;
	unsigned playvol_level = 0;
	unsigned Micvol_level = 0;
	unsigned voice_level = 0;


	MG_audevSetPlayVolume(level);
	MG_audevSetVoiceVolume(level);
	//MG_audevSetMicVolume(level);

	playvol_level = MG_audevGetPlayVolume();
	voice_level = MG_audevGetVoiceVolume();
	//Micvol_level = MG_audevGetMicVolume();

	MG_osiTracePrintf(LOG_TAG, "the play volume level   is  %d, Mic volume level is %d",playvol_level,Micvol_level);

	MG_audevPlayTone(AUDEV_TONE_SUBSCRIBER_BUSY, 10000);

	MG_osiThreadSleep(10000);

	MG_audevStopTone();

	MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg tone example enter");
    osiThreadCreate("auplay", prvAudioPlayEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 4);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg tone example exit");
}
