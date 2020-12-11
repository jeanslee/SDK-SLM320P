/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */


#define _ADV_DEVICE_ //define this: device adv, otherwise device scan 
//#define _IBEACONTEST_ //ibeacon or not
#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'B', 'T')
#define sleep(x) do{for(int zz = 0; zz < (2*x); ++zz)\
MG_osiThreadSleep(500);\
}while(0)



# include "osi_api.h"
#include "osi_log.h"
#include "mg_bt_api.h"
#include "bt_abs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char PUBADDR[6];
char RANADDR[6];
uint8 remote_devaddr[6] = {0};

int btOpen();
uint16_t *AppConvertStrToWstr(const uint8_t *src, uint16_t *dst);
uint8_t *AppConvertWstrToStr(const uint16_t *src, uint8_t *dst);
int AddrStringsToU8Int(char *_src, char *_des, int _length);
int AddrU8IntToStrings(char *_src, char *_des);
int setPublicAddr(char *addr, int size);
int setRandomAddr(char *addr, int size);
void getPublicAddr();
void getRandomAddr();
void AppBtMsgCallback(unsigned int msg_id, char status, void *data_ptr);
uint8_t deviceBleAdv(uint16_t advMin, uint16_t advMax, uint8_t advType, uint8_t ownAddrType, uint8_t directAddrType, bdaddr_t directAddr, uint8_t advChannMap, uint8_t advFilter);
uint8_t deviceBleScan(uint8 type, uint16 interval, uint16 window, uint8 filter);
void addWhiteList(const char *addr, uint8_t addr_type);
void quiryWhiteList();
void _AppBleRecvTpDataCb(short _length, unsigned char *_data);

void scandev();
void connectTest();
void whiteListTest();

void bletest();

