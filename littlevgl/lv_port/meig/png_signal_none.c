#include "lvgl.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_PNG_SIGNAL_NONE
#define LV_ATTRIBUTE_IMG_PNG_SIGNAL_NONE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_PNG_SIGNAL_NONE uint8_t png_signal_none_map[] = {
  0xbc, 0xbe, 0xbc, 0xff, 	/*Color of index 0*/
  0xfc, 0xfe, 0xfc, 0xff, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 15*/

  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 
  0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x11, 0x11, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x11, 0x11, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x11, 0x11, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 
};

const lv_img_dsc_t png_signal_none = {
  .header.always_zero = 0,
  .header.w = 18,
  .header.h = 14,
  .data_size = 190,
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .data = png_signal_none_map,
};

