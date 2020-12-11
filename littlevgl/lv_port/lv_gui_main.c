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

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define DUMP_LCD_BUFFER 0
#include "lv_gui_main.h"
#include "drv_lcd_v2.h"
#include "drv_names.h"
#include "drv_keypad.h"
#include "lvgl.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_config.h"
#include "ats_config.h"
#include <string.h>
#include <stdlib.h>
#if DUMP_LCD_BUFFER
#include "vfs.h"
#include "vfs_ops.h"
#endif
#ifdef CONFIG_FILL_BLANK
#define GUI_BLANK_288 1
#else
#define GUI_BLANK_288 0 
#endif
#if DUMP_LCD_BUFFER
int32_t gFd=0;
#endif
static lv_color_t *temp_buf=NULL,*ror_buf=NULL,*ror2_buf=NULL;
static uint32_t lcd_count=0;
#define EV_MMI_EV_BASE 0x00100000
#define EV_USER_UART_CTL (EV_MMI_EV_BASE + 1)
#define EV_USER_UART_RESP (EV_MMI_EV_BASE + 2)
#define EV_USER_KEY (EV_MMI_EV_BASE + 3)
#define EV_SCREEN_OFF (EV_MMI_EV_BASE + 4)
#define EV_SCREEN_ON (EV_MMI_EV_BASE + 5)

typedef struct
{
    bool screen_on;            // state of screen on
    bool keypad_pending;       // keypad pending, set in ISR, clear in thread
    bool anim_inactive;        // property of whether animation is regarded as inactive
    drvLcd_t *lcd;             // LCD instance
    osiThread_t *thread;       // gui thread
    osiTimer_t *task_timer;    // timer to trigger task handler
    drvLcdVideoLayer_t vl;     // extern video layer
    lv_disp_buf_t disp_buf;    // display buffer
    lv_disp_t *disp;           // display device
    lv_indev_t *keypad;        // keypad device
    keyMap_t last_key;         // last key from ISR
    keyState_t last_key_state; // last key state from ISR
    uint32_t screen_on_users;  // screen on user bitmap
    uint32_t inactive_timeout; // property of inactive timeout
} lvGuiContext_t;

typedef struct
{
    uint8_t keypad;
    uint8_t lv_key;
} lvGuiKeypadMap_t;

static const lvGuiKeypadMap_t gLvKeyMap[] = {
    {KEY_MAP_POWER, LV_KEY_POWER},
#if !defined(CONFIG_BOARD_MG_MODULE_NAME_MAYI)
#if defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320P) || defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320)	
    {KEY_MAP_SIM2, LV_KEY_0},
    {KEY_MAP_3, LV_KEY_1},
    {KEY_MAP_VOLUME_UP, LV_KEY_2},		
    {KEY_MAP_WELL, LV_KEY_3},	
    {KEY_MAP_SOFT_R, LV_KEY_4},	
    {KEY_MONEY, LV_KEY_5},	
    {KEY_MAP_6, LV_KEY_6},
    {KEY_MAP_LEFT, LV_KEY_7},
    {KEY_MAP_8, LV_KEY_8},	
    {KEY_MAP_STAR, LV_KEY_9},
    {KEY_MAP_9, LV_KEY_A},
    {KEY_MAP_VOLUME_DOWN, LV_KEY_B},
    {KEY_MAP_SOFT_L, LV_KEY_C},
    {KEY_MAP_OK, LV_KEY_D},
    {KEY_BACK, LV_KEY_E},
    {KEY_MAP_SLEEP, LV_KEY_F},	
    {KEY_MAP_7, LV_KEY_LEFT},
    {KEY_MAP_UP, LV_KEY_RIGHT},
    {KEY_MAP_RIGHT, LV_KEY_UP},
    {KEY_MAP_DOWN, LV_KEY_DOWN},
   {KEY_MAP_ESC,LV_KEY_HOME},
    {KEY_MAP_MENU, LV_KEY_ENTER},	
    {KEY_MAP_4, LV_KEY_BACK},
    {KEY_MAP_SHARP, LV_KEY_PREV},	
    {KEY_MAP_SIM1, LV_KEY_NEXT},
#else
    {KEY_MAP_SIM1, 0xf1},
    {KEY_MAP_SIM2, 0xf2},
    {KEY_MAP_0, '0'},
    {KEY_MAP_1, '1'},
    {KEY_MAP_2, '2'},
    {KEY_MAP_3, '3'},
    {KEY_MAP_4, '4'},
    {KEY_MAP_5, '5'},
    {KEY_MAP_6, '6'},
    {KEY_MAP_7, '7'},
    {KEY_MAP_8, '8'},
    {KEY_MAP_9, '9'},
    {KEY_MAP_STAR, '*'},
    {KEY_MAP_WELL, '#'},
    {KEY_MAP_LEFT, LV_KEY_LEFT},
    {KEY_MAP_RIGHT, LV_KEY_RIGHT},
    {KEY_MAP_UP, LV_KEY_UP},
    {KEY_MAP_DOWN, LV_KEY_DOWN},
    {KEY_MAP_SOFT_L, LV_KEY_PREV},
    {KEY_MAP_SOFT_R, LV_KEY_NEXT},
#endif	
	
#else
    {KEY_MAP_2, LV_KEY_NEXT},
    {KEY_MAP_8, LV_KEY_PREV},
    {KEY_MAP_0, LV_KEY_0},
    {KEY_MAP_1, LV_KEY_1},
    //{KEY_MAP_2, LV_KEY_2},
    {KEY_MAP_3, LV_KEY_3},
    {KEY_MAP_4, LV_KEY_4},
    {KEY_MAP_5, LV_KEY_5},
    {KEY_MAP_6, LV_KEY_6},
    {KEY_MAP_7, LV_KEY_7},
    //{KEY_MAP_8, LV_KEY_8},
    {KEY_MAP_9, LV_KEY_9},
    {KEY_MAP_CANCEL, LV_KEY_DEL},
    {KEY_MAP_DOT, LV_KEY_DOT},
    {KEY_BACK, LV_KEY_BACK},
    {KEY_MONEY, LV_KEY_MONEY},
    {KEY_MAP_PLUS, LV_KEY_PLUS},
    {KEY_MAP_SCAN, LV_KEY_SCAN},
    {KEY_MAP_STORE, LV_KEY_STORE},
    {KEY_MAP_OK, LV_KEY_ENTER},	
#endif
};

