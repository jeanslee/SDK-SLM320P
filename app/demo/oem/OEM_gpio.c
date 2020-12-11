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
#include "drv_keypad.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oem.h"

#define CONFIG_DETECT_PRESS_DEBOUNCE_TIMEOUT (100) // ms
#define CONFIG_DETECT_RELEASE_DEBOUNCE_TIMEOUT (100) // ms

#define CONFIG_PTT_GPIO_ID (29)



drvGpioConfig_t cfg_in = {
    .mode = DRV_GPIO_INPUT,
    .intr_enabled = true,
    .intr_level = false,
    .rising = false,
    .falling = true,
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


extern uint8_t g_PttKeyStastus;

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

	if(gc->flag== KEY_STATE_PRESS)
	{
		g_PttKeyStastus=KEY_STATE_PRESS;
		
	}else if(gc->flag== KEY_STATE_RELEASE)
	{
		g_PttKeyStastus=KEY_STATE_RELEASE;
	}
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
    if (gc->pressed == false && gc->lock_state == 1)
    {
        gc->flag = KEY_STATE_PRESS;//press
        osiTimerStart(gc->debounce_timer, CONFIG_DETECT_PRESS_DEBOUNCE_TIMEOUT);
    }
    if (gc->pressed == true && gc->lock_state == 1)
    {
        gc->flag = KEY_STATE_RELEASE;//release
        osiTimerStart(gc->debounce_timer, CONFIG_DETECT_RELEASE_DEBOUNCE_TIMEOUT);
    }
}

void OEM_GpioThread(void *param)
{
    cbDetect_t *gc = &gcb;
    /***example, use GPIO29 as intput mode, simulate the key of PTT***/
    gc->flag = 0;
    gc->lock_state = 0;
    gc->debounce_timer = osiTimerCreate(NULL, _debounceCB, gc);
    gc->gpio = MG_gpio_open(CONFIG_PTT_GPIO_ID, &cfg_in, _gpioCb, gc);
    gc->pressed = MG_gpio_get(gc->gpio);
    MG_osiTracePrintf(LOG_TAG, "gpio simulate key:gpio state %d\n", gc->pressed);
    if (gc->gpio == NULL) {
	goto GPIO_open_fail;
    }	
    MG_osiTracePrintf(LOG_TAG, "gpio init succeed\n");
    //MG_osiThreadSleep(10000);//if you want to try interrupt function more times, please increase delay or disable MG_gpio_close
    //MG_gpio_close(gc->gpio);
    for (;;)
    {
		osiEvent_t event;
		event.id = OSI_EVENT_ID_NONE;    
        osiEventTryWait(MG_osiThreadCurrent(), &event,300*1000);
        OSI_LOGI(0, "receive event %d", event.id);
        if (event.id == OSI_EVENT_ID_QUIT)
            break;    
    }	
GPIO_open_fail:
    MG_osiTracePrintf(LOG_TAG, "gpio init failure\n");
    MG_osiThreadExit();
}



