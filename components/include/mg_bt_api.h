#include "bt_abs.h"

/**
 * ��ȡ��ǰ�����汾
 * @return �汾��Ϣ
*/
const char *MG_GetBTVersion(void);

/**
 * ��������������ʹ�ܣ�
 * @return ��������״̬
*/
// BT_STATUS MG_BT_Start(void);

/**
 * �ر�����
 * @return ��������״̬
*/
// BT_STATUS MG_BT_Stop(void);

/**
 * ��ȡ����״̬
 * @return ����ʹ��״̬
*/
// BOOLEAN MG_BT_GetState(void);

/**
 * �����豸�ɼ���
 * @param visible [in]�Ƿ�ɼ�
 * @return �����ɼ���״̬
*/
BT_STATUS MG_BT_SetVisibilityUni(BT_SCAN_E visible);

/**
 * ��ȡ�豸�ɼ���
 * @return �豸�ɼ���
*/
BT_SCAN_E MG_BT_GetVisibilityUni(void);

/**
 * ���ñ����豸����
 * @param name [in]Ҫ���õ�����
 * @return ���ý��
*/
BT_STATUS MG_BT_SetLocalName(const uint16 *name);

/**
 * ��ȡ�豸����
 * @param name [out]��ȡ������
 * @return ִ�н��
*/
BT_STATUS MG_BT_GetLocalName(uint16 *name);

/**
 * ����������ַ
 * @param addr [in]������ַ
 * @return ���ý��
*/
BT_STATUS MG_BT_SetBdAddr(BT_ADDRESS *addr);

/**
 * ��ȡ������ַ
 * @param addr [out]��ȡ��������ַ
*/
void MG_BT_GetBdAddr(BT_ADDRESS *addr);

/**
 * ������������ǰ��֧��SPP��
 * @param bd_addr [in]�Է���������ַ
 * @param spp_speed [in]�˿�����
 * @return ���ӽ��
*/
BT_STATUS MG_BT_SppConnect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);

/**
 * �Ͽ�SPP����
 * @return �Ͽ����
*/
BT_STATUS MG_BT_SppDisconnect(void);

/**
 * ��ȡ��������״̬
 * @param addr [in]������ַ
 * @return ��ȡ��״̬
*/
BOOLEAN MG_BT_SppConnectStatusUni(const BT_ADDRESS *addr);

/**
 * ���������豸
 * @param service_type [in]�豸����
 * @return ���ҽ��
*/
// BT_STATUS MG_BT_SearchDevice(uint32 service_type);

/**
 * ƥ�������豸
 * @param addr [in]������ַ
 * @return ƥ����
*/
BT_STATUS MG_BT_PairDevice(const BT_ADDRESS *addr);

/**
 * ��ѯƥ������������� BT_GetPairedDeviceCount(0xFFFF);
 * @param service_type [in]�豸����
 * @return ƥ�������
*/
int MG_BT_GetPairedDeviceCount(uint32 service_type);

/**
 * ��ȡƥ����豸��Ϣ
 * @param service_type [in]�豸����
 * @param index [in]�豸����
 * @param info [out]�豸��Ϣ
 * @return ��ȡ���
*/
BT_STATUS MG_BT_GetPairedDeviceInfo(uint32 service_type, int index, BT_DEVICE_INFO *info);

/**
 * �Ͽ�ƥ����豸
 * @param addr [in]�豸��ַ
 * @return �Ͽ����
*/
BT_STATUS MG_BT_RemovePairedDevice(const BT_ADDRESS *addr);


/*****************************BLE*****************************/
/**
 * ���ù�����ַ
 * @param addr [in]Ҫ���õĵ�ַ
 * @return �������ý��
*/
uint8 MG_BLE_SetPublicAddr(uint8 addr[6]);

/**
 * ��ȡ������ַ
 * @param addr [out]��ȡ�ĵ�ַ
*/
void MG_BLE_GetPublicAddr(uint8 *addr);

/**
 * ���������ַ
 * @param addr [in]Ҫ���õĵ�ַ
 * @return ���ý��
*/
uint8 MG_BLE_SetRandomAddr(uint8 addr[6]);

/**
 * ��ȡ�����ַ
 * @param addr [out]��ȡ�ĵ�ַ
*/
void MG_BLE_GetRandomAddr(uint8 *addr);

/**
 * ����豸��������
 * @param white_list_info [in]�豸�����Ϣ
 * @return ��ӽ��
*/
uint8 MG_BLE_AddWhiteList(st_white_list_info white_list_info);

