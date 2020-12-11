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

#ifndef _TTS_PLAYER_H_
#define _TTS_PLAYER_H_

#include "osi_compiler.h"
#include "tts_config.h"

OSI_EXTERN_C_BEGIN

typedef void (*ttsFinsihCB_t)(void *ctx);

/**
 * \brief initialize tts player
 *
 * TTS player is designed as singleton.
 */
bool MG_ttsPlayerInit(void);

/**
 * \brief TTS play text
 *
 * This is asynchronized, that is it will return immediately after the play
 * event is sent.
 *
 * When \p len is -1, the actual text size will be determined by terminating
 * NUL character.
 *
 * \p text is the string to be played, encoded with \p encoding. Refer to
 * ml.h for the value of \p encoding. Not all encodings are supported.
 *
 * \param text      text to be played
 * \param len       text len, -1 for terminated with NUL
 * \param encoding  text encoding
 * \return
 *      - true on success
 *      - false on failed
 */
bool MG_ttsPlayText(const char *text, int size, ttsFinsihCB_t cb, void *ctx);

/**
 * \brief whether tts is playing
 *
 * \return
 *      - true if there are text in playing
 *      - false if no text in playing
 */
bool MG_ttsIsPlaying(void);

/**
 * \brief get or set tts param
 *
 * \param mode      set 0 when you need get params and set 1 when you need set params
 * \param param     0--tts voice speed(value is between -32768 and 32767, the default value is 0)
 *                  1--tts voice volume(value is between -32768 and 32767, the default value is 0)
 *                  2--tts codepage(value is 437, 936, 65001, 1201, 1200; 437--ASCII, 936--GBK, 65001--UTF8,
 *                                     1201--UTF16BE, 1200--UTF16LE, the default value is 936)
 *                  3--tts language(value is 0, 1, 2; 0--language auto, 1--Chinese, 2--English, the default value is 0)
 * \param value     when mode is 0, write 0; when mode is 1, write a value that matches the parameter
 * \return
 *      - value if mode is 0
 *      - 0 on sucess or -1 on failed when mode is 1
 */
int MG_ttsParamGet(int mode, int param, int value);


/**
 * \brief playback stop
 *
 * After stop, internal resources of audio player may be freed.
 *
 */
void MG_audioPlayerFinished(void);

OSI_EXTERN_C_END

#endif
