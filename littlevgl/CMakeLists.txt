# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

#configure_file(include/lv_gui_config.h.in ${BINARY_TOP_DIR}/include/lv_gui_config.h)
project(lvgl)
set(target lvgl)
# add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_LVGL)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SRC_DIR} lvgl include)
#target_include_targets(${target} PRIVATE kernel fs driver nvm)
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})

target_sources(${target} PRIVATE
    lvgl/src/lv_core/lv_group.c
    lvgl/src/lv_core/lv_indev.c
    lvgl/src/lv_core/lv_disp.c
    lvgl/src/lv_core/lv_obj.c
    lvgl/src/lv_core/lv_refr.c
    lvgl/src/lv_core/lv_style.c
    lvgl/src/lv_core/lv_debug.c
    lvgl/src/lv_draw/lv_draw_basic.c
    lvgl/src/lv_draw/lv_draw.c
    lvgl/src/lv_draw/lv_draw_rect.c
    lvgl/src/lv_draw/lv_draw_label.c
    lvgl/src/lv_draw/lv_draw_line.c
    lvgl/src/lv_draw/lv_draw_img.c
    lvgl/src/lv_draw/lv_draw_arc.c
    lvgl/src/lv_draw/lv_draw_triangle.c
    lvgl/src/lv_draw/lv_img_decoder.c
    lvgl/src/lv_draw/lv_img_cache.c
    lvgl/src/lv_font/lv_font.c
    lvgl/src/lv_font/lv_font_fmt_txt.c
    lvgl/src/lv_font/lv_font_roboto_12.c
    lvgl/src/lv_font/lv_font_roboto_16.c
    lvgl/src/lv_font/lv_font_roboto_22.c
    lvgl/src/lv_font/lv_font_roboto_28.c
    lvgl/src/lv_font/lv_font_unscii_8.c
    lvgl/src/lv_hal/lv_hal_disp.c
    lvgl/src/lv_hal/lv_hal_indev.c
    lvgl/src/lv_hal/lv_hal_tick.c
    lvgl/src/lv_misc/lv_circ.c
    lvgl/src/lv_misc/lv_area.c
    lvgl/src/lv_misc/lv_task.c
    lvgl/src/lv_misc/lv_fs.c
    lvgl/src/lv_misc/lv_anim.c
    lvgl/src/lv_misc/lv_mem.c
    lvgl/src/lv_misc/lv_ll.c
    lvgl/src/lv_misc/lv_color.c
    lvgl/src/lv_misc/lv_txt.c
    lvgl/src/lv_misc/lv_math.c
    lvgl/src/lv_misc/lv_log.c
    lvgl/src/lv_misc/lv_gc.c
    lvgl/src/lv_misc/lv_utils.c
    lvgl/src/lv_misc/lv_async.c
    lvgl/src/lv_misc/lv_printf.c
    lvgl/src/lv_misc/lv_bidi.c
    lvgl/src/lv_objx/lv_arc.c
    lvgl/src/lv_objx/lv_bar.c
    lvgl/src/lv_objx/lv_cb.c
    lvgl/src/lv_objx/lv_cpicker.c
    lvgl/src/lv_objx/lv_ddlist.c
    lvgl/src/lv_objx/lv_kb.c
    lvgl/src/lv_objx/lv_line.c
    lvgl/src/lv_objx/lv_mbox.c
    lvgl/src/lv_objx/lv_preload.c
    lvgl/src/lv_objx/lv_roller.c
    lvgl/src/lv_objx/lv_table.c
    lvgl/src/lv_objx/lv_tabview.c
    lvgl/src/lv_objx/lv_tileview.c
    lvgl/src/lv_objx/lv_btn.c
    lvgl/src/lv_objx/lv_calendar.c
    lvgl/src/lv_objx/lv_chart.c
    lvgl/src/lv_objx/lv_canvas.c
    lvgl/src/lv_objx/lv_gauge.c
    lvgl/src/lv_objx/lv_label.c
    lvgl/src/lv_objx/lv_list.c
    lvgl/src/lv_objx/lv_slider.c
    lvgl/src/lv_objx/lv_ta.c
    lvgl/src/lv_objx/lv_spinbox.c
    lvgl/src/lv_objx/lv_btnm.c
    lvgl/src/lv_objx/lv_cont.c
    lvgl/src/lv_objx/lv_img.c
    lvgl/src/lv_objx/lv_imgbtn.c
    lvgl/src/lv_objx/lv_led.c
    lvgl/src/lv_objx/lv_lmeter.c
    lvgl/src/lv_objx/lv_page.c
    lvgl/src/lv_objx/lv_sw.c
    lvgl/src/lv_objx/lv_win.c
    lvgl/src/lv_themes/lv_theme_alien.c
    lvgl/src/lv_themes/lv_theme.c
    lvgl/src/lv_themes/lv_theme_default.c
    lvgl/src/lv_themes/lv_theme_night.c
    lvgl/src/lv_themes/lv_theme_templ.c
    lvgl/src/lv_themes/lv_theme_zen.c
    lvgl/src/lv_themes/lv_theme_material.c
    lvgl/src/lv_themes/lv_theme_nemo.c
    lvgl/src/lv_themes/lv_theme_mono.c
)

if(CONFIG_SOC_8910)
    target_sources(${target} PRIVATE
        lv_port/lv_gui_main.c
        lv_port/lv_gui_example1.c
        lv_port/lv_gui_example2.c
        lv_port/lv_gui_bl.c
        lv_port/meig/cjk.c 
        lv_port/meig/png_group_icon.c 
        lv_port/meig/png_back.c 
        lv_port/meig/png_group_title.c 
        lv_port/meig/png_setting.c 
        lv_port/meig/png_member.c 
        lv_port/meig/png_call_left.c 
        lv_port/meig/png_call.c 
       lv_port/meig/png_ok.c 
        lv_port/meig/png_group.c 
        lv_port/meig/png_call_right.c 
        lv_port/meig/meig.h 
       lv_port/meig/png_battery_10.c 
        lv_port/meig/png_battery_20.c 
        lv_port/meig/png_battery_30.c 
        lv_port/meig/png_battery_40.c 
        lv_port/meig/png_battery_50.c 
        lv_port/meig/png_battery_60.c 
       lv_port/meig/png_battery_70.c 
        lv_port/meig/png_battery_80.c 
        lv_port/meig/png_battery_90.c 
       lv_port/meig/png_battery_100.c 
        lv_port/meig/png_signal_none.c 
        lv_port/meig/png_signal.c 
        lv_port/meig/png_signal_3.c 
        lv_port/meig/png_signal_2.c 
        lv_port/meig/png_signal_1.c 
        lv_port/meig/meig.c
    )
endif()
