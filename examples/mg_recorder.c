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

#define EXAMPLE_FILE_NAME "/example.pcm"

static void prvRecordMem(void)
{
    auRecorder_t *recorder = MG_auRecorderCreate();
    MG_auRecorderStartMem(recorder, AUDEV_RECORD_TYPE_MIC, AUSTREAM_FORMAT_PCM, NULL, 32000);

    MG_osiThreadSleep(2000);

    auMemWriter_t *writer = (auMemWriter_t *)auRecorderGetWriter(recorder);
    osiBuffer_t wbuf = auMemWriterGetBuf(writer);
   	MG_osiTracePrintf(LOG_TAG, "record memory 0x%x size/%x", wbuf.ptr, wbuf.size);

    MG_auRecorderStop(recorder);
    MG_auRecorderDelete(recorder);
}

static void prvRecordFile(void)
{
    auRecorder_t *recorder = MG_auRecorderCreate();
    MG_auRecorderStartFile(recorder, AUDEV_RECORD_TYPE_MIC, AUSTREAM_FORMAT_PCM, NULL, EXAMPLE_FILE_NAME);

    MG_osiThreadSleep(2000);

    MG_auRecorderStop(recorder);
    MG_auRecorderDelete(recorder);

    int file_size = vfs_file_size(EXAMPLE_FILE_NAME);
    MG_osiTracePrintf(LOG_TAG, "record file size/%d", file_size);
}

typedef struct
{
    osiPipe_t *pipe;
    int fd;
} pipeAndFd_t;

static void prvPipeReadWork(void *param)
{
    pipeAndFd_t *d = (pipeAndFd_t *)param;

    char buf[256];
    for (;;)
    {
        int bytes = osiPipeRead(d->pipe, buf, 256);
        if (bytes <= 0)
            break;

        vfs_write(d->fd, buf, bytes);
    }
}

static void prvPipeReadCallback(void *param, unsigned event)
{
    osiWork_t *work = (osiWork_t *)param;
    osiWorkEnqueue(work, osiSysWorkQueueLowPriority());
}

static void prvRecordPipe(void)
{
    int fd = vfs_open(EXAMPLE_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC);
    osiPipe_t *pipe = osiPipeCreate(4096);
    pipeAndFd_t pipefd = {.pipe = pipe, .fd = fd};
    osiWork_t *work = osiWorkCreate(prvPipeReadWork, NULL, &pipefd);
    osiPipeSetReaderCallback(pipe, OSI_PIPE_EVENT_RX_ARRIVED, prvPipeReadCallback, work);

    auRecorder_t *recorder = MG_auRecorderCreate();
    MG_auRecorderStartPipe(recorder, AUDEV_RECORD_TYPE_MIC, AUSTREAM_FORMAT_PCM, NULL, pipe);

    MG_osiThreadSleep(2000);

    MG_auRecorderStop(recorder);
    MG_auRecorderDelete(recorder);
    osiWorkWaitFinish(work, OSI_WAIT_FOREVER);
    osiWorkDelete(work);
    osiPipeDelete(pipe);
    vfs_close(fd);

    int file_size = vfs_file_size(EXAMPLE_FILE_NAME);
    MG_osiTracePrintf(LOG_TAG, "record pipe file size/%d", file_size);
}

static void prvRecordWriter(void)
{
    auWriter_t *writer = (auWriter_t *)auFileWriterCreate(EXAMPLE_FILE_NAME);
    auRecorder_t *recorder = MG_auRecorderCreate();
    MG_auRecorderStartWriter(recorder, AUDEV_RECORD_TYPE_MIC, AUSTREAM_FORMAT_PCM, NULL, writer);

    MG_osiThreadSleep(2000);

    MG_auRecorderStop(recorder);
    MG_auRecorderDelete(recorder);
    MG_auWriterDelete(writer);

    int file_size = vfs_file_size(EXAMPLE_FILE_NAME);
   	MG_osiTracePrintf(LOG_TAG, "record writer file size/%d", file_size);
}

static void prvRecordEncoder(void)
{
    auWriter_t *writer = (auWriter_t *)auFileWriterCreate(EXAMPLE_FILE_NAME);
    auEncoder_t *encoder = auEncoderCreate(writer, AUSTREAM_FORMAT_PCM);
    auRecorder_t *recorder = MG_auRecorderCreate();
    MG_auRecorderStartEncoder(recorder, AUDEV_RECORD_TYPE_MIC, encoder);

    MG_osiThreadSleep(2000);
	MG_auRecorderPause(recorder);
	MG_osiThreadSleep(2000);
	MG_auRecorderResume(recorder);
	MG_osiThreadSleep(2000);
    MG_auRecorderStop(recorder);
    MG_auRecorderDelete(recorder);
    MG_auEncoderDelete(encoder);
    MG_auWriterDelete(writer);

    int file_size = vfs_file_size(EXAMPLE_FILE_NAME);
    MG_osiTracePrintf(LOG_TAG, "record encoder file size/%d", file_size);
}

static void prvAudioPlayEntry(void *param)
{
    prvRecordMem();
    prvRecordFile();
    prvRecordPipe();
    prvRecordWriter();
    prvRecordEncoder();

    vfs_unlink(EXAMPLE_FILE_NAME);
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "application image enter, param 0x%x", param);
    osiThreadCreate("auplay", prvAudioPlayEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 4);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "application image exit");
}