static lvGuiContext_t gLvGuiCtx;
extern bool mg_proc_key(int key,int state);
extern void mg_update_ui();

uint32_t gTick;
static int gTestCnt = 0;
static int gTestcount=0;
//static bool gotKey = false;

static void prvFillVerticalColorBarRgb565(uint16_t *buffer, unsigned width, unsigned height)
{
    for (unsigned h = 0; h < height; ++h)
    {
        for (unsigned w = 0; w < width; ++w)
        {
            if (w < width / 5)
                buffer[h * width + w] = 0xf800;
            else if (w < 2*width/5)
                buffer[h * width + w] = 0x07e0;
            else if (w < 3*width/5)
                buffer[h * width + w] = 0x001f;
            else if (w < 4*width/5)
                buffer[h * width + w] = 0x0000;
            else
                buffer[h * width + w] = 0xffff;
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
static void prvFillBlack(uint16_t *buffer, unsigned width, unsigned height)
{
    for (unsigned h = 0; h < height; ++h)
    {
        for (unsigned w = 287; w < width; ++w)
        {
            buffer[h * width + w] = 0x0000;
        }
    }
}
#if defined(CONFIG_BOARD_MG_MODULE_NAME_MAYI)
/**
 * flush display forcedly
 */
static void prvDispForceFlush(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    drvLcdArea_t roi = {
        .x = 0,
        .y = 0,
        .w = d->disp->driver.ver_res,
        .h = d->disp->driver.hor_res,
    };
	if(ror2_buf==NULL)
	{
		ror2_buf = (lv_color_t *)malloc(roi.w*roi.h*sizeof(lv_color_t));
		if(ror2_buf==NULL)
			return;
	}	
	drvRotationLcdBuffer(1,d->disp_buf.buf1,ror2_buf,roi.w,roi.h);
    drvLcdOverlay_t ovl = {
        .buf = ror2_buf,
        .enabled = true,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
        .key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };

    drvLcdLayers_t layers = {
        .vl = &d->vl,
        .ovl = {NULL, NULL, &ovl},
        .layer_roi = roi,
        .screen_roi = roi,
    };

    drvLcdFlush(d->lcd, &layers, true);
	if(ror2_buf)
	{
		free(ror2_buf);
		ror2_buf = NULL;
	}
}
#else
static void prvDispForceFlush(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    drvLcdArea_t roi = {
        .x = 0,
        .y = 0,
        .w = d->disp->driver.hor_res,
        .h = d->disp->driver.ver_res,
    };

    drvLcdOverlay_t ovl = {
        .buf = d->disp_buf.buf1,
        .enabled = true,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
        .key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };

    drvLcdLayers_t layers = {
        .vl = &d->vl,
        .ovl = {NULL, NULL, &ovl},
        .layer_roi = roi,
        .screen_roi = roi,
    };

    drvLcdFlush(d->lcd, &layers, true);
}
#endif
void start_lcd_test(){
    gTestCnt = 10;
	//gTestcount=18;
    OSI_LOGD(0, "gTestCnt  %d",gTestCnt);
}

/**
 * display device flush_cb
 */
#if defined(CONFIG_BOARD_MG_MODULE_NAME_MAYI)
static void prvDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    drvLcdArea_t roi = {
        .x = area->x1,
        .y = area->y1,
        .h = area->x2 - area->x1 + 1,
        .w = area->y2 - area->y1 + 1,
    };
    /*
    if(gotKey ){
        OSI_LOGD(0, "got key");
        gotKey = false;
        lv_disp_flush_ready(disp);
        return;
    }
    */

    if(temp_buf==NULL)
	{
		temp_buf = (lv_color_t *)malloc(roi.w*roi.h*sizeof(lv_color_t));
	}
	if(ror_buf==NULL)
	{
		ror_buf = (lv_color_t *)malloc(roi.w*roi.h*sizeof(lv_color_t));
	}

    OSI_LOGD(0, "disp flush %d/%d/%d/%d test:%d", roi.x, roi.y, roi.w, roi.h,gTestCnt);

	if(gTestcount>1)
	gTestcount--;
    //OSI_LOGD(0,"gTestcount %d",gTestcount);
	if(gTestcount==2)
	{
		gTestCnt=10;
	}	
    if(gTestCnt >0){
        if(gTestCnt == 10)
        {        	
            if(temp_buf==NULL)  
            {
                OSI_LOGD(0, "gTestCnt temp_buf null");
            }
            //OSI_LOGD(0, "color_p1  %x buf1 %x temp_buf %x",color_p,d->disp_buf.buf1,temp_buf);
            memcpy(temp_buf,color_p,roi.w*roi.h*sizeof(lv_color_t));
            //OSI_LOGD(0, "color_p2  %x buf1 %x temp_buf %x",color_p,d->disp_buf.buf1,temp_buf);
        }
        --gTestCnt;
		if(gTestCnt%2==0)
			prvFillHorizontalColorBarRgb565(color_p,roi.w,roi.h);
		else
        	prvFillBlack(color_p,roi.w,roi.h);
        //MG_drvLcdBacklightBrightness(gTestCnt%17);
    }
    else{
        if(strstr(GMM_ID,"MAYI") != NULL){			
            prvFillBlack(color_p,320,240);
        }
    }
    if(gTestCnt == 1){
        //OSI_LOGD(0, "gTestCnt is 1");
        OSI_LOGD(0, "color_p1  len %x len %x",(color_p),(temp_buf));
		//prvFillBlack(color_p,roi.w,roi.h);
	    OSI_LOGD(0, "color_p2  len %x len %x",(color_p),(temp_buf));
        memcpy(color_p,temp_buf,roi.w*roi.h*sizeof(lv_color_t));
		
		//free(temp_buf);
        //MG_drvLcdBacklightBrightness(10);
        //prvFillBlack(color_p,roi.w,roi.h);
		
        gTestCnt = 0;
    }
	drvRotationLcdBuffer(1,color_p,ror_buf,roi.w,roi.h);
	#if DUMP_LCD_BUFFER
	lcd_count++;
	/*Save lcd buffer to file*/
	if(gFd>0 && lcd_count==10)
	{
		vfs_write(gFd,ror_buf,roi.w*roi.h*sizeof(lv_color_t));
		vfs_close(gFd);
		gFd=-1;
		OSI_LOGI(0, "vfs_write");
	}
	#endif	
	drvLcdVideoLayer_t vl = {
		.buf = ror_buf,
        .enabled = false,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
		.rotation = false,
		.buf_width = roi.w,
		.buf_height = roi.h,
		.key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };
	
    drvLcdOverlay_t ovl = {
		.buf = ror_buf,
        .enabled = true,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
        .key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };
    drvLcdLayers_t layers = {
        .vl = &d->vl,
		//.vl = &vl,
        .ovl = {NULL, NULL, &ovl},
        .layer_roi = roi,
        .screen_roi = roi,
    };
    drvLcdFlush(d->lcd, &layers, true);
    lv_disp_flush_ready(disp);
	//drvLcdSetBackLightEnable(d->lcd, true);
	
	if(temp_buf)
	{
		free(temp_buf);
		temp_buf = NULL;
	}
	if(ror_buf)
	{
		free(ror_buf);
		ror_buf = NULL;
	}

}
#else
static void prvDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    drvLcdArea_t roi = {
        .x = area->x1,
        .y = area->y1,
        .w = area->x2 - area->x1 + 1,
        .h = area->y2 - area->y1 + 1,
    };
    /*
    if(gotKey ){
        OSI_LOGD(0, "got key");
        gotKey = false;
        lv_disp_flush_ready(disp);
        return;
    }
    */

    if(temp_buf==NULL)
	{
		temp_buf = (lv_color_t *)malloc(roi.w*roi.h*sizeof(lv_color_t));
	}

    OSI_LOGD(0, "disp flush %d/%d/%d/%d test:%d", roi.x, roi.y, roi.w, roi.h,gTestCnt);

	if(gTestcount>1)
	gTestcount--;
    //OSI_LOGD(0,"gTestcount %d",gTestcount);
	if(gTestcount==2)
	{
		gTestCnt=10;
	}
	

    if(gTestCnt >0){
        if(gTestCnt == 10)
        {
        	
            if(temp_buf==NULL)  
            {
                OSI_LOGD(0, "gTestCnt temp_buf null");
            }
            //OSI_LOGD(0, "color_p1  %x buf1 %x temp_buf %x",color_p,d->disp_buf.buf1,temp_buf);
            memcpy(temp_buf,color_p,roi.w*roi.h*sizeof(lv_color_t));
            //OSI_LOGD(0, "color_p2  %x buf1 %x temp_buf %x",color_p,d->disp_buf.buf1,temp_buf);
        }
        --gTestCnt;
        prvFillVerticalColorBarRgb565(color_p,roi.w,roi.h);
        //MG_drvLcdBacklightBrightness(gTestCnt%17);
    }
    else{
        if(strstr(GMM_ID,"MAYI") != NULL){
            prvFillBlack(color_p,roi.w,roi.h);
        }
    }
    if(gTestCnt == 1){
        //OSI_LOGD(0, "gTestCnt is 1");
        OSI_LOGD(0, "color_p1  len %x len %x",(color_p),(temp_buf));
		//prvFillBlack(color_p,roi.w,roi.h);
	    OSI_LOGD(0, "color_p2  len %x len %x",(color_p),(temp_buf));
        memcpy(color_p,temp_buf,roi.w*roi.h*sizeof(lv_color_t));
		
		//free(temp_buf);
        //MG_drvLcdBacklightBrightness(10);
        //prvFillBlack(color_p,roi.w,roi.h);
		
        gTestCnt = 0;
    }
    drvLcdOverlay_t ovl = {
        .buf = color_p,
        .enabled = true,
        .in_fmt = DRV_LCD_IN_FMT_RGB565,
        .alpha = 255,
        .key_en = LV_INDEXED_CHROMA,
        .key_color = lv_color_to16(LV_COLOR_TRANSP),
        .stride = roi.w,
        .out = roi,
    };
    drvLcdLayers_t layers = {
        .vl = &d->vl,
        .ovl = {NULL, NULL, &ovl},
        .layer_roi = roi,
        .screen_roi = roi,
    };
    
    drvLcdFlush(d->lcd, &layers, true);
    lv_disp_flush_ready(disp);
	//drvLcdSetBackLightEnable(d->lcd, true);
}
#endif
/**
 * initialize LCD display device
 */
static bool prvLvInitLcd(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->lcd = drvLcdGetByname(DRV_NAME_LCD1);
    if (d->lcd == NULL)
        return false;
	//drvLcdSetBackLightEnable(d->lcd, false);
   // if (!drvLcdOpenV2(d->lcd))
    //    return false;
#if defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320P) || defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320) || defined(CONFIG_BOARD_MG_MODULE_NAME_SLM330)
    drvLcdSetDirection(d->lcd, DRV_LCD_DIR_ROTATE_90);
	//drvLcdSetDirection(d->lcd, DRV_LCD_DIR_ROTATE_270);
#endif	
    //drvLcdSetDirection(d->lcd, DRV_LCD_DIR_NORMAL);
#if DUMP_LCD_BUFFER
    gFd = vfs_open("/lcd.raw",O_RDWR|O_CREAT|O_TRUNC);
	if(gFd<0)
		OSI_LOGI(0, "vfs_open lcd failure");
#endif
    drvLcdPanelInfo_t panel_info;
    if (!drvLcdGetPanelInfo(d->lcd, &panel_info))
        return false;


    unsigned pixel_cnt = panel_info.width * panel_info.height;
    lv_color_t *buf = (lv_color_t *)malloc(pixel_cnt * sizeof(lv_color_t));
    if (buf == NULL)
        return false;

    OSI_LOGD(0, "width/height %d/%d", panel_info.width , panel_info.height);
    lv_disp_buf_init(&d->disp_buf, buf, buf, pixel_cnt);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = prvDispFlush;
    disp_drv.buffer = &d->disp_buf;
    disp_drv.ver_res = panel_info.height;     
    disp_drv.hor_res = panel_info.width;    

    d->disp = lv_disp_drv_register(&disp_drv); // pointer copy
    return true;
}

/**
 * callback of keypad driver, called in ISR
 */
static void prvKeypadCallback(keyMap_t key, keyState_t evt, void *p)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->last_key = key;
    d->last_key_state = evt;
    d->keypad_pending = true;
	OSI_LOGI(0, "prvKeypadCallback--key:%d,state:%d",key,evt);
}

