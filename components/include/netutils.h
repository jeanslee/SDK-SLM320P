#ifndef _NETUTILS_H_
#define _NETUTILS_H_

#include "lwip/netif.h"

struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
#if IP_NAT
struct netif *getPPPNetIf(uint8_t nSim, uint8_t nCid);
struct netif *getGprsWanNetIf(uint8_t nSim, uint8_t nCid);
#endif
struct netif *getEtherNetIf(uint8_t nCid);
struct netif *ip4_src_route(const ip4_addr_t *dst, const ip4_addr_t *src);
bool getSimIccid(uint8_t simId, uint8_t *simiccid, uint8_t *len);
bool getSimImei(uint8_t simId, uint8_t *imei, uint8_t *len);
bool getSimImsi(uint8_t simId, uint8_t *imsi, uint8_t *len);


bool MG_getFirmwareVersion(uint8_t*ver,int*len);
bool MG_getHardwareVersion(uint8_t*ver,int*len);
bool MG_getManufactureInfo(uint8_t*buf,int*len);
bool MG_getModelInfo(uint8_t*buf,int*len);
bool MG_NwGetMccMnc(uint8_t*mnc,uint8_t*mcc);

void MG_NwGetPdpContextList(CFW_GPRS_PDPCONT_INFO_V2 *pdp,uint8_t *len,uint8_t nSim);
#endif
