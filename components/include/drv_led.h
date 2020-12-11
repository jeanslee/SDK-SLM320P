/* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _DRV_LED_H_
#define _DRV_LED_H_
#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN
#define R_LED 1
	#define R_LED_CUR 3 /*0~63*/
#define G_LED 2
	#define G_LED_CUR 5 /*0~63*/
#define B_LED 4
	#define B_LED_CUR 4 /*0~63*/
#define RG_LED 3
	#define RG_LED_CUR 3 /*0~63*/
#define RGB_LED 7
	#define RGB_LED_CUR 3 /*0~63*/
#define TORCH 0x20
	#define TORCH_LED_CUR 15 /*0~15*/

typedef struct
{
    uint8_t color; // API
    uint8_t current;
	uint8_t onoff;
} Leds_t;

typedef enum
{
    LED_NET_STATUS, /*output:Net status led*/
    LED_SYS_STATUS,  /*output:Module status led*/
    LED_NET_MODE, /*output:Net mode led*/
    LED_NET_SLEEP_IN, /*output:system sleep status led*/
    LED_DUMMY
} ledId_t;

typedef enum
{
    LED_CTRL_GPIO_19=19,
    LED_CTRL_GPIO_18=18,
    LED_CTRL_GPIO_13=13,
    LED_CTRL_GPIO_8=8
} led_gpio_t;

typedef struct
{
    bool on;
    ledId_t id;
    uint8_t mode;     
} drvLedCtrl_t;

void ledCtrl(ledId_t id,bool on);
void ledInit(ledId_t id);
void rgbLedCtrl(uint8_t id,bool on);

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/

OSI_EXTERN_C_END

#endif //_DRV_LED_H
