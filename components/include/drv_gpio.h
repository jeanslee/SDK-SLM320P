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

#ifndef _DRV_GPIO_H_
#define _DRV_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct drvGpio;
#define GPIO_RI_LOW_TIME 300

/**
 * opaque data struct for GPIO instance
 */
typedef struct drvGpio drvGpio_t;

/**
 * GPIO interrupt callback
 *
 * It will be called when GPIO interrupt occurs. Due to it is executed in
 * ISR, the callback should follow ISR programming guides.
 */
typedef void (*drvGpioIntrCB_t)(void *ctx);

typedef enum
{
    DRV_GPIO_INPUT,
    DRV_GPIO_OUTPUT
} drvGpioMode_t;

/**
 * GPIO configuration
 */
typedef struct
{
	uint8_t	id;
	uint8_t inout;
	uint8_t level;
} Gpio_t;

/**
 * GPIO configuration
 */
typedef struct
{
    drvGpioMode_t mode; ///< GPIO mode
    bool out_level;     ///< level to be set for GPIO output
    bool intr_enabled;  ///< interrupt enabled, only for GPIO input
    bool intr_level;    ///< true for level interrupt, false for edge interrupt
    bool debounce;      ///< debounce enabled
    bool rising;        ///< rising edge or level high interrupt enabled
    bool falling;       ///< falling edge or level low interrupt enabled
} drvGpioConfig_t;

/**
 * @brief GPIO module initialization
 *
 * It just initialize GPIO module, and won't touch any GPIO. It should be
 * called before any \a drvGpioOpen.
 */
void drvGpioInit(void);

/**
 * @brief open a GPIO
 *
 * IOMUX will be set to GPIO mode at open.
 *
 * Each GPIO sould be opened only once. When it is already opened, this
 * API will return NULL.
 *
 * If the specified GPIO can't support the specified mode, this API will
 * return NULL.
 *
 * If the specified GPIO can't support input interrupt, and interrupt is
 * enabled, this API will return NULL.
 *
 * The returned instance is dynamic allocated, caller should free it after
 * \a drvGpioClose is called.
 *
 * @param id    GPIO id. The range may be different among chips.
 * @param cfg   GPIO configuration
 * @param cb    callback at interrupt
 * @param cb_ctx    context pointer for callback
 * @return
 *      - GPIO instance pointer
 *      - NULL if parameter is invalid
 */
drvGpio_t *drvGpioOpen(uint32_t id, drvGpioConfig_t *cfg, drvGpioIntrCB_t cb, void *cb_ctx);

/**
 * @brief close a GPIO
 *
 * IOMUX will be kept, and the GPIO will be set to input mode.
 *
 * @param d     GPIO instance
 */
void drvGpioClose(drvGpio_t *d);

/**
 * @brief reconfigure the opened GPIO
 *
 * When the configuration is invalid, the current configuration will
 * be kept.
 *
 * @param d     GPIO instance
 * @param cfg   GPIO configuration
 * @return
 *      - true if GPIO configuration is valid
 *      - false if GPIO configuration is invalid
 */
bool drvGpioReconfig(drvGpio_t *d, drvGpioConfig_t *cfg);

/**
 * @brief reconfigure the opened GPIO
 *
 * When the configuration is invalid, the current configuration will
 * be kept.
 *
 * @param d     GPIO instance
 * @param cfg   GPIO configuration
 * @return
 *      - true if GPIO configuration is valid
 *      - false if GPIO configuration is invalid
 */
bool MG_drvGpioReconfig(drvGpio_t *d, drvGpioConfig_t *cfg);


/**
 * @brief read the level of GPIO
 *
 * It can be called for both GPIO input and output. For GPIO output,
 * it is the level set by software.
 *
 * @param d     GPIO instance
 * @return
 *      - true for level high
 *      - false for level low
 */
bool drvGpioRead(drvGpio_t *d);

/**
 * @brief write level for GPIO
 *
 * When it is called for GPIO input, it will do nothing.
 *
 * When it is called for GPIO output, set the output level.
 *
 * @param d     GPIO instance
 * @param level GPIO level to be set,
 *              - true for level high
 *              - false for level low
 */
void drvGpioWrite(drvGpio_t *d, bool level);

/**
 * @brief open a GPIO
 *
 * @param id           GPIO id, there are 32 GPIOs totally
 * @param cfg          #include <drv_gpio.h> GPIO configuration
 * @param cb           GPIO interrupt callback
 * @param cb_ctx       context pointer for callback
 *
 * @return
 *      - GPIO instance pointer
 *      - -1 if parameter is invalid
 */
drvGpio_t *MG_gpio_open(uint32_t id, drvGpioConfig_t *cfg, drvGpioIntrCB_t cb, void *cb_ctx);

/**
 * @brief Write level for GPIO
 *
 * @param drvG         GPIO instance
 * @param level        GPIO level to be set, true for level high or false for level low
 *
 * @return
 *      - 1 if success
 *      - 0 if fail
 */
int MG_gpio_set(drvGpio_t *d, bool level);

/**
 * @brief Read the level of GPIO
 *
 * @param drvG         GPIO instance
 *
 * @return
 *      - 1 if the level of GPIO is high
 *      - 0 if the level of GPIO is low
 */
int MG_gpio_get(drvGpio_t *d);
 
/**
 * @brief Close a GPIO
 *
 * @param drvG         GPIO instance
 */
void MG_gpio_close(drvGpio_t *d);

bool MG_gpio_outRI(uint32_t utime);
void drvGpioPulseCtrl(drvGpio_t *d, uint8_t egde_num);
void mayi_uart_flow_ctrl_on(bool on);
#if defined(CONFIG_BOARD_MG_MODULE_NAME_MAYI)
void MG_wakeup_x1520_ctrl(bool on);
#endif
#ifdef __cplusplus
}
#endif
#endif