/**
 * keypad device read_cb
 */
static bool prvLvKeypadRead(lv_indev_drv_t *kp, lv_indev_data_t *data)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    uint32_t critical = osiEnterCritical();
    keyMap_t last_key = d->last_key;
    keyState_t last_key_state = d->last_key_state;
    bool keypad_pending = d->keypad_pending;
    d->keypad_pending = false;
    osiExitCritical(critical);

    gTick ++;

    if (keypad_pending)
    {
        data->state = (last_key_state & KEY_STATE_RELEASE) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
        data->key = 0xff;
        for (unsigned n = 0; n < OSI_ARRAY_SIZE(gLvKeyMap); n++)
        {
            if (gLvKeyMap[n].keypad == last_key)
            {
                data->key = gLvKeyMap[n].lv_key;
                break;
            }
        }

        OSI_LOGD(0, "prvLvKeypadRead--key:%x,state:%x",data->key,data->state);
        osiEvent_t ev;
        ev.id = EV_USER_KEY; 
        ev.param1= data->key;
        ev.param2= data->state;
        osiEventSend(d->thread, &ev);

        if(data->key == LV_KEY_POWER){
            lvGuiScreenOn();
        }
        /*
        if(data->key == LV_KEY_NEXT || data->key == LV_KEY_PREV)
            gotKey = true;
        */
    }

    // no more to be read
    return false;
}

