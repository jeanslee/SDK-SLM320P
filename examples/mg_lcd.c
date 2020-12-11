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

#include <string.h>
#include <stdlib.h>
#include "osi_log.h"
#include "osi_api.h"
#include "drv_lcd.h"

typedef struct
{
    lcdSpec_t info;
    uint16_t *buffer;
} mg_lcdExample_t;
static void prvFillVerticalColorBarRgb565(uint16_t *buffer, unsigned width, unsigned height)
{
    for (unsigned h = 0; h < height; ++h)
    {
        for (unsigned w = 0; w < width; ++w)
        {
            if (w < width / 3)
                buffer[h * width + w] = 0xf800;
            else if (w < width / 3 * 2)
                buffer[h * width + w] = 0x07e0;
            else
                buffer[h * width + w] = 0x001f;
        }
    }
}

static void prvFillHorizontalColorBarRgb565(uint16_t *buffer, unsigned width, unsigned height)
{
    for (unsigned h = 0; h < height; ++h)
    {
        for (unsigned w = 0; w < width; ++w)
        {
            if (h < height / 3)
                buffer[h * width + w] = 0xf800;
            else if (h < height / 3 * 2)
                buffer[h * width + w] = 0x07e0;
            else
                buffer[h * width + w] = 0x001f;
        }
    }
}

static void prvFillBufferWhiteScreen(void *buffer, unsigned width, unsigned height)
{
    memset(buffer, 0xff, width * height * sizeof(uint16_t));
}

static void prvLcdClear(mg_lcdExample_t *lcd)
{
    prvFillBufferWhiteScreen(lcd->buffer, lcd->info.width, lcd->info.height);
    lcdFrameBuffer_t fb = {};
    fb.buffer = lcd->buffer;
    fb.colorFormat = LCD_RESOLUTION_RGB565;
    fb.height = lcd->info.height;
    fb.width = lcd->info.width;
    fb.widthOriginal = lcd->info.width;

    lcdDisplay_t rec = {};
    rec.width = lcd->info.width;
    rec.height = lcd->info.height;
    mg_drvLcdBlockTransfer(&fb, &rec);
}

static void prvLcdDisplayColorBar(mg_lcdExample_t *lcd, uint16_t width, uint16_t height,
                                  uint16_t x, uint16_t y, bool vertical)
{
    if (width > lcd->info.width)
        width = lcd->info.width;
    if (height > lcd->info.height)
        height = lcd->info.width;

    if (vertical)
        prvFillVerticalColorBarRgb565(lcd->buffer, width, height);
    else
        prvFillHorizontalColorBarRgb565(lcd->buffer, width, height);

    lcdDisplay_t rec = {};
    rec.x = x;
    rec.y = y;
    rec.width = width;
    rec.height = height;

    lcdFrameBuffer_t fb = {};
    fb.buffer = lcd->buffer;
    fb.colorFormat = LCD_RESOLUTION_RGB565;
    fb.height = height;
    fb.width = width;
    fb.widthOriginal = width;
    mg_drvLcdBlockTransfer(&fb, &rec);
}

static void prvLcdInit(mg_lcdExample_t *lcd)
{   
    mg_drvLcdInit();
    mg_drvLcdGetLcdInfo(&lcd->info);
    lcd->buffer = (uint16_t *)malloc(lcd->info.width * lcd->info.height * sizeof(uint16_t));
    prvLcdClear(lcd);    
}

static void prvLcdThread(void *param)
{
    mg_lcdExample_t lcd;
	
    prvLcdInit(&lcd);
	/*vertical display*/
    prvLcdClear(&lcd);	
    prvLcdDisplayColorBar(&lcd, &lcd.info.width, &lcd.info.height,0,0,true);
	MG_osiThreadSleep(3000);
	/*horizontal display*/
	prvLcdClear(&lcd);
	prvLcdDisplayColorBar(&lcd, 128, 128, 40, 160, false);
	MG_osiThreadSleep(3000);
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
   MG_osiTracePrintf(LOG_TAG, "mg example enter");

    MG_osiThreadCreate("meg-lcd", prvLcdThread, NULL, OSI_PRIORITY_LOW, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}