#include "bt_abs.h"

/**
 * 获取当前蓝牙版本
 * @return 版本信息
*/
const char *MG_GetBTVersion(void);

/**
 * 开启蓝牙（蓝牙使能）
 * @return 蓝牙开关状态
*/
// BT_STATUS MG_BT_Start(void);

/**
 * 关闭蓝牙
 * @return 蓝牙开关状态
*/
// BT_STATUS MG_BT_Stop(void);

/**
 * 获取蓝牙状态
 * @return 蓝牙使能状态
*/
// BOOLEAN MG_BT_GetState(void);

/**
 * 设置设备可见性
 * @param visible [in]是否可见
 * @return 蓝牙可见性状态
*/
BT_STATUS MG_BT_SetVisibilityUni(BT_SCAN_E visible);

/**
 * 获取设备可见性
 * @return 设备可见性
*/
BT_SCAN_E MG_BT_GetVisibilityUni(void);

/**
 * 设置本地设备名称
 * @param name [in]要设置的名字
 * @return 设置结果
*/
BT_STATUS MG_BT_SetLocalName(const uint16 *name);

/**
 * 获取设备名称
 * @param name [out]获取的名字
 * @return 执行结果
*/
BT_STATUS MG_BT_GetLocalName(uint16 *name);

/**
 * 设置蓝牙地址
 * @param addr [in]蓝牙地址
 * @return 设置结果
*/
BT_STATUS MG_BT_SetBdAddr(BT_ADDRESS *addr);

/**
 * 获取蓝牙地址
 * @param addr [out]获取的蓝牙地址
*/
void MG_BT_GetBdAddr(BT_ADDRESS *addr);

/**
 * 连接蓝牙（当前仅支持SPP）
 * @param bd_addr [in]对方的蓝牙地址
 * @param spp_speed [in]端口速率
 * @return 连接结果
*/
BT_STATUS MG_BT_SppConnect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);

/**
 * 断开SPP连接
 * @return 断开结果
*/
BT_STATUS MG_BT_SppDisconnect(void);

/**
 * 获取蓝牙连接状态
 * @param addr [in]蓝牙地址
 * @return 获取的状态
*/
BOOLEAN MG_BT_SppConnectStatusUni(const BT_ADDRESS *addr);

/**
 * 查找蓝牙设备
 * @param service_type [in]设备类型
 * @return 查找结果
*/
// BT_STATUS MG_BT_SearchDevice(uint32 service_type);

/**
 * 匹配蓝牙设备
 * @param addr [in]蓝牙地址
 * @return 匹配结果
*/
BT_STATUS MG_BT_PairDevice(const BT_ADDRESS *addr);

/**
 * 查询匹配的蓝牙的数量 BT_GetPairedDeviceCount(0xFFFF);
 * @param service_type [in]设备类型
 * @return 匹配的数量
*/
int MG_BT_GetPairedDeviceCount(uint32 service_type);

/**
 * 获取匹配的设备信息
 * @param service_type [in]设备类型
 * @param index [in]设备索引
 * @param info [out]设备信息
 * @return 获取结果
*/
BT_STATUS MG_BT_GetPairedDeviceInfo(uint32 service_type, int index, BT_DEVICE_INFO *info);

/**
 * 断开匹配的设备
 * @param addr [in]设备地址
 * @return 断开结果
*/
BT_STATUS MG_BT_RemovePairedDevice(const BT_ADDRESS *addr);


/*****************************BLE*****************************/
/**
 * 设置公共地址
 * @param addr [in]要设置的地址
 * @return 返回设置结果
*/
uint8 MG_BLE_SetPublicAddr(uint8 addr[6]);

/**
 * 获取公共地址
 * @param addr [out]获取的地址
*/
void MG_BLE_GetPublicAddr(uint8 *addr);

/**
 * 设置随机地址
 * @param addr [in]要设置的地址
 * @return 设置结果
*/
uint8 MG_BLE_SetRandomAddr(uint8 addr[6]);

/**
 * 获取随机地址
 * @param addr [out]获取的地址
*/
void MG_BLE_GetRandomAddr(uint8 *addr);

/**
 * 添加设备到白名单
 * @param white_list_info [in]设备相关信息
 * @return 添加结果
*/
uint8 MG_BLE_AddWhiteList(st_white_list_info white_list_info);

