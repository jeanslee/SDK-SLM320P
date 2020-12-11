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

#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#include "hal_config.h"

// Auto generated. Don't edit it manually!

/**
 * whether enable sysnv mechanism
 */
#define CONFIG_KERNEL_SYSNV_ENABLED

/**
 * RTOS tick Hz
 */
#define CONFIG_KERNEL_TICK_HZ 50

/**
 * Default hardware tick frequency
 */
#define CONFIG_KERNEL_HWTICK_FREQ 2000000

/**
 * RTOS tick relaxed time
 *
 * When it is 0, \p osiThreadSleep, \p osiMutexTryLock and etc. will wakeup
 * system at sleep. Otherwise, these APIs will wakeup system with relaxed
 * timeout to save power consumption.
 */
#define CONFIG_KERNEL_OSTICK_RELAXED_TIME  0

/**
 * whether \p OSI_ASSERT will panic
 */
#define CONFIG_KERNEL_ASSERT_ENABLED

/**
 * system minimum utc time in second
 *
 * \p CONFIG_KERNEL_MIN_UTC_SECOND and \p CONFIG_KERNEL_MAX_UTC_SECOND are
 * rough check of epoch time.
 *
 * \code
 * $ date -u -d "@946684800"
 * Sat Jan  1 00:00:00 UTC 2000
 * $ date -u -d "@4102444800"
 * Fri Jan  1 00:00:00 UTC 2100
 * \endcode
 */
#define CONFIG_KERNEL_MIN_UTC_SECOND 946684800LL

/**
 * system maximum utc time in second
 */
#define CONFIG_KERNEL_MAX_UTC_SECOND 4102444800LL

/**
 * whether global work queue with OSI_PRIORITY_HIGH enabled
 */
#define CONFIG_KERNEL_HIGH_PRIO_WQ_ENABLED

/**
 * whether global work queue with OSI_PRIORITY_LOW enabled
 */
#define CONFIG_KERNEL_LOW_PRIO_WQ_ENABLED

/**
 * whether global work queue for async file write enabled
 */
#define CONFIG_KERNEL_FILE_WRITE_WQ_ENABLED

/**
 * stack size (in bytes) of global high priority work queue
 */
#define CONFIG_KERNEL_HIGH_PRIO_WQ_STACKSIZE 4096

/**
 * stack size (in bytes) of global low priority work queue
 */
#define CONFIG_KERNEL_LOW_PRIO_WQ_STACKSIZE 4096

/**
 * stack size (in bytes) of global async file write work queue
 */
#define CONFIG_KERNEL_FILE_WRITE_WQ_STACKSIZE 2048

/**
 * stack size (in bytes) of timer work queue
 */
#define CONFIG_KERNEL_TIMER_WQ_STACKSIZE 4096

/**
 * minimum periodic timer period in milliseconds
 */
#define CONFIG_KERNEL_PERIODIC_TIMER_MIN_PERIOD 10

/**
 * use 32KHz tick as profile clock (8910)
 */
/* #undef CONFIG_KERNEL_PROFILE_CLOCK32K */

/**
 * use 2mHz tick as profile clock (8910)
 */
#define CONFIG_KERNEL_PROFILE_CLOCK2M

/**
 * profile buffer size in bytes
 */
#define CONFIG_KERNEL_PROFILE_BUF_SIZE 0xc000

/**
 * use host packet, flow id 0x97 for log
 */
#define CONFIG_KERNEL_HOST_TRACE

/**
 * use host packet, flow id 0x98 for log
 */
/* #undef CONFIG_KERNEL_DIAG_TRACE */

/**
 * disable traceid forcedly
 */
/* #undef CONFIG_KERNEL_DISABLE_TRACEID */

/**
 * size in bytes for each log buffer
 */
#define CONFIG_KERNEL_TRACE_BUF_SIZE 0x2000

/**
 * log buffer count
 */
#define CONFIG_KERNEL_TRACE_BUF_COUNT 4

/**
 * external log buffer count
 */
#define CONFIG_KERNEL_TRACE_EXTERNAL_BUF_COUNT 8

/**
 * count of memory allocate/free records
 */
#define CONFIG_KERNEL_MEM_RECORD_COUNT 1024

/**
 * Maximum blue screen handler count
 */
#define CONFIG_KERNEL_BLUE_SCREEN_HANDLER_COUNT 8

/**
 * whether to enable watchdog
 *
 * By default, watchdog will be enabled when \p BUILD_RELEASE_TYPE is
 * "release".
 */
/* #undef CONFIG_WDT_ENABLE */
#define CONFIG_BOARD_MG_MODULE_NAME_SLM320P
/* #undef CONFIG_BOARD_MG_MODULE_NAME_SLM320PE */
/* #undef CONFIG_BOARD_MG_MODULE_NAME_SLM320PWY */

#endif
