if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/plat/CMakeLists.txt)
   add_subdirectory(plat)
endif()

if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/protocol/CMakeLists.txt)
   add_subdirectory(protocol)
endif()

if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/app/CMakeLists.txt)
   add_subdirectory(app)
endif()

if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/cloud/CMakeLists.txt)
   add_subdirectory(cloud)
endif()

if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/plat/CMakeLists.txt)
   add_subdirectory(utils)
endif()

if(EXISTS ${SOURCE_TOP_DIR}/ty_ssnb/os/CMakeLists.txt)
   add_subdirectory(os)
endif()

   
   
   
   
  