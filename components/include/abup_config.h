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

#ifndef _ABUP_CONFIG_H_
#define _ABUP_CONFIG_H_

// Auto generated. Don't edit it manually!

#define LOG_TAG_ABUP OSI_MAKE_LOG_TAG('A', 'B', 'U', 'P')

/**
 * Whether ABUP FOTA is supported
 */
#define CONFIG_ABUP_SUPPORT

/**
 * Wheth the dummy ABUP engine is enabled.
 *
 * When a real ABUP engine is used, it must be disabled.
 */
#define CONFIG_ABUP_DUMMY_SUPPORT

/**
 * ABUP thread stack size in bytes
 */
/* #undef CONFIG_ABUP_STACK_SIZE */

/**
 * ABUP output PCM sample rate
 */
/* #undef CONFIG_ABUP_PCM_SAMPLE_RATE */

#endif
