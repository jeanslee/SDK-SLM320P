
#ifndef _MG_GPS_API_H_
#define _MG_GPS_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MG_LONGITUDE_STR_SIZE 24
#define MG_LATITUDE_STR_SIZE 24

#define MG_GPS_MODE_DEFAULT 0
#define MG_GPS_MODE_TIME 1

#define MG_GPS_POS_TYPE_STANDALON 0
#define MG_GPS_POS_TYPE_AGPS 1

#define MG_GPS_GNSS_TYPE_BDS 1
#define MG_GPS_GNSS_TYPE_GPS 2
#define MG_GPS_GNSS_TYPE_BDS_GPS 3

#define GPS_POS_FAILURE 0
#define GPS_POS_SUCCESS 1

#define GPS_CLOSE_MODE 0
#define GPS_CLOSE_ALL 1

#ifdef __cplusplus
extern "C" {
#endif

int MG_GetLbsLocation(uint8_t *lng_str,uint8_t *lat_str);
uint8_t MG_GpsGetEphVaildTime(void);
int MG_GpsOpen (uint8_t mode,uint8_t pos_type,uint8_t gnss_type,void *cb, uint16_t time_s);
int MG_GpsClose(uint8_t type);

#ifdef __cplusplus
}
#endif

#endif