/**
 * initialize keypad device
 */
static bool prvLvInitKeypad(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    lv_indev_drv_t kp_drv;
    lv_indev_drv_init(&kp_drv);
    kp_drv.type = LV_INDEV_TYPE_KEYPAD;
    kp_drv.read_cb = prvLvKeypadRead;
	OSI_LOGI(0, "1=prvLvInitKeypad");
    d->keypad = lv_indev_drv_register(&kp_drv); // pointer copy
    drvKeypadInit(); // permit multiple calls
    drvKeypadSetCB(prvKeypadCallback, KEY_STATE_PRESS | KEY_STATE_RELEASE, NULL);
    return true;
}

/**
 * run littlevgl task handler
 */
static void prvLvTaskHandler(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    lv_task_handler();

    unsigned next_run = lv_task_get_tick_next_run();
    osiTimerStartRelaxed(d->task_timer, next_run, OSI_WAIT_FOREVER);

    lv_refr_now(d->disp);
}

/**
 * whether inactive timeout
 */
static bool prvIsInactiveTimeout(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (d->screen_on_users != 0)
        return false;
    if (d->inactive_timeout == 0)
        return false;
    if (!d->anim_inactive && lv_anim_count_running())
        return false;
    return lv_disp_get_inactive_time(d->disp) > d->inactive_timeout;
}

/**
 * gui thread entry
 */
static void prvLvThreadEntry(void *param)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->thread = osiThreadCurrent();
    d->task_timer = osiTimerCreate(d->thread, (osiCallback_t)prvLvTaskHandler, NULL);
    osiThread_t * eng = MG_regEventThread(d->thread);

    lv_init();
    prvLvInitLcd();
    if(d->keypad_pending)
        prvLvInitKeypad();

    lvGuiCreate_t create = (lvGuiCreate_t)param;
    if(create != NULL)
        create();

    osiTimerStart(d->task_timer, 0);
    lv_disp_trig_activity(d->disp);

    for (;;)
    {
        osiEvent_t event;
        event.id = OSI_EVENT_ID_NONE;

        osiEventTryWait(d->thread, &event,500);
		if(d->screen_on)
        	mg_update_ui();
        if (event.id == OSI_EVENT_ID_QUIT)
            break;
        else if(event.id == EV_USER_UART_CTL){
            OSI_LOGD(0, "got uart event");
            osiEvent_t evt;
            evt.id = EV_USER_UART_RESP;
            evt.param1 = event.param1;
            evt.param2 = event.param2;
            evt.param3 = event.param3;
            osiEventSend(eng, &evt);
        }else if(event.id == EV_USER_KEY){
            if(mg_proc_key(event.param1,event.param2)){
                OSI_LOGD(0, "processed by ui");
            }
        }		
		else if(event.id == EV_SCREEN_OFF)
		{
			osiTimerStop(d->task_timer);
			lvGuiScreenOff();
		}
		else if(event.id == EV_SCREEN_ON)
		{
			osiTimerStart(d->task_timer, 0);
			lvGuiScreenOn();
		}
		if (d->screen_on && prvIsInactiveTimeout())
        {
            //lvGuiScreenOff();
        }    
    }
    MG_unregEventThread(d->thread);
    osiThreadExit();
}

