
#ifndef _MG_SYS_API_H_
#define _MG_SYS_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cfw.h"

#define SN_MIN_LENGTH  24
#define IEMI_MIN_LENGTH  16
#define NORMAL_REBOOT 0
#define WTD_REBOOT 1
#define EXT_RESET_REBOOT 2
#define FORCE_REBOOT 3
#define UNKNOW_REBOOT 4

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
                        EDIT HISTORY

  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.

When             Who              What, Where, Why
----------   ----------   ---------------------------------------------------
2020-09-23   zhangxu         modify  	MG_SetFlightMode  
							move	MG_GetFlightMode
								


2020-08-01   zhangxu         Added  	MG_cfwBcdToDialString  
								MG_smsNumberStr
								MG_smsNodeDataStr

2020-08-24  zhangxu         Added    MG_Get_FlashID
							MG_Get_CpuID
							MG_SendMessage_to_AT_Serial

2020-******                        Created initial version.
===========================================================================*/




/**
 * \brief read SN
 *
 * \param sn     output buffer for store SN
 * \param size   The size of buffer
 * \return
 *      - the length of SN
 *      - -1 on invalid parameter
 */
int MG_SysReadSN(uint8_t *sn, uint8_t size);

/**
 * \brief read Imei
 *
 * \param Imei   output buffer for store Imei
 * \param size   The size of buffer
 * \return
 *      - the length of Imei
 *      - -1 on invalid parameter
 */
int MG_SysReadImei(uint8_t *imei,uint8_t size);

/**
 * \brief read RF calibration flag
 *
 * \return
 *      - true if RF calibration success
 *      - false if RF calibration failure
 */
bool MG_SysReadRfCali(void);

/**
 * \brief get battery capacity
 *
 * \param bcl  battery capacity pointer    
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysBattGetRemainCap(uint8_t *bcl);

/**
 * \brief get battery voltage
 *
 * \param bcv  battery voltage pointer    
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysBattGetVol(uint32_t *bcv);


/**
 * \brief get Changer Status
 *
 * \param bcs  Changer Status pointer    
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysChangerGetStatus(uint8_t *bcs);

/**
 * \brief get module compile time
 *
 * \param time  output buffer for store module compile time
 * \param len   buffer length of module compile time,return the real length of module compile time
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysGetModuleCompileTime(uint8_t *time,int *len);
/**
 * \brief get module type
 *
 * \param type  output buffer for store module type
 * \param len   buffer length of module type,return the real length of module type
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysGetModuleType(uint8_t *type,int *len);
/**
 * \brief get module Version
 *
 * \param ver  output buffer for store module Version
 * \param len   buffer length of module Version,return the real length of module Version
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysGetModuleVersion(uint8_t *ver,int *len);
/**
 * \brief Power Off
 *  
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysPowerOff(void);

/**
 * \brief restart
 *  
 * \return
 *      - true on success
 *      - false on failure
 */
bool MG_SysReboot(void);

/**
 * \brief get time zone
 *  
 * \return
 *      - time zone
 */
int MG_SysGetTimeZone(void);

/**
 * \brief set time zone
 *  
 * \param offset  time zone
 */
void MG_SysSetTimeZone(int offset);




int MG_cfwBcdToDialString(const void *src, size_t size, void *dst);

void MG_smsNumberStr(const uint8_t *bcd, unsigned size, unsigned type, char *s);

char * MG_smsNodeDataStr(uint8_t type, CFW_SMS_INFO_UNION *info, uint8_t nSim);

unsigned int MG_Get_FlashID(void);

unsigned int MG_Get_CpuID(void);

void MG_SendMessage_to_AT_Serial(const char *text,int length);


int MG_nvmReadStationName(int index, void *nvmName, int size,int *  iTestSign, int * iItem);

int MG_nvmWriteStationName(int index, const void *nvmName, int size,int  iTestSign, int iItem);

int MG_nvmReadSN2(void *nvmSn, int size);


int MG_nvmWriteSN2(const void *nvmSn, int size);

bool MG_SetFlightMode(uint8_t nSim,uint8_t fun);

int MG_GetFlightMode(uint8_t nSim);

int MG_GetRebootCause(void);

#ifdef __cplusplus
}
#endif

#endif



