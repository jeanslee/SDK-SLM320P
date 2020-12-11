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

#include "lv_gui_main.h"
#include "drv_lcd_v2.h"
#include "drv_names.h"
#include "lvgl.h"
#include "osi_api.h"
#include "osi_log.h"
#include <string.h>
#include <stdlib.h>


LV_FONT_DECLARE(cjk)        /*Declare a font*/

static lv_obj_t * gPage;

static void setTheme(void)
{
    lv_theme_t *th = lv_theme_material_init(210, &cjk);
    lv_theme_set_current(th);

    lv_obj_t *scr = lv_disp_get_scr_act(NULL); /*Get the current screen*/
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    gPage = lv_page_create(scr,NULL);
    lv_obj_set_size(gPage,hres,vres);
    lv_page_set_sb_mode(gPage,LV_SB_MODE_OFF);
}

void bl_init(void)
{
    OSI_LOGI(0, "bl init start");
    drvLcd_t *lcd = drvLcdGetByname(DRV_NAME_LCD1);
    drvLcdOpenV2(lcd);
    drvLcdFill(lcd, 0, NULL, true);
    drvLcdSetBackLightEnable(lcd, true);
    lvGuiInit(setTheme,false);
}
void show_text(char*text,int x,int y){

    /*
    lv_obj_t * btn = lv_btn_create(scr,NULL);
    lv_obj_set_pos(btn,x,y);
    */
    lv_obj_t *label = lv_label_create(gPage,NULL);
    lv_obj_set_pos(label,x,y);
    lv_label_set_text(label,text);

}
void clear_screen(){
    lv_obj_t *scr = lv_disp_get_scr_act(NULL); /*Get the current screen*/
    lv_obj_clean(scr);
}
