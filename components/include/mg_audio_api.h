
#ifndef _MG_AUDIO_API_H_
#define _MG_AUDIO_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef enum
{
    MG_AUSTREAM_FORMAT_UNKNOWN, ///< placeholder for unknown format
    MG_AUSTREAM_FORMAT_PCM,     ///< raw PCM data
    MG_AUSTREAM_FORMAT_WAVPCM,  ///< WAV, PCM inside
    MG_AUSTREAM_FORMAT_MP3,     ///< MP3
    MG_AUSTREAM_FORMAT_AMRNB,   ///< AMR-NB
    MG_AUSTREAM_FORMAT_AMRWB,   ///< AMR_WB
} MG_auStreamFormat;

#define AUDIO_RSP_PLAY_BUSY 0
#define AUDIO_RSP_PLAY_FINISH 1

#ifdef __cplusplus
extern "C" {
#endif

int MG_AudioStreamRecordStart(void * cb,MG_auStreamFormat format,uint32_t frame_size);
void MG_AudioStreamRecordStop(void);

int MG_AudioPlayStart(void);
void MG_AudioPlayEnd(void);
int MG_AudioPlayMem(void * cb, MG_auStreamFormat format,uint8_t *buf, uint32_t size);
int MG_AudioStreamPlay(MG_auStreamFormat format,uint8_t *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif



