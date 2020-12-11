
project(AppDemo)

set(APP_DEMO_PATH ${SOURCE_TOP_DIR}/app/demo)

include_directories(${APP_DEMO_PATH}/include)
include_directories(${APP_DEMO_PATH}/lib/include)
include_directories(${APP_DEMO_PATH}/oem/include)
include_directories(${APP_DEMO_PATH}/src)

set(EX_LIB ${APP_DEMO_PATH}/lib/libMathAdd.a ${APP_DEMO_PATH}/lib/libMathSub.a)

set(OEM
	${APP_DEMO_PATH}/oem/OEM_audio_demo.c
	${APP_DEMO_PATH}/oem/OEM_gpio.c
	${APP_DEMO_PATH}/oem/OEM_keypad_demo.c
	${APP_DEMO_PATH}/oem/OEM_lcd.c
	${APP_DEMO_PATH}/oem/OEM_nw_tcp_demo.c
	${APP_DEMO_PATH}/oem/OEM_usb_serial_demo.c
)


 set(target_file main)
 set(target_file_path ${APP_DEMO_PATH}/src)
 
if(CONFIG_APPIMG_LOAD_FLASH) 
 set(target ${target_file}_flash)
 
 add_appimg(${target} ${flash_ldscript} ${target_file_path}/${target_file}.c ${OEM})
 
 target_link_libraries(${target} PRIVATE ${EX_LIB})
 target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
 
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















