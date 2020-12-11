#include "ty_http_adpt.h"
#include "ty_sc_adpt.h"
#include "ty_ssl_adpt.h"
#include "ty_os_adpt.h"
#include "ty_sock_test.h"

#define  OSI_PRIORITY_NORMAL 24
int ty_virt1_handle = 0;
int ty_virt2_handle = 0;
int ty_virt3_handle = 0;
static void VirtAtEntry1(void *param)
{
    char * paid = "A0000005591010FFFFFFFF8900000100";
    char *inData = "81CAFF2100";
    char out[256] = {0};    
    int outlen = 256;  
    sc_handle handle1 = Ty_ScOpenLogicChannel(paid);
    Ty_ScTransimt(handle1,  inData, out, &outlen);
//    Ty_ScCloseLogicChannel(handle1);
    ty_task_delete(&ty_virt1_handle);
}

static void VirtAtEntry2(void *param)
{
    char * paid = "A0000005591010FFFFFFFF8900000100";
    char *inData = "01A4040410A0000005591010FFFFFFFF8900000100";
//    char out[256] = {0};
    char *out = NULL;
    int outlen = 256;
    sc_handle handle2 = Ty_ScOpenLogicChannel(paid);
    for(int i = 0; i< 7; ++i)
    {   
//        Ty_ScTransimt(handle2,  inData, out, &outlen);
        Ty_ScTransimtAutoFetch(handle2, inData, &out);
        LOG("%s out = %s", __func__, out);
        lpa_free(out);
        out = NULL;
        ty_sleep(2000);
    }
    
//    Ty_ScCloseLogicChannel(handle2);
     ty_task_delete(&ty_virt2_handle);
}

static void VirtAtEntry3(void *param)
{
    char * paid = "A0000005591010FFFFFFFF8900000100";
    char *inData = "81E2910003BF3C00";
    char out[256] = {0};    
    int outlen = 256;
    sc_handle handle3 =NULL;

    handle3 = Ty_ScOpenLogicChannel(paid);
    Ty_ScOpenLogicChannel(paid);      //test error  三个通道已经全部打开，继续打开会报错
    Ty_ScTransimt(handle3,  inData, out, &outlen);
    Ty_ScCloseLogicChannel(handle3);
    Ty_ScCloseLogicChannel(handle3);    //test error    第三个通道已经关闭了一次，继续关闭会报错,再AT指令测试可以继续打开通道3
    Ty_ScTransimt(handle3,  inData, out, &outlen); //test error  第三个通道已经关闭，往三通道里面传数据会报错
    ty_task_delete(&ty_virt3_handle);
}

void ty_sc_channel_test()
{
        LOG("%s", __func__);
        ty_sleep(1000);
        ty_task_create(VirtAtEntry1, "vat1", 1024, NULL, OSI_PRIORITY_NORMAL, &ty_virt1_handle);
        ty_task_create(VirtAtEntry3, "vat3", 1024, NULL, OSI_PRIORITY_NORMAL, &ty_virt3_handle);
        ty_task_create(VirtAtEntry2, "vat2", 1024, NULL, OSI_PRIORITY_NORMAL, &ty_virt2_handle);
        while(1)
        {
            LOG("%s",__func__);
            ty_sleep(5000);
        }
    
}