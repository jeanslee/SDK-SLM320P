#ifndef _TY_ENCODING_UTIL_H_
#define _TY_ENCODING_UTIL_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "json_util.h"
#include "time.h"
#include "hex_util.h"
#include "des.h"
#include "ty_util.h"
#include "ty_os_adpt.h"


int encodeRequest(char* appKey, char* srcData, char** dest);

int decodeResponse(char* appKey, char*srcData, char** dest);


#if defined(__cplusplus)
}
#endif
#endif