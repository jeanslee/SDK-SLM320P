/**
 * @file demo.h
 *
 */

#ifndef MEIG_H 
#define MEIG_H 

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
//#include "lv_ex_conf.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
    typedef enum{
        PAGE_INVALID=-1,
        PAGE_HOME=0,
        PAGE_GROUP, 
        PAGE_SETTING, 
        PAGE_CGROUP, 
        PAGE_CIT, 
        PAGE_MAX,
    }PAGE;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a demo application
 */
void mg_demo_create(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DEMO_H*/