/**
 * start gui based on littlevgl
 */
void lvGuiInit(lvGuiCreate_t create,bool haskey)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    d->screen_on = true;
    d->keypad_pending = haskey;
    d->anim_inactive = false;
    d->last_key = 0xff;
    d->last_key_state = KEY_STATE_RELEASE;
    d->screen_on_users = 0;
    d->inactive_timeout = CONFIG_LV_GUI_SCREEN_OFF_TIMEOUT;

    d->thread = osiThreadCreate("lvgl", prvLvThreadEntry, create,
                                OSI_PRIORITY_NORMAL,
                                CONFIG_LV_GUI_THREAD_STACK_SIZE,
                                CONFIG_LV_GUI_THREAD_EVENT_COUNT);
}

/**
 * get littlevgl gui thread
 */
osiThread_t *lvGuiGetThread(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    return d->thread;
}

/**
 * get keypad input device
 */
lv_indev_t *lvGuiGetKeyPad(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    return d->keypad;
}

/**
 * schedule a callback to be executed in gui thread
 */
void lvGuiThreadCallback(osiCallback_t cb, void *param)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    osiThreadCallback(d->thread, cb, param);
}

/**
 * send event to gui thread
 */
void lvGuiSendEvent(const osiEvent_t *evt)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    osiEventSend(d->thread, evt);
}

/**
 * request screen on
 */
bool lvGuiRequestSceenOn(uint8_t id)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users |= mask;
    return true;
}

/**
 * release screen on request
 */
bool lvGuiReleaseScreenOn(uint8_t id)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (id > 31)
        return false;

    unsigned mask = (1 << id);
    d->screen_on_users &= ~mask;
    return true;
}

/**
 * turn off screen
 */
void lvGuiScreenOff(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (!d->screen_on)
        return;

    OSI_LOGI(0, "screen off");
    drvLcdSetBackLightEnable(d->lcd, false);
    drvLcdSleep(d->lcd);
    d->screen_on = false;
}

/**
 * turn on screen
 */
void lvGuiScreenOn(void)
{
    lvGuiContext_t *d = &gLvGuiCtx;

    if (d->screen_on)
        return;

    OSI_LOGI(0, "screen on");
    drvLcdWakeup(d->lcd);
    prvDispForceFlush();
    drvLcdSetBackLightEnable(d->lcd, true);
    d->screen_on = true;
}

/**
 * set screen off timeout at inactive
 */
void lvGuiSetInactiveTimeout(unsigned timeout)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->inactive_timeout = timeout;
}

/**
 * set whether animation is regarded as inactive
 */
void lvGuiSetAnimationInactive(bool inactive)
{
    lvGuiContext_t *d = &gLvGuiCtx;
    d->anim_inactive = inactive;
}
