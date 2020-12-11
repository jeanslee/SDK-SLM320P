
#ifndef _MG_NET_API_H_
#define _MG_NET_API_H_

#include<stdint.h>
#include "cfw.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CurrentCellInfo
{
    uint8_t mnc[3];
    uint8_t mcc[3]; 
    uint16_t lac;
    uint32_t cid;
    int16_t rsrp;
    int16_t rsrq;
}CurrentCellInfo;

typedef struct _NeighborCellInfo
{
    uint8_t nCellNum;
    uint16_t cellId[10];
    uint16_t lac[10];
}NeighborCellInfo;

/* \brief The CFW_EventName function get eventname by event id
 * \param [in] id  EVENT ID
 * \return EventName
*/
const char *MG_cfwEventName(uint32_t id);

/* \brief retrieves the state of the GPRS Service according to the selected cid
 * \param [in] nCid    Specify the PDP Context Identifier (a numeric) which specifies
 *              a particular PDP context definition.
 * \param [out] pState    Point to uint8_t to hold the state of GPRS activation.
 *              - \a CFW_GPRS_DEACTIVED (0)
 *              - \a CFW_GPRS_ACTIVED (1)
 * \param [in] nSimID  SIM ID
 * \return \a ERR_SUCCESS on success, or error code
 */
uint32_t MG_cfwGetGprsActState(uint8_t *pState, CFW_SIM_ID nSimID);

/* \brief retrieves the state of the GPRS service
 * \param [out] pState    Point to uint8_t to hold the state of GPRS attachment.
 *              - \a CFW_GPRS_DEACTIVED (0)
 *              - \a CFW_GPRS_ACTIVED (1)
 * \param [in] nSimID  SIM ID
 * \return \a ERR_SUCCESS on success, or error code
 */
uint32_t MG_cfwGetGprsAttState(uint8_t *pState, CFW_SIM_ID nSimID);

/* \brief get network rat
 * \param nSimID [in]    SIM ID
 * \return network rat
 */
uint8_t MG_cfwNWGetRat(CFW_SIM_ID nSimID);

/* \brief set pdp context
 * \param [in] nCid      Cid
 * \param [in] pdp_cont  Pdp Context
 * \return 0    Set Pdp Context success
 *         -1    Set Pdp Context failure
 */
uint32_t MG_cfwGprsSetPdpCxt(uint8_t nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID);

/* \brief get Plmn
 * \param PlmnInfo [out]    Plmn info
 * \param nSim [in]         SIM ID
 * \return 0    get Plmn success
 *         -1   get Plmn failure
 */
int MG_getPlmn(uint8_t nSim, char *PlmnInfo);

/* \brief get Csq
 * \param nSim [in]          SIM ID
 * \param nSignalLevel [out] rssi range is (0-31,99)   
 * \param nBitError [out]    channel bit error rate
 * \return 0    get Csq success
 *         -1   get Csq failure
 */
int MG_getCsq(uint8_t nSim, uint8_t *nSignalLevel, uint8_t *nBitError);

/* \brief get local IPV4 address
 * \param nSim [in]         SIM ID
 * \param addr [out]        local ipv4 addr string
 * \return 0    get address success
 *         -1   get address failed 
 */
int MG_getLocalAddr(uint8_t nSim, char *addr);

bool MG_NwGetSignalQuality(uint8_t*rat,uint8_t*siglevel,uint8_t*biterr);

uint8_t MG_cfwGetSimStatus(uint8_t nSimID);

bool MG_getSimImei(uint8_t simId, uint8_t *imei,int *len);

bool MG_getSimImsi(uint8_t simId, uint8_t *imsi,int *len);

bool MG_getSimIccid(uint8_t simId, uint8_t *iccid,int *len);

bool MG_NwModifyPdpContext(uint8_t nSim,uint8_t cid,uint8_t pdptype,const char*apn);

bool MG_NwActivePdpContext(uint8_t nSim,uint8_t uCid);

bool MG_NwDeactivePdpContext(uint8_t sim,uint8_t cid);

struct netif *MG_NwGetGprsNetIf(uint8_t nSim, uint8_t nCid);

bool MG_NwGetCurrentCellInfo(CurrentCellInfo *cellinf);

bool MG_NwGetNeighborCellInfo(NeighborCellInfo *cellinf, uint32_t timeout);

uint32_t MG_VoiceCallAnswer(uint8_t sim);

uint32_t MG_VoiceCallHangup(uint8_t sim);

uint32_t MG_VoiceCallDial(uint8_t sim, const char *pDialNum);

uint32_t MG_CFW_GprsGetstatus(CFW_NW_STATUS_INFO *pStatus, CFW_SIM_ID nSimID);

uint8_t MG_CFW_GprsGetDefaultPdnInfo(CFW_GPRS_PDPCONT_DFTPDN_INFO *PdnInfo, CFW_SIM_ID nSimID);

uint32_t MG_CFW_GetGprsAttState(uint8_t *pState,CFW_SIM_ID nSimID);

/*
 * \brief active or deactive pdp connect
 * \param nSim [in]     SIM ID
 * \param nCid [in]     Specify the PDP Context Identifier (a numeric) which specifies
 *                      a particular PDP context definition.Range is (1-7).
 *                      if nCid is 0, then default active/deactive all cid if possible.
 * \param nState [in]   the state of GPRS PDP activation.
 *                      - \a CFW_GPRS_DEACTIVED (0)
 *                      - \a CFW_GPRS_ACTIVED (1)
 * \param timeout [in]  the wait time when failed.
 *
 * \return 0    execute success
 *         -1   execute failure
 *         -2   param invalid
 *         -3   wait time out
 */
int MG_NwGprsPdpAct(uint8_t nSim, uint8_t nCid, uint8_t nState, uint32_t timeout);

#endif