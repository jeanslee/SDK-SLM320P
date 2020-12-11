#add_definitions(-DADPT_DEBUG)
set(ADPT_DEBUG 1)
MESSAGE(STATUS "ADPT_DEBUG: " ${ADPT_DEBUG})
add_compile_options(-DADPT_DEBUG=${ADPT_DEBUG})

##begin define 
#add_definitions(-DSSNB_BUILD_LIB)
#add_definitions(-DPRODUCT_TYPE)
set(PRODUCT_TYPE \"$ENV{PRODUCT_TYPE}\")
MESSAGE(STATUS "PRODUCT_TYPE: " ${PRODUCT_TYPE})
add_compile_options(-DPRODUCT_TYPE=${PRODUCT_TYPE})

#add_definitions(-DFIRMWARE_VERSION)
set(FIRMWARE_VERSION \"$ENV{FIRMWARE_VERSION}\")
MESSAGE(STATUS "FIRMWARE_VERSION: " ${FIRMWARE_VERSION})
add_compile_options(-DFIRMWARE_VERSION=${FIRMWARE_VERSION})

#add_definitions(-DMQTT_PRODUCT_KEY)
set(MQTT_PRODUCT_KEY \"$ENV{MQTT_PRODUCT_KEY}\")
MESSAGE(STATUS "MQTT_PRODUCT_KEY: " ${MQTT_PRODUCT_KEY})
add_compile_options(-DMQTT_PRODUCT_KEY=${MQTT_PRODUCT_KEY})

#add_definitions(-DMQTT_PRODUCT_SECRET)
set(MQTT_PRODUCT_SECRET \"$ENV{MQTT_PRODUCT_SECRET}\")
MESSAGE(STATUS "MQTT_PRODUCT_SECRET: " ${MQTT_PRODUCT_SECRET})
add_compile_options(-DMQTT_PRODUCT_SECRET=${MQTT_PRODUCT_SECRET})

#add_definitions(-DTY_CLOUD_MQTT_SERVER)
set(TY_CLOUD_MQTT_SERVER \"$ENV{TY_CLOUD_MQTT_SERVER}\")
MESSAGE(STATUS "TY_CLOUD_MQTT_SERVER: " ${TY_CLOUD_MQTT_SERVER})
add_compile_options(-DTY_CLOUD_MQTT_SERVER=${TY_CLOUD_MQTT_SERVER})

#add_definitions(-DTY_CLOUD_PORT)
set(TY_CLOUD_PORT $ENV{TY_CLOUD_PORT})
MESSAGE(STATUS "TY_CLOUD_PORT: " ${TY_CLOUD_PORT})
add_compile_options(-DTY_CLOUD_PORT=${TY_CLOUD_PORT})


#add_definitions(-DTY_CLOUD_HTTP_DEVREG_URL)
set(TY_CLOUD_HTTP_DEVREG_URL \"$ENV{TY_CLOUD_HTTP_DEVREG_URL}\")
MESSAGE(STATUS "TY_CLOUD_HTTP_DEVREG_URL: " ${TY_CLOUD_HTTP_DEVREG_URL})
add_compile_options(-DTY_CLOUD_HTTP_DEVREG_URL=${TY_CLOUD_HTTP_DEVREG_URL})

#add_definitions(-DTY_MEMLEAK_CHECK)
set(TY_MEMLEAK_CHECK $ENV{TY_MEMLEAK_CHECK})
MESSAGE(STATUS "TY_MEMLEAK_CHECK: " ${TY_MEMLEAK_CHECK})
add_compile_options(-DMEMLEAK_CHECK=${TY_MEMLEAK_CHECK})
###end define 

#for link lib esim

#add_definitions(-DTY_ESIM_MODE)
set(TY_ESIM_MODE $ENV{TY_ESIM_MODE})
MESSAGE(STATUS "TY_ESIM_MODE: " ${TY_ESIM_MODE})
add_compile_options(-DTY_ESIM_MODE=${TY_ESIM_MODE})
###end define 


set(MEM_CHECK "")
if(${TY_MEMLEAK_CHECK} STREQUAL 1)

MESSAGE(STATUS "do mem check")

elseif(${TY_MEMLEAK_CHECK} STREQUAL 0 )
set(MEM_CHECK "_nomemchk")
MESSAGE(STATUS "no mem check")
endif()

if(${TY_ESIM_MODE} STREQUAL "STD")
set(TY_ESIM_LIB libtyesimstd${MEM_CHECK}.a)
MESSAGE(STATUS "link ty esim std library")

elseif(${TY_ESIM_MODE} STREQUAL "PRI" )
set(TY_ESIM_LIB libtyesim${MEM_CHECK}.a)
MESSAGE(STATUS "link ty esim library")
else()
MESSAGE(FATAL_ERROR  "no set env TY_ESIM_MODE esim mode option for \"STD\" or PRIVATE \"PRI\"")
endif()

#for all log
set(TY_LOG_ENABLE $ENV{TY_LOG_ENABLE})
MESSAGE(STATUS "TY_LOG_ENABLE: " ${TY_LOG_ENABLE})
add_compile_options(-DTY_LOG_ENABLE=${TY_LOG_ENABLE})

#config for print log  by  at channel
set(AT_LOG_PRINT $ENV{AT_LOG_PRINT})
if(${AT_LOG_PRINT} STREQUAL 1)
MESSAGE(STATUS "AT_LOG_PRINT: " ${AT_LOG_PRINT})
add_compile_options(-DAT_LOG_PRINT)
endif()


MESSAGE(STATUS "link ty esim library =" ${TY_ESIM_LIB})

add_definitions(-DTY_ESIM_PROFILE)
#if(${TY_MEMLEAK_CHECK} STREQUAL 1)
#set(TY_ESIM_LIB "libtyesim.a")
#MESSAGE(STATUS "do mem check")
#elseif(${TY_MEMLEAK_CHECK} STREQUAL 0 )
#set(TY_ESIM_LIB "libtyesim_nomemchk.a")
#MESSAGE(STATUS "no mem check")
#endif()

## http source 
file(GLOB CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/*.c")
file(GLOB UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/utils/*.c")
file(GLOB SYSDEP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/sysdep/*.c")
file(GLOB aiot_port_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/portfiles/aiot_port/*.c")
file(GLOB http_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/http/src/*.c")
#
file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/app/*.c")
file(GLOB CLOUD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/cloud/*.c")
file(GLOB utils_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/utils/*.c")
if(CONFIG_APPIMG_LOAD_FLASH)

    set(target app_main_flash_$ENV{PRODUCT_TYPE}_$ENV{FIRMWARE_VERSION})
    add_appimg(${target} ${flash_ldscript}  
		app_main.c 
		ty_esim_at.c 
		ty_fota_at.c 
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_os_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/rtos_api.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_sc_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_flash_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_socket_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_ssl_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_mqtt_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_fota_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_at_adpt.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/mqtt_test.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/sslsock_test.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/tcp_test.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/sc_channel_test.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/ty_ss_at.c
		
		
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTT_port.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTClient.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTConnectClient.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTConnectServer.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTDeserializePublish.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTFormat.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTPacket.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTSerializePublish.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTSubscribeClient.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTSubscribeServer.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTUnsubscribeClient.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/MQTTUnsubscribeServer.c
		

		${CORE_SOURCES}
		${UTILS_SOURCES}
		${SYSDEP_SOURCES}
		${aiot_port_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/slm320/slm320_http_adpt.c
	#	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/http/src/http_parser.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/http/src/http_string.c
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test/http_test.c
		
		${APP_SOURCES}
		${CLOUD_SOURCES}
		${utils_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/os/ty_devinfo.c
		)
	#$ENV{PRODUCT_TYPE} 
	#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR folder: " $ENV{PRODUCT_TYPE} )
	target_include_directories(${target} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../simid/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/include
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/plat/test
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/mqtt/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/utils/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/core/sysdep/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/linksdk/LinkSDK/portfiles/aiot_port/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/protocol/http/include/
	
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/utils/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/include/
	${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/lib/include/
	) 
	target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name} ${CMAKE_CURRENT_SOURCE_DIR}/../ty_ssnb/lib/${TY_ESIM_LIB} )
	#target_link_libraries(${target} PRIVATE ${out_lib_dir}/libsimid.a ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
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