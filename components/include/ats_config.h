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

#ifndef _ATS_CONFIG_H_
#define _ATS_CONFIG_H_

#include "srv_config.h"
#include "tts_config.h"

// Auto generated. Don't edit it manually!

#define GMI_ID "MeiG"
#define GMM_ID "SLM320P-debug"
#define GMR_ID "SLM320P_20201110_V32_T14"
#define SGSW_ID ""

#define FW_REL_DATE_ID	""
#define FW_REL_TIME_ID	""
#define HW_VER_ID		"HardwareVersion:V1.00"
#define HW_REL_DATE_ID	"160320"
			
/**
 * Whether to support coap AT commands
 */
#define CONFIG_AT_COAP_SUPPORT

/**
 * Whether to support http AT commands
 */
#define CONFIG_AT_HTTP_SUPPORT

/**
 * Whether to support alic AT commands
 */
#define CONFIG_AT_ALIC_SUPPORT

/**
 * Whether to support ftp client AT commands
 */
/* #undef CONFIG_AT_FTP_SUPPORT */

/**
  * Whether to support (refactored) ftp client AT commands
 */
#define CONFIG_AT_NEWFTP_SUPPORT

/**
 * Whether to support MYNET AT commands
 */
#define CONFIG_AT_MYNET_TCPIP_SUPPORT

/**
 * Whether to support MYINFO AT commands
 */
#define CONFIG_AT_MYINFO_SUPPORT

/**
 * Whether to support +IPR=<n>&W
 *
 * It doesn't conform to specification, but it is needed in many cases.
 */
#define CONFIG_AT_IPR_SUPPORT_ANDW

/**
 * Whether to support CISSDK AT commands
 */
#define CONFIG_AT_CISSDK_MIPL_SUPPORT

/**
 * Whether to support softsim AT commands
 */
/* #undef CONFIG_AT_SSIM_SUPPORT */

/**
 * whether to support onenet DM AT command
 */
#define CONFIG_AT_DM_LWM2M_SUPPORT

/**
 * whether to support oceanconnect AT commands
 */
#define CONFIG_AT_OCEANCONNECT_SUPPORT

/**
 * whether to support MQTT (based on lwIP) AT commands
 */
/* #undef CONFIG_AT_LWIP_MQTT_SUPPORT */

/**
 * whether to support MQTT (based on paho) AT commands
 */
#define CONFIG_AT_PAHO_MQTT_SUPPORT

/**
 * whether to support AT commands for memory size
 */
#define CONFIG_AT_EMMCDDRSIZE_SUPPORT

/**
 * whether to support audio AT commands
 */
#define CONFIG_AT_AUDIO_SUPPORT

/**
 * whether to use global APN
 *
 * When enabled, global/large APN information will be embedded. Otherwise,
 * local/small APN information will be embedded.
 */
#define CONFIG_AT_GLOBAL_APN_SUPPORT

/**
 * whether some bt test at command can be used
 */
#define CONFIG_AT_SPBTTEST_SUPPORT

/**
 * whether some ble test at command can be used
 */
#define CONFIG_AT_SPBLETEST_SUPPORT

/**
 * whether some ble test at command can be used
 */
#define CONFIG_AT_SPBLE_SUPPORT

/**
 * whether some bt basic at command can be used
 */
/* #undef CONFIG_AT_BT_CLASSIC_SUPPORT */

/**
 * whether some btcomm at command can be used
 */
#define CONFIG_AT_BTCOMM_SUPPORT

/**
 * whether some wifi basic at command can be used
 */
#define CONFIG_AT_WIFISCAN_SUPPORT

/**
 * whether wifi sensitivity test command can be used
 */
/* #undef CONFIG_AT_WIFI_SENSITIVITY_TEST_SUPPORT */

/**
 * whether AT ABUP command can be used
 */
#define CONFIG_MEIG_ABUP_SLM320P_CUSTOM

/**
 * whether AT ABUP command can be used
 */
/* #undef CONFIG_MEIG_ABUP_SLM320_CUSTOM */

#endif
