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

#ifndef _DRV_KEYPAD_H_
#define _DRV_KEYPAD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
extern uint8_t keyid;
/**
 * \brief keypad logic definition
 */
typedef enum
{
    KEY_MAP_POWER = 0, ///< power key
    KEY_MAP_0,         ///< '0'
    KEY_MAP_1,         ///< '1'
    KEY_MAP_2,         ///< '2'
    KEY_MAP_3,         ///< '3'
    KEY_MAP_4,         ///< '4'
    KEY_MAP_5,         ///< '5'
    KEY_MAP_6,         ///< '6'
    KEY_MAP_7,         ///< '7'
    KEY_MAP_8,         ///< '8'
    KEY_MAP_9,         ///< '9'
    KEY_MAP_STAR,      ///< '*'
    KEY_MAP_SHARP,     ///< '#'
    KEY_MAP_LEFT,      ///< left
    KEY_MAP_RIGHT,     ///< right
    KEY_MAP_UP,        ///< up
    KEY_MAP_DOWN,      ///< down
    KEY_MAP_OK,        ///< ok
    KEY_MAP_CANCEL,    ///< cancel
    KEY_MAP_SOFT_L,    ///< left soft key
    KEY_MAP_SOFT_R,    ///< rigfht soft key
    KEY_MAP_SIM1,      ///< customized
    KEY_MAP_SIM2,      ///< customized
    KEY_MAP_VOLUME_UP,
    KEY_MAP_VOLUME_DOWN,
    KEY_MAP_WELL,
    KEY_MAP_SLEEP,
    KEY_MONEY,
    KEY_MAP_SCAN,
    KEY_BACK,
    KEY_MAP_PLUS,
    KEY_MAP_DOT,
    KEY_MAP_STORE,
    KEY_MAP_MENU,
    KEY_MAP_ESC,
    KEY_MAP_MAX_COUNT  ///< total count
} keyMap_t;

/**
 * @brief the key state
 */
typedef enum
{
    KEY_STATE_PRESS = (1 << 0),
    KEY_STATE_RELEASE = (1 << 1),
} keyState_t;

/**
 * @brief function type, key event handler
 */
typedef void (*keyEventCb_t)(keyMap_t key, keyState_t evt, void *p);

/**
 * @brief init the keypad driver
 */
void drvKeypadInit();
void MG_drvKeypadInit();
/**
 * @brief set key event handler
 *
 * @param evtmsk    the event mask caller care about
 * @param cb        the handler
 * @param ctx       caller context
 */
void drvKeypadSetCB(keyEventCb_t cb, uint32_t mask, void *ctx);
void MG_drvKeypadSetCB(keyEventCb_t cb, uint32_t mask, void *ctx);
/**
 * @brief get key current state
 *
 * @param key   the key indicator
 * @return
 *      - (-1)              fail
 *      - KEY_STATE_PRESS   the key is pressed
 *      - KEY_STATE_RELEASE the key is released
 */
int drvKeypadState(keyMap_t key);
int MG_drvKeypadState(keyMap_t key);
keyMap_t MG_getKeyID(uint8_t row, uint8_t col);

/**
 * @brief Get all press keys code from register,values can not more than 10 .
 * Provided for bbat autotest use.
 *
 * @param key      array pointer
 * @param count    variable addr 
 * @return
 *      - (-1)              fail
 *      1                   pass
 */

int bbatGetAllScanKeyId(uint8_t *key, int *count);

#ifdef __cplusplus
}
#endif

#endif