/**
 * ��ѯ������
 * @param index [in]�豸����
 * @param white_list_info [out]��ȡ���豸��Ϣ
 * @return ��ѯ���
*/
uint8 MG_BLE_QuiryWhiteListInfo(uint8 index, st_white_list_info *white_list_info);

/**
 * �Ӱ������Ƴ��豸
 * @param addr [in]�豸������ַ
 * @return �Ƴ����
*/
uint8 MG_BLE_RemoveWhiteList(uint8 *addr);

/**
 * ��հ�����
 * @return ��ս��
*/
uint8 MG_BLE_ClearWhiteList(void);

/**
 * ������������
 * @param name [in]Ҫ���õ�����
 * @return ����״̬
*/
BT_STATUS MG_BLE_SetLocalName(const uint16 *name);

/**
 * ��ȡ�豸����
 * @param name [out]�豸������
 * @return ����״̬
*/
BT_STATUS MG_BLE_GetLocalName(uint16 *name);

/**
 * �����豸
 * @param type [in]��ַ����
 * @param addr [in]������ַ
 * @return ���ӽ��
*/
uint8 MG_BLE_Connect(uint8 type, uint8 addr[6]);

/**
 * ��ȡ����״̬
 * @param addr [in]���Ӷ�Ӧ�ĶԷ���������ַ
 * @return ��ȡ���
*/
uint8 MG_BLE_GetConnectState(uint8 *addr);

/**
 * �Ͽ�����
 * @param addr [in]Ҫ�Ͽ������ӵĵ�ַ
 * @return �Ͽ����
*/
uint8 MG_BLE_Disconnect(uint8 addr[6]);

/**
 * ��������
 * @param handle [in]���Ӿ��
 * @param intervalMin [in]��С���
 * @param intervalMax [in]�����
 * @param slaveLatency [in]�ӳ�
 * @param timeoutMulti [in]���ӳ�ʱʱ��
 * @return ���½��
*/
uint8 MG_BLE_UpdateConnect(uint16 handle, uint16 intervalMin, uint16 intervalMax, uint16 slaveLatency, uint16 timeoutMulti);

/**
 * ��������
 * @param datalen [in]���ݳ���
 * @param data [in]����
 * @return ���͵ĳ���
*/
uint8 MG_BLE_SendData(uint16 datalen, uint8 *data);

/**
 * ���ù㲥����
 * @param advMin [in]��С���
 * @param advMax [in]�����
 * @param advType [in]�㲥����
 * @param ownAddrType [in]�㲥��ַ����
 * @param directAddrType [in]�����ַ����
 * @param directAddr [in]�����ַ
 * @param advChannMap [in]�㲥ͨ��
 * @param sdvFilter [in]�㲥��������
 * @return ���ý��
*/
uint8 MG_BLE_SetAdvPara(uint16 advMin, uint16 advMax, uint8 advType, uint8 ownAddrType, uint8 directAddrType, bdaddr_t directAddr, uint8 advChannMap, uint8 advFilter);

/**
 * ���ù㲥����
 * @param len [in]���ݳ���
 * @param data [in]����
 * @return ���ý��
*/
uint8 MG_BLE_SetAdvData(uint8 len, char *data);

/**
 * ���ù㲥ʹ��
 * @param enable [in]Ҫ���õĹ㲥ʹ��״̬
 * @return ���ý��
*/
uint8 MG_BLE_SetAdvEnable(uint8 enable);

/**
 * ����ɨ��ظ�
 * @param data [in]�ظ�����
 * @param length [in]���ݳ���
 * @return ���ý��
*/
uint8 MG_BLE_SetScanRsp(uint8 *data, uint8 length);

/**
 * ����ɨ�����
 * @param type [in]ɨ������
 * @param interval [in]ɨ����
 * @param window [in]ɨ�贰��
 * @param filter [in]��������
 * @return ���ý��
*/
uint8 MG_BLE_SetScanPara(uint8 type, uint16 interval, uint16 window, uint8 filter);

/**
 * ����ɨ��ʹ��
 * @param enable [in]Ҫ���õ�ɨ��ʹ��״̬
 * @return ���ý��
*/
uint8 MG_BLE_SetScanEnable(uint8 enable);

/**
 * ��ȡ�������豸����
 * @return �豸����
*/
uint8 MG_BLE_QuiryWhiteCount(void);

/**
 * ��������
*/
uint8 MG_BLE_SendTpData(uint16 datalen, uint8 *data);

uint8 MG_BLE_SetBeaconData(char *uuid, char *major, char *minor);

void MG_BLE_RegisterWriteUartCallback(BLE_WRITE_UART_CALLBACK callback);

void MG_BT_Register_AtCallback_Func(BT_CALLBACK_STACK callback);