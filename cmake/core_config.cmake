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

# Auto generated. Don't edit it manually!

set(CROSS_COMPILE arm-none-eabi-)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(ARCH arm)

set(CONFIG_SOC 8910)
set(CONFIG_SOC_8910 on)

set(CONFIG_CPU_ARM on)
set(CONFIG_CPU_ARMV7A on)
set(CONFIG_CPU_ARM_CA5 on)

set(CONFIG_NOR_PHY_ADDRESS 0x60000000)
set(CONFIG_RAM_PHY_ADDRESS 0x80000000)

set(CONFIG_APPIMG_LOAD_FLASH on)
set(CONFIG_APPIMG_LOAD_FILE on)
set(CONFIG_APPIMG_LOAD_FILE_NAME /appload.img)

set(CONFIG_APPIMG_FLASH_ADDRESS 0x602b0000)
set(CONFIG_APPIMG_FLASH_SIZE 0x100000)
set(CONFIG_APP_FLASHIMG_RAM_OFFSET 0xe80000)
set(CONFIG_APP_FLASHIMG_RAM_SIZE 0x100000)
set(CONFIG_APP_FILEIMG_RAM_OFFSET 0xf80000)
set(CONFIG_APP_FILEIMG_RAM_SIZE 0x40000)

set(CONFIG_FDL1_IMAGE_START 0x838000)
set(CONFIG_FDL1_IMAGE_SIZE 0x8000)
set(CONFIG_FDL2_IMAGE_START 0x810000)
set(CONFIG_FDL2_IMAGE_SIZE 0x28000)

set(CONFIG_GPS_CHIP_TD1030 on)
set(CONFIG_TTS_SUPPORT on)
set(CONFIG_WATCH_DOG_SUPPORT off)
