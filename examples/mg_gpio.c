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

#include "osi_api.h"
#include "osi_log.h"
#include "drv_gpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CONFIG_DETECT_PRESS_DEBOUNCE_TIMEOUT (100) // ms
#define CONFIG_DETECT_RELEASE_DEBOUNCE_TIMEOUT (100) // ms

drvGpioConfig_t cfg_in = {
    .mode = DRV_GPIO_INPUT,
    .intr_enabled = true,
    .intr_level = false,
    .rising = true,
    .falling = false,
    .debounce = true,
};

typedef struct
{
    int flag;
    int lock_state;
    bool pressed;
    osiTimer_t *debounce_timer;
    drvGpio_t *gpio;
} cbDetect_t;
static cbDetect_t gcb;

bool srvGpioPressSwitch(bool status)
{
    cbDetect_t *gc = &gcb;
    if (gc->gpio == NULL)
    {
        return false;
    }

    (status == true) ? (cfg_in.rising = false) : (cfg_in.rising = true);
    (status == true) ? (cfg_in.falling = true) : (cfg_in.falling = false);

    return MG_drvGpioReconfig(gc->gpio, &cfg_in);
}

static void _debounceCB(void *gc_)
{
    cbDetect_t *gc = (cbDetect_t *)gc_;
    MG_osiTracePrintf(LOG_TAG, "gpio simulate key:current_state:%d, doing something here\n", gc->flag);
    srvGpioPressSwitch(gc->pressed);
    gc->lock_state = 0;
}

static void _gpioCb(void *gc_)
{
    cbDetect_t *gc = (cbDetect_t *)gc_;

    //Avoid triggering interrupt processing multiple times
    bool current_state = MG_gpio_get(gc->gpio);
    if (gc->pressed == current_state)
        return;
    gc->pressed = current_state;
    gc->lock_state ++;
    MG_osiTracePrintf(LOG_TAG, "gpio simulate key:gpio state %d\n", gc->pressed);

    //interrupt processing
    if (gc->pressed == true && gc->lock_state == 1)
    {
        gc->flag = 1;
        osiTimerStart(gc->debounce_timer, CONFIG_DETECT_PRESS_DEBOUNCE_TIMEOUT);
    }
    if (gc->pressed == false && gc->lock_state == 1)
    {
        gc->flag = 2;
        osiTimerStart(gc->debounce_timer, CONFIG_DETECT_RELEASE_DEBOUNCE_TIMEOUT);
    }
}

static void prvThreadEntry(void *param)
{
    int status_read, status_write;
    cbDetect_t *gc = &gcb;

    drvGpio_t *Gpio_info;

    drvGpioConfig_t cfg_out = {
        .mode = DRV_GPIO_OUTPUT,
	.out_level=false,
	.intr_level = false,
        .rising = false,
        .falling = false,
        .debounce = false,
    };	

    /***example 1, use GPIO31 as output mode***/
    Gpio_info = MG_gpio_open(31, &cfg_out, NULL, NULL);
    if (Gpio_info == NULL) {
	MG_osiTracePrintf(LOG_TAG, "gpio%d init failure\n", 31);
	goto GPIO_open_fail;
    }
    //Write level for GPIO31
    status_write = MG_gpio_set(Gpio_info, 1);
    if (status_write == 1) {
	MG_osiTracePrintf(LOG_TAG, "gpio%d write success\n", 31);
    }else {
	MG_osiTracePrintf(LOG_TAG, "gpio%d write fail\n", 31);
    }
    //Read the level of GPIO31
    status_read = MG_gpio_get(Gpio_info);
    if (status_read == 1) {
	MG_osiTracePrintf(LOG_TAG, "gpio%d status is high\n", 31);
    }else {
	MG_osiTracePrintf(LOG_TAG, "gpio%d status is low\n", 31);
    }

    MG_osiThreadSleep(1000);
    //close GPIO31
    MG_gpio_close(Gpio_info);// MG_gpio_close(ledGpio_info_1);

    /***example 2, use GPIO31 as intput mode, simulate a key***/
    gc->flag = 0;
    gc->lock_state = 0;
    gc->debounce_timer = osiTimerCreate(NULL, _debounceCB, gc);
    gc->gpio = MG_gpio_open(31, &cfg_in, _gpioCb, gc);
    gc->pressed = MG_gpio_get(gc->gpio);
    MG_osiTracePrintf(LOG_TAG, "gpio simulate key:gpio state %d\n", gc->pressed);
    if (gc->gpio == NULL) {
	goto GPIO_open_fail;
    }	
    MG_osiTracePrintf(LOG_TAG, "gpio init succeed\n");
    MG_osiThreadSleep(10000);//if you want to try interrupt function more times, please increase delay or disable MG_gpio_close
    MG_gpio_close(gc->gpio);

    MG_osiThreadExit();
	
GPIO_open_fail:
    MG_osiTracePrintf(LOG_TAG, "gpio init failure\n");
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-gpio", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}


