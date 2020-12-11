project(simid)
set(target simid)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)
add_library(${target} STATIC ${DIR_SRCS})

#add_library(${target} STATIC ${dummy_c_file})
#MESSAGE(STATUS "dummy_c_file folder: " ${dummy_c_file})
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
#target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_LVGL)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SRC_DIR} inc src)
#target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})

target_sources(${target} PRIVATE 
	src/simid.c 
	src/simidplatform.c
	)