static void prvThreadEntry(void *param)
{
    //public addr set and read
    setPublicAddr("4B:B0:D9:68:09:2b",18);
    getPublicAddr();
    sleep(1);
    //open blue tooth
    btOpen();
    sleep(1);
    //random addr set and read
    setRandomAddr("7C:C0:D9:68:09:2b",18);
    getRandomAddr();
    sleep(1);
    //set device name, these can not be done in function
    uint16_t wstr[24] = {0};
    AppConvertStrToWstr("BLE320", wstr);
    MG_BLE_SetLocalName(wstr);
    sleep(1);
    bzero(wstr, sizeof(wstr));
    uint8_t str[36] = {0};
    MG_BLE_GetLocalName(wstr);
    AppConvertWstrToStr(wstr, str);
    MG_osiTracePrintf(LOG_TAG, "device name : %s", str);
    sleep(1);

    bletest();
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mgsock", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}




uint8_t *AppConvertWstrToStr(const uint16_t *src, uint8_t *dst)
{
    uint8_t *return_str = dst;
    if (src == NULL || NULL == dst)
    {
        return return_str;
    }
    while (!(*src & 0xFF00) && '\0' != (*dst++ = *src++));
    return return_str;
}
uint16_t *AppConvertStrToWstr(const uint8_t *src, uint16_t *dst)
{
    uint16_t *return_wstr = dst;
    if (src == NULL || NULL == dst)
    {
        return return_wstr;
    }
    while ('\0' != (*dst++ = *src++));
    return return_wstr;
}
int btOpen()
{
    if (MG_BT_GetState())
    {
        MG_osiTracePrintf(LOG_TAG, "btOpen() : already opened");
        return 0;
    }
    else
    {
        if (MG_BT_Start() == BT_PENDING)
        {
            MG_osiTracePrintf(LOG_TAG, "MG_BT_Start() : bt pending");
            int i = 5;
            while ((i--) > 0) sleep(1);
            if (MG_BT_GetState())
            {
                MG_osiTracePrintf(LOG_TAG, "MG_BT_Start() : bt opened successfully");
                return 0;
            }
            else
            {
                MG_osiTracePrintf(LOG_TAG, "MG_BT_Start() : bt opened failed");
                return -1;
            }
        }
        else
        {
            MG_osiTracePrintf(LOG_TAG, "MG_BT_Start() : bt opened failed");
            return -1;
        }  
    }
    return -1;
}
int AddrStringsToU8Int(char *_src, char *_des, int _length)
{
    int i = 0;
    int j = 0;
    char Tmp[18] = {0};
    int Length = 0;
    char TraceBuf[128] = {0,};
    for (i = 0, j = 0; i < _length; i++)
    {
        if (':' != _src[i])
        {
            Tmp[j] = _src[i];
            j++;
        }
    }

    Length = strlen(Tmp);

    for (i = 0; i < Length; i++)
    {
        if (Tmp[i] >= '0' && Tmp[i] <= '9')
            Tmp[i] = Tmp[i] - '0';
        else if (Tmp[i] >= 'A' && Tmp[i] <= 'F')
            Tmp[i] = 10 + (Tmp[i] - 'A');
        else if (Tmp[i] >= 'a' && Tmp[i] <= 'f')
            Tmp[i] = 10 + (Tmp[i] - 'a');
        else
            return -1;
    }

    for (i = 0, j = 0; i < Length; i += 2, j++)
    {
        _des[j] = ((16 * (Tmp[i])) + (Tmp[i + 1]));
    }

    return 0;
}
int AddrU8IntToStrings(char *_src, char *_des)
{
    sprintf(_des, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", _src[0], ":", _src[1], ":", _src[2], ":", _src[3], ":", _src[4], ":", _src[5]);
    return 0;
}
int setPublicAddr(char *addr, int size)
{
    int i = 0;
    if (size != 18)
    {
        MG_osiTracePrintf(LOG_TAG, "setPublicAddr() : addr size incorrect");
        return -1;
    }
    else
    {
        AddrStringsToU8Int(addr,PUBADDR,size);
        int ret = MG_BLE_SetPublicAddr((unsigned char *)PUBADDR);
        MG_osiTracePrintf(LOG_TAG, "MG_BLE_SetPublicAddr() : ret = %d", ret);
    }
    return 0;
}
int setRandomAddr(char *addr, int size)
{
    int i = 0;
    if(size != 18)
    {
        MG_osiTracePrintf(LOG_TAG, "setRandomAddr() : addr size incorrect");
        return -1;
    }
    else
    {
        AddrStringsToU8Int(addr,RANADDR,size);
        int ret = MG_BLE_SetRandomAddr((unsigned char *)RANADDR);
        MG_osiTracePrintf(LOG_TAG, "MG_BLE_RandomAddr() : ret = %d", ret);
    }
    return 0;
}
void AppBtMsgCallback(unsigned int msg_id, char status, void *data_ptr)
{
    if (msg_id == ID_STATUS_AT_BLE_SET_SCAN_REPORT_RES)
    {
        if (!strcmp(((st_scan_report_info *)data_ptr)->name, "aabbcc"))
        {
            char devscan[128] = {0};
            MG_osiTracePrintf(LOG_TAG, "deviceType = %d", ((st_scan_report_info *)data_ptr)->addr_type);
            MG_osiTracePrintf(LOG_TAG, "remote_devname = %s", ((st_scan_report_info *)data_ptr)->name);
            sprintf(devscan,"remote_devaddr : %x:%x:%x:%x:%x:%x", ((st_scan_report_info *)data_ptr)->bdAddress.addr[0],
            ((st_scan_report_info *)data_ptr)->bdAddress.addr[1],((st_scan_report_info *)data_ptr)->bdAddress.addr[2],
            ((st_scan_report_info *)data_ptr)->bdAddress.addr[3],((st_scan_report_info *)data_ptr)->bdAddress.addr[4],
            ((st_scan_report_info *)data_ptr)->bdAddress.addr[5]);
            MG_osiTracePrintf(LOG_TAG, "%s", devscan);
            bzero(remote_devaddr, sizeof(remote_devaddr));
            for(int i = 0; i < 6; ++i)
                remote_devaddr[i] = ((st_scan_report_info *)data_ptr)->bdAddress.addr[i];
        }
    }
    return;
}
uint8_t deviceBleAdv(uint16_t advMin, 
                uint16_t advMax, 
                uint8_t advType, 
                uint8_t ownAddrType, 
                uint8_t directAddrType, 
                bdaddr_t directAddr, 
                uint8_t advChannMap, 
                uint8_t advFilter)
{
    int ret = MG_BLE_SetAdvPara((unsigned short)advMin, 
                                (unsigned short)advMax, 
                                (unsigned char)advType, 
                                (unsigned char)ownAddrType, 
                                (unsigned char)directAddrType, 
                                directAddr, 
                                (unsigned char)advChannMap, 
                                (unsigned char)advFilter);
    MG_osiTracePrintf(LOG_TAG, "MG_BLE_SetAdvPara(): ret = %d", ret);
    sleep(5);  

    #ifdef _IBEACONTEST_
        char uuid[48] = "b9407f30f5f8466eaff925556b57fe6d";
        char major[12] = "0001";
        char minor[12] = "0002";
        BLE_SetBeaconData(uuid, major, minor);
        sleep(1);
    #endif

    MG_BLE_RegisterWriteUartCallback(_AppBleRecvTpDataCb);

    uint8_t data[7] = {0x06,0x09,0x05,0x30,0x30,0x30,0x30};
    int rspResult = MG_BLE_SetScanRsp(data, 7);
    MG_osiTracePrintf(LOG_TAG, "%d  rspResult = %d", __LINE__, rspResult);

    return MG_BLE_SetAdvEnable(1);
}
uint8_t deviceBleScan(uint8 type, uint16 interval, uint16 window, uint8 filter)
{
    int ret = MG_BLE_SetScanPara((unsigned int)type, (unsigned short)interval, (unsigned short)window, (unsigned int)filter);
    sleep(15);
    MG_osiTracePrintf(LOG_TAG, "MG_BLE_SetScanPara() return %d", ret);
    MG_BT_Register_AtCallback_Func(AppBtMsgCallback);
    int scanenable = 1;
    ret = MG_BLE_SetScanEnable((unsigned char)scanenable);
    sleep(15);
    MG_osiTracePrintf(LOG_TAG, "MG_BLE_SetScanEnable() return %d", ret);
    return ret;
}
void getPublicAddr()
{
    char publicaddr[18] = "00:00:00:00:00:00";
    AddrU8IntToStrings(PUBADDR, publicaddr);
    MG_osiTracePrintf(LOG_TAG, "%d  getPublicAddr() : publicAddr = %s", __LINE__, publicaddr);
}
void getRandomAddr()
{
    char randomaddr[18] = "00:00:00:00:00:00";
    AddrU8IntToStrings(RANADDR, randomaddr);
    MG_osiTracePrintf(LOG_TAG, "%d  getRandomAddr() : randomAddr = %s", __LINE__, randomaddr);
}
void addWhiteList(const char *addr, uint8_t addr_type)
{
    st_white_list_info white_list_info;
    white_list_info.addr_type = addr_type;
    memcpy(white_list_info.addr, addr, 6);
    uint8_t ret = MG_BLE_AddWhiteList(white_list_info);
    MG_osiTracePrintf(LOG_TAG, "%d  %s() : addWhiteList result = %d", __LINE__, __func__, ret);
}
void quiryWhiteList()
{
    unsigned char WhiteCount = 0;
    st_white_list_info stWhiteList = {0};
    WhiteCount = MG_BLE_QuiryWhiteCount();
    MG_osiTracePrintf(LOG_TAG, "%d  %s() : devicecount = %d", __LINE__, __func__, WhiteCount);
    unsigned char i = 0;
    for (i = 0; i < WhiteCount; ++i)
    {
        MG_BLE_QuiryWhiteListInfo(i, &stWhiteList);
        MG_osiTracePrintf(LOG_TAG, "%d  %s() : addrtype = %d, addr = %x:%x:%x:%x:%x:%x",__LINE__, __func__, stWhiteList.addr_type, stWhiteList.addr[0], stWhiteList.addr[1], stWhiteList.addr[2], stWhiteList.addr[3], stWhiteList.addr[4], stWhiteList.addr[5]);
    }
}
void _AppBleRecvTpDataCb(short _length, unsigned char *_data)
{
    if (_length > 255)
    {
        MG_osiTracePrintf(LOG_TAG, "Ble receive tp data,but data length is too long.");
    }
    else
    {
        char buf[256] = {0};
        char *ptr = buf;
        while (*_data != '\0')
        {
            ptr += sprintf(ptr, "%c", *_data++);
        }
        MG_osiTracePrintf(LOG_TAG, "%s() : recv data = %s", __func__, buf);
        sleep(5);
        MG_BLE_SendTpData(sizeof(buf), buf);
    }
    return;
}
void scandev()
{
    deviceBleScan(0, 96, 48, 0);
    MG_BLE_SetScanEnable(0);
    sleep(15);
}
void connectTest(int connect)
{
    if(connect)
    {
        //connect to the device
        MG_BLE_Connect(1, remote_devaddr);
        sleep(15);
        int connectStateB = MG_BLE_GetConnectState(remote_devaddr);
        sleep(1);
        MG_BLE_UpdateConnect(0, 60, 80, 4, 500);
        sleep(1);
        int connectStateA = MG_BLE_GetConnectState(remote_devaddr);
        MG_osiTracePrintf(LOG_TAG, "%s():%d : update connect state from %d to %d", __func__, __LINE__, connectStateB, connectStateA);
    }
    else
    {
        MG_BLE_Disconnect(remote_devaddr);
        sleep(1);
        int connectState = MG_BLE_GetConnectState(remote_devaddr);
        sleep(1);
        MG_osiTracePrintf(LOG_TAG, "%s():%d : connect state after close is %d", __func__, __LINE__, connectState);
    }    
}
void whiteListTest()
{
    uint8_t temp[6] = {0};
    AddrStringsToU8Int("11:11:11:22:22:22", temp, 18);
    addWhiteList(temp, 1);
    AddrStringsToU8Int("33:33:33:44:44:44", temp, 18);
    addWhiteList(temp, 1);
    AddrStringsToU8Int("55:55:55:66:66:66", temp, 18);
    addWhiteList(temp, 1);
    quiryWhiteList();
    sleep(3);

    MG_BLE_RemoveWhiteList(temp);
    quiryWhiteList();
    sleep(3);

    MG_BLE_ClearWhiteList();
    quiryWhiteList();
    sleep(1);

    addWhiteList(remote_devaddr, 1);
}
void bletest()
{
    #ifdef _ADV_DEVICE_
        bdaddr_t ar = {0};
        int advResult = deviceBleAdv(32,35,0,0,-1,ar,7,0);
        MG_osiTracePrintf(LOG_TAG, "%d  advResult = %d", __LINE__, advResult); 
    #else
        //search device
        scandev();
        //whiteListTest
        whiteListTest();
        //connect test
        connectTest(1);
        sleep(10);
        //disconnect test
        connectTest(0);
    #endif 
    sleep(30);   
}


