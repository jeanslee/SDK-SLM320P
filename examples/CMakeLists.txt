# Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

if(CONFIG_APPIMG_LOAD_FLASH)

    
    set(target mg_sysinfo_flash)
    add_appimg(${target} ${flash_ldscript}  mg_sysinfo.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )

    set(target mg_cmdforward_flash)
    add_appimg(${target} ${flash_ldscript}  mg_cmd_forward.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )

	
    set(target mg_aes_flash)
    add_appimg(${target} ${flash_ldscript}  mg_aes.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_aes2_flash)
    add_appimg(${target} ${flash_ldscript}  mg_aes2.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_adc_flash)
    add_appimg(${target} ${flash_ldscript}  mg_adc.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_audio_flash)
    add_appimg(${target} ${flash_ldscript}  mg_audio.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_event_flash)
    add_appimg(${target} ${flash_ldscript}  mg_event.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_file_flash)
    add_appimg(${target} ${flash_ldscript}  mg_file.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_keypad_flash)
    add_appimg(${target} ${flash_ldscript}  mg_keypad.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_lcd_flash)
    add_appimg(${target} ${flash_ldscript}  mg_lcd.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )

    set(target mg_baolan_lcd_flash)
    add_appimg(${target} ${flash_ldscript}  mg_baolan_lcd.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
    
    set(target mg_messagequeue_flash)
    add_appimg(${target} ${flash_ldscript}  mg_messagequeue.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_notify_flash)
    add_appimg(${target} ${flash_ldscript}  mg_notify.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_pipe_flash)
    add_appimg(${target} ${flash_ldscript}  mg_pipe.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_semaphore_flash)
    add_appimg(${target} ${flash_ldscript}  mg_semaphore.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_sim_flash)
    add_appimg(${target} ${flash_ldscript}  mg_sim.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_sock_flash)
    add_appimg(${target} ${flash_ldscript}  mg_sock.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_systime_flash)
    add_appimg(${target} ${flash_ldscript}  mg_systime.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_virt_at_flash)
    add_appimg(${target} ${flash_ldscript}  mg_virt_at.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_wifi_flash)
    add_appimg(${target} ${flash_ldscript}  mg_wifi.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target hello_world_flash)
    add_appimg(${target} ${flash_ldscript}  hello_world.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_call_flash)
    add_appimg(${target} ${flash_ldscript}  mg_call.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_fifo_flash)
    add_appimg(${target} ${flash_ldscript}  mg_fifo.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_gpio_flash)
    add_appimg(${target} ${flash_ldscript}  mg_gpio.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_lbs_flash)
    add_appimg(${target} ${flash_ldscript}  mg_lbs.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_mem_flash)
    add_appimg(${target} ${flash_ldscript}  mg_mem.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_mutex_flash)
    add_appimg(${target} ${flash_ldscript}  mg_mutex.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_nw_flash)
    add_appimg(${target} ${flash_ldscript}  mg_nw.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_recorder_flash)
    add_appimg(${target} ${flash_ldscript}  mg_recorder.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_sms_flash)
    add_appimg(${target} ${flash_ldscript}  mg_sms.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_tone_flash)
    add_appimg(${target} ${flash_ldscript}  mg_tone.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_uart_flash)
    add_appimg(${target} ${flash_ldscript}  mg_uart.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_volte_flash)
    add_appimg(${target} ${flash_ldscript}  mg_volte.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_sys_example_flash)
    add_appimg(${target} ${flash_ldscript}  mg_sys_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_mqtt_example_flash)
    add_appimg(${target} ${flash_ldscript}  mg_mqtt_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
	set(target mg_example_mqttE)
	add_appimg(${target} ${flash_ldscript}  mg_mqttE_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )

    set(target mg_bt_example_flash)
    add_appimg(${target} ${flash_ldscript}  mg_bt_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_cus_tone_flash)
    add_appimg(${target} ${flash_ldscript}  mg_cus_tone.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_at_device_flash)
    add_appimg(${target} ${flash_ldscript}  mg_at_device.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_vib_flash)
    add_appimg(${target} ${flash_ldscript}  mg_vib.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_at_gpio_flash)
    add_appimg(${target} ${flash_ldscript}  mg_at_gpio.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_at_uart_flash)
    add_appimg(${target} ${flash_ldscript}  mg_at_uart.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_at_leds_flash)
    add_appimg(${target} ${flash_ldscript}  mg_at_leds.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
    set(target mg_battery_flash)
    add_appimg(${target} ${flash_ldscript}  mg_battery.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
	
if(CONFIG_GPS_CHIP_TD1030)
    set(target mg_gps_example_flash)
    add_appimg(${target} ${flash_ldscript}  mg_gps_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
endif()

    set(target mg_aurecord_example_flash)
    add_appimg(${target} ${flash_ldscript}  mg_aurecord_example.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )

if(CONFIG_TTS_SUPPORT)
    set(target mg_tts_flash)
    add_appimg(${target} ${flash_ldscript}  mg_tts.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
endif()

set(target mg_bl_flash)
    add_appimg(${target} ${flash_ldscript}  mg_bl.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} lvgl)

if(CONFIG_WATCH_DOG_SUPPORT)
    set(target mg_watchdog_flash)
    add_appimg(${target} ${flash_ldscript}  mg_watchdog.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} )
endif()
	
#
# the default example pac is mg_gui_flash.pac
#	
    set(target mg_gui_flash)
    add_appimg(${target} ${flash_ldscript}  mg_gui.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} lvgl)
	
# 
# the following code is to generate the .pac file , you copy the following code to where you want to generate .pac file. 
# here is to generate the mg_gui_flash.pac 
# 

    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.img)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-image -i APPIMG -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
                -p ${target_img_file}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py} ${target_img_file}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})

endif()




