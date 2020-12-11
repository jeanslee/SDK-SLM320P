#include "lvgl.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_PNG_CALL
#define LV_ATTRIBUTE_IMG_PNG_CALL
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_PNG_CALL uint8_t png_call_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0xdf, 0xdf, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0xff, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0xbf, 0xff, 0xdf, 0x9f, 0x9f, 0xbf, 0xdf, 0xdf, 0xbf, 0x9f, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xbf, 0x9f, 0xbf, 0xff, 0xbf, 0x9f, 0xbf, 0xbf, 0xbf, 0xff, 0xbf, 0x9f, 0xff, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xbf, 0x9f, 0x9f, 0xff, 0xdf, 0x9f, 0xdf, 0xff, 0xbf, 0x9f, 0xff, 0x9f, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xdf, 0x9f, 0xff, 0xff, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xbf, 0xdf, 0x9f, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0x9f, 0xff, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0xdf, 0xbf, 0xbf, 0x9f, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0x9f, 0xff, 0xff, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0x9f, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xdf, 0x9f, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0xff, 0xdf, 0x9f, 0xff, 0xff, 0xff, 0x9f, 0xbf, 0xff, 0xdf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0xff, 0xff, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xdf, 0xbf, 0xbf, 0xdf, 0xff, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
  0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xfa, 0x8e, 0x1b, 0x9f, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0x3c, 0xaf, 0x9e, 0xd7, 0x9e, 0xd7, 0xbe, 0xdf, 0x7d, 0xcf, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xfb, 0x9e, 0xff, 0xff, 0xff, 0xff, 0x7d, 0xc7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xda, 0x8e, 0xda, 0x86, 0xda, 0x86, 0xbe, 0xdf, 0xbf, 0xe7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xdf, 0xf7, 0xbe, 0xdf, 0x1b, 0x9f, 0xff, 0xff, 0x7d, 0xc7, 0xda, 0x86, 0xda, 0x86, 0x3c, 0xa7, 0x7d, 0xc7, 0x9e, 0xd7, 0x3c, 0xaf, 0xda, 0x86, 0x9e, 0xdf, 0x9d, 0xcf, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xff, 0xff, 0x1b, 0x9f, 0xda, 0x86, 0x3c, 0xaf, 0xff, 0xff, 0x1b, 0x9f, 0xda, 0x86, 0x1b, 0x9f, 0x1b, 0xa7, 0x1b, 0x9f, 0xdf, 0xf7, 0x3c, 0xaf, 0xda, 0x86, 0xbe, 0xe7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xff, 0xff, 0xfb, 0x9e, 0xda, 0x86, 0xda, 0x86, 0xff, 0xff, 0x5c, 0xb7, 0xda, 0x86, 0x5c, 0xbf, 0xbe, 0xdf, 0x3c, 0xaf, 0xfa, 0x8e, 0xbe, 0xdf, 0xda, 0x86, 0x7d, 0xc7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xff, 0xff, 0x5c, 0xbf, 0xda, 0x86, 0xbe, 0xe7, 0xdf, 0xf7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xdf, 0xef, 0x1b, 0x9f, 0x9e, 0xd7, 0xfa, 0x8e, 0x9e, 0xd7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0x9e, 0xd7, 0xdf, 0xe7, 0xda, 0x86, 0xff, 0xff, 0x1b, 0xa7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0x5c, 0xbf, 0x1b, 0xa7, 0x3c, 0xaf, 0xfa, 0x8e, 0x3c, 0xb7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xfb, 0x96, 0xff, 0xff, 0xfa, 0x8e, 0xbe, 0xe7, 0xbe, 0xdf, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xbe, 0xdf, 0xdf, 0xef, 0xda, 0x86, 0xff, 0xff, 0x7d, 0xc7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0x3c, 0xa7, 0xfb, 0x96, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xff, 0xff, 0x7d, 0xc7, 0xda, 0x8e, 0xff, 0xff, 0x9e, 0xdf, 0x3c, 0xa7, 0xbe, 0xe7, 0xff, 0xff, 0xff, 0xff, 0x5d, 0xbf, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xfb, 0x96, 0xff, 0xff, 0x7d, 0xc7, 0xda, 0x8e, 0xbe, 0xdf, 0xff, 0xff, 0xbe, 0xe7, 0xfa, 0x8e, 0x3c, 0xaf, 0xff, 0xff, 0x7d, 0xcf, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xfb, 0x9e, 0xff, 0xff, 0xbe, 0xe7, 0xfb, 0x96, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xff, 0xff, 0x1b, 0xa7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xbe, 0xdf, 0xff, 0xff, 0xbe, 0xe7, 0x5d, 0xbf, 0x1b, 0x9f, 0x1b, 0xa7, 0x7d, 0xcf, 0xff, 0xff, 0x1b, 0x9f, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0x1b, 0x9f, 0xbe, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xf7, 0x1b, 0xa7, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
  0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit BUT the 2 bytes are swapped*/
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xfa, 0x9f, 0x1b, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xaf, 0x3c, 0xd7, 0x9e, 0xd7, 0x9e, 0xdf, 0xbe, 0xcf, 0x7d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x9e, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xc7, 0x7d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xda, 0x8e, 0xda, 0x86, 0xda, 0x86, 0xda, 0xdf, 0xbe, 0xe7, 0xbf, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xf7, 0xdf, 0xdf, 0xbe, 0x9f, 0x1b, 0xff, 0xff, 0xc7, 0x7d, 0x86, 0xda, 0x86, 0xda, 0xa7, 0x3c, 0xc7, 0x7d, 0xd7, 0x9e, 0xaf, 0x3c, 0x86, 0xda, 0xdf, 0x9e, 0xcf, 0x9d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xff, 0xff, 0x9f, 0x1b, 0x86, 0xda, 0xaf, 0x3c, 0xff, 0xff, 0x9f, 0x1b, 0x86, 0xda, 0x9f, 0x1b, 0xa7, 0x1b, 0x9f, 0x1b, 0xf7, 0xdf, 0xaf, 0x3c, 0x86, 0xda, 0xe7, 0xbe, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xff, 0xff, 0x9e, 0xfb, 0x86, 0xda, 0x86, 0xda, 0xff, 0xff, 0xb7, 0x5c, 0x86, 0xda, 0xbf, 0x5c, 0xdf, 0xbe, 0xaf, 0x3c, 0x8e, 0xfa, 0xdf, 0xbe, 0x86, 0xda, 0xc7, 0x7d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xff, 0xff, 0xbf, 0x5c, 0x86, 0xda, 0xe7, 0xbe, 0xf7, 0xdf, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xda, 0xef, 0xdf, 0x9f, 0x1b, 0xd7, 0x9e, 0x8e, 0xfa, 0xd7, 0x9e, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xd7, 0x9e, 0xe7, 0xdf, 0x86, 0xda, 0xff, 0xff, 0xa7, 0x1b, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xbf, 0x5c, 0xa7, 0x1b, 0xaf, 0x3c, 0x8e, 0xfa, 0xb7, 0x3c, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x96, 0xfb, 0xff, 0xff, 0x8e, 0xfa, 0xe7, 0xbe, 0xdf, 0xbe, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xdf, 0xbe, 0xef, 0xdf, 0x86, 0xda, 0xff, 0xff, 0xc7, 0x7d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xa7, 0x3c, 0x96, 0xfb, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xff, 0xff, 0xc7, 0x7d, 0x8e, 0xda, 0xff, 0xff, 0xdf, 0x9e, 0xa7, 0x3c, 0xe7, 0xbe, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x5d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x96, 0xfb, 0xff, 0xff, 0xc7, 0x7d, 0x8e, 0xda, 0xdf, 0xbe, 0xff, 0xff, 0xe7, 0xbe, 0x8e, 0xfa, 0xaf, 0x3c, 0xff, 0xff, 0xcf, 0x7d, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x9e, 0xfb, 0xff, 0xff, 0xe7, 0xbe, 0x96, 0xfb, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x8e, 0xda, 0xff, 0xff, 0xa7, 0x1b, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0xdf, 0xbe, 0xff, 0xff, 0xe7, 0xbe, 0xbf, 0x5d, 0x9f, 0x1b, 0xa7, 0x1b, 0xcf, 0x7d, 0xff, 0xff, 0x9f, 0x1b, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x9f, 0x1b, 0xdf, 0xbe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xdf, 0xa7, 0x1b, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
  0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 0x86, 0xda, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format: Fix 0xFF: 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd4, 0xdb, 0x8c, 0xff, 0xd9, 0xdf, 0x99, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xdf, 0xe4, 0xa9, 0xff, 0xed, 0xf0, 0xd0, 0xff, 0xed, 0xf0, 0xd0, 0xff, 0xf0, 0xf3, 0xd8, 0xff, 0xeb, 0xee, 0xca, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd7, 0xde, 0x95, 0xff, 0xfe, 0xfe, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe9, 0xed, 0xc4, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd3, 0xda, 0x88, 0xff, 0xd3, 0xda, 0x89, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd1, 0xd9, 0x83, 0xff, 0xf0, 0xf3, 0xd7, 0xff, 0xf5, 0xf6, 0xe3, 0xff, 0xd1, 0xd9, 0x83, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf9, 0xfa, 0xf0, 0xff, 0xf0, 0xf3, 0xd7, 0xff, 0xda, 0xe0, 0x9b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe6, 0xeb, 0xbd, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xdd, 0xe3, 0xa4, 0xff, 0xe7, 0xeb, 0xbe, 0xff, 0xed, 0xf0, 0xcf, 0xff, 0xdd, 0xe3, 0xa5, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf0, 0xf2, 0xd6, 0xff, 0xec, 0xef, 0xcb, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd8, 0xdf, 0x96, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xde, 0xe4, 0xa7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd8, 0xdf, 0x96, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xda, 0xe0, 0x9c, 0xff, 0xdb, 0xe1, 0x9f, 0xff, 0xd8, 0xdf, 0x97, 0xff, 0xf8, 0xf9, 0xed, 0xff, 0xdf, 0xe5, 0xab, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf3, 0xf5, 0xdf, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xfd, 0xfd, 0xf9, 0xff, 0xd8, 0xde, 0x95, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0xe7, 0xb1, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xe4, 0xe9, 0xb7, 0xff, 0xf2, 0xf4, 0xdc, 0xff, 0xde, 0xe4, 0xa7, 0xff, 0xd4, 0xdb, 0x8b, 0xff, 0xf2, 0xf4, 0xdb, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xe9, 0xed, 0xc4, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xfc, 0xfc, 0xf6, 0xff, 0xe4, 0xe9, 0xb7, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf3, 0xf5, 0xde, 0xff, 0xf9, 0xfa, 0xee, 0xff, 0xd0, 0xd8, 0x82, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd2, 0xda, 0x86, 0xff, 0xf7, 0xf9, 0xea, 0xff, 0xda, 0xe0, 0x9b, 0xff, 0xed, 0xf0, 0xcf, 0xff, 0xd4, 0xdb, 0x8c, 0xff, 0xed, 0xf0, 0xce, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xef, 0xf2, 0xd4, 0xff, 0xf5, 0xf7, 0xe4, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdb, 0xe1, 0x9e, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xe4, 0xe9, 0xb8, 0xff, 0xdb, 0xe1, 0xa0, 0xff, 0xdf, 0xe5, 0xab, 0xff, 0xd3, 0xdb, 0x89, 0xff, 0xe1, 0xe6, 0xad, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd5, 0xdc, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd3, 0xdb, 0x8a, 0xff, 0xf3, 0xf5, 0xde, 0xff, 0xf2, 0xf4, 0xdb, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf0, 0xf3, 0xd7, 0xff, 0xf7, 0xf9, 0xeb, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xeb, 0xbf, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xdd, 0xe3, 0xa4, 0xff, 0xd6, 0xdd, 0x90, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xeb, 0xbe, 0xff, 0xd2, 0xd9, 0x85, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf2, 0xd6, 0xff, 0xdd, 0xe3, 0xa4, 0xff, 0xf4, 0xf6, 0xe2, 0xff, 0xfe, 0xfe, 0xfd, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xe6, 0xea, 0xbb, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd7, 0xde, 0x93, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xeb, 0xbf, 0xff, 0xd2, 0xda, 0x86, 0xff, 0xf2, 0xf4, 0xdb, 0xff, 0xfb, 0xfc, 0xf5, 0xff, 0xf4, 0xf6, 0xe0, 0xff, 0xd4, 0xdb, 0x8a, 0xff, 0xdf, 0xe4, 0xa9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xee, 0xc9, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd8, 0xde, 0x96, 0xff, 0xfe, 0xfe, 0xfd, 0xff, 0xf4, 0xf6, 0xe2, 0xff, 0xd7, 0xde, 0x94, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd2, 0xda, 0x86, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdc, 0xe2, 0xa2, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xf0, 0xf3, 0xd8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf4, 0xf6, 0xe2, 0xff, 0xe5, 0xea, 0xba, 0xff, 0xd9, 0xe0, 0x9a, 0xff, 0xdb, 0xe1, 0x9d, 0xff, 0xea, 0xed, 0xc6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd9, 0xe0, 0x9a, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd8, 0xdf, 0x97, 0xff, 0xf0, 0xf3, 0xd8, 0xff, 0xfb, 0xfc, 0xf5, 0xff, 0xfc, 0xfd, 0xf7, 0xff, 0xfc, 0xfd, 0xf7, 0xff, 0xf9, 0xfa, 0xf0, 0xff, 0xdb, 0xe1, 0xa0, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
  0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 0xd0, 0xd8, 0x81, 0xff, 
#endif
};

const lv_img_dsc_t png_call = {
  .header.always_zero = 0,
  .header.w = 34,
  .header.h = 18,
  .data_size = 612 * LV_COLOR_SIZE / 8,
  .header.cf = LV_IMG_CF_TRUE_COLOR,
  .data = png_call_map,
};