/**
 * 查询白名单
 * @param index [in]设备索引
 * @param white_list_info [out]读取的设备信息
 * @return 查询结果
*/
uint8 MG_BLE_QuiryWhiteListInfo(uint8 index, st_white_list_info *white_list_info);

/**
 * 从白名单移除设备
 * @param addr [in]设备蓝牙地址
 * @return 移除结果
*/
uint8 MG_BLE_RemoveWhiteList(uint8 *addr);

/**
 * 清空白名单
 * @return 清空结果
*/
uint8 MG_BLE_ClearWhiteList(void);

/**
 * 设置蓝牙名字
 * @param name [in]要设置的名字
 * @return 蓝牙状态
*/
BT_STATUS MG_BLE_SetLocalName(const uint16 *name);

/**
 * 获取设备名字
 * @param name [out]设备的名字
 * @return 蓝牙状态
*/
BT_STATUS MG_BLE_GetLocalName(uint16 *name);

/**
 * 连接设备
 * @param type [in]地址类型
 * @param addr [in]蓝牙地址
 * @return 连接结果
*/
uint8 MG_BLE_Connect(uint8 type, uint8 addr[6]);

/**
 * 获取连接状态
 * @param addr [in]连接对应的对方的蓝牙地址
 * @return 获取结果
*/
uint8 MG_BLE_GetConnectState(uint8 *addr);

/**
 * 断开连接
 * @param addr [in]要断开的连接的地址
 * @return 断开结果
*/
uint8 MG_BLE_Disconnect(uint8 addr[6]);

/**
 * 更新连接
 * @param handle [in]连接句柄
 * @param intervalMin [in]最小间隔
 * @param intervalMax [in]最大间隔
 * @param slaveLatency [in]延迟
 * @param timeoutMulti [in]连接超时时间
 * @return 更新结果
*/
uint8 MG_BLE_UpdateConnect(uint16 handle, uint16 intervalMin, uint16 intervalMax, uint16 slaveLatency, uint16 timeoutMulti);

/**
 * 发送数据
 * @param datalen [in]数据长度
 * @param data [in]数据
 * @return 发送的长度
*/
uint8 MG_BLE_SendData(uint16 datalen, uint8 *data);

/**
 * 设置广播参数
 * @param advMin [in]最小间隔
 * @param advMax [in]最大间隔
 * @param advType [in]广播类型
 * @param ownAddrType [in]广播地址类型
 * @param directAddrType [in]定向地址类型
 * @param directAddr [in]定向地址
 * @param advChannMap [in]广播通道
 * @param sdvFilter [in]广播过滤设置
 * @return 设置结果
*/
uint8 MG_BLE_SetAdvPara(uint16 advMin, uint16 advMax, uint8 advType, uint8 ownAddrType, uint8 directAddrType, bdaddr_t directAddr, uint8 advChannMap, uint8 advFilter);

/**
 * 设置广播数据
 * @param len [in]数据长度
 * @param data [in]数据
 * @return 设置结果
*/
uint8 MG_BLE_SetAdvData(uint8 len, char *data);

/**
 * 设置广播使能
 * @param enable [in]要设置的广播使能状态
 * @return 设置结果
*/
uint8 MG_BLE_SetAdvEnable(uint8 enable);

/**
 * 设置扫描回复
 * @param data [in]回复数据
 * @param length [in]数据长度
 * @return 设置结果
*/
uint8 MG_BLE_SetScanRsp(uint8 *data, uint8 length);

/**
 * 设置扫描参数
 * @param type [in]扫描类型
 * @param interval [in]扫描间隔
 * @param window [in]扫描窗口
 * @param filter [in]过滤设置
 * @return 设置结果
*/
uint8 MG_BLE_SetScanPara(uint8 type, uint16 interval, uint16 window, uint8 filter);

/**
 * 设置扫描使能
 * @param enable [in]要设置的扫描使能状态
 * @return 设置结果
*/
uint8 MG_BLE_SetScanEnable(uint8 enable);

/**
 * 获取白名单设备数量
 * @return 设备数量
*/
uint8 MG_BLE_QuiryWhiteCount(void);

/**
 * 发送数据
*/
uint8 MG_BLE_SendTpData(uint16 datalen, uint8 *data);

uint8 MG_BLE_SetBeaconData(char *uuid, char *major, char *minor);

void MG_BLE_RegisterWriteUartCallback(BLE_WRITE_UART_CALLBACK callback);

void MG_BT_Register_AtCallback_Func(BT_CALLBACK_STACK callback);