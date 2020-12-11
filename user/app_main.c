/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "app_main.h"

int ty_task_handle = 0;
int ty_sslsock_handle = 0;
int ty_mqtt_handle = 0;
int ty_http_handle = 0;
#ifdef TY_ESIM_PROFILE
#include "lpa.h"
#endif


static void ty_TestEntry(void *param)
{
    char * buff = (char*)param;
    for(int i=0; i < 2; ++i)
        {
            LOG("%s:%s", __func__, buff);
            LOG("%s:%d :PRODUCT_TYPE: %s", __func__, __LINE__, PRODUCT_TYPE);
            LOG("%s:%d :FIRMWARE_VERSION: %s", __func__, __LINE__, FIRMWARE_VERSION);
            LOG("%s:%d :MQTT_PRODUCT_KEY: %s", __func__, __LINE__, MQTT_PRODUCT_KEY);
            LOG("%s:%d :MQTT_PRODUCT_SECRET: %s", __func__, __LINE__, MQTT_PRODUCT_SECRET);
            LOG("%s:%d :TY_CLOUD_MQTT_SERVER: %s", __func__, __LINE__, TY_CLOUD_MQTT_SERVER);
            LOG("%s:%d :TY_CLOUD_PORT: %d", __func__, __LINE__, TY_CLOUD_PORT);
            LOG("%s:%d :TY_CLOUD_HTTP_DEVREG_URL: %s", __func__, __LINE__, TY_CLOUD_HTTP_DEVREG_URL);
//            LOG("%s:%d :TY_MEMLEAK_CHECK: %d", __func__, __LINE__, TY_MEMLEAK_CHECK);    
        }
    char *secret = "12345678123456781234567812345678";
    /*char load_secret[64] = {0};
    int ret = -1;
    ret = ty_SaveDeviceSecret(secret);
    if(ret != 0)
    {
        LOG ("save fail\r\n");
    }
    ret = ty_LoadDeviceSecret(load_secret);
    if(ret != 0)
    {
        LOG ("load fail\r\n");
    }
    LOG(" ret = %d load_secret : %s", ret, load_secret);
    */
    char *hello = "hello123";
    bool flag = ty_ota_update_Init();
    if(true == flag)
        {
            LOG("ty_ota_update_Init success!");
            
        }else{
             LOG("ty_ota_update_Init failed!!");

        }

    for(int i =0; i <3; i++){
            int ret = ty_ota_write_flash(secret, strlen(secret));
            if(ret < 0){
                LOG("ty_ota_write_flash failed ret = %d", ret);
                break;
                }
        }
    ty_ota_update_read();

   flag = ty_ota_update_Init();
    if(true == flag)
        {
            LOG("ty_ota_update_Init success!");
            
        }else{
             LOG("ty_ota_update_Init failed!!");

        }

    for(int i =0; i <3; i++){
            int ret = ty_ota_write_flash(hello, strlen(hello));
            if(ret < 0){
                LOG("ty_ota_write_flash failed ret = %d", ret);
                break;
                }
        }

    ty_ota_update_read();
    ty_task_delete(&ty_task_handle);
}


int app_main_test(OPTION option)
{
    do
    {
        switch(option)
        {
            case TY_TASK_TEST:
            {
               char * ty_param = "param test";
               ty_task_create(ty_TestEntry, "ty_test", 1024*800, ty_param,OSI_PRIORITY_NORMAL, &ty_task_handle);
               break;
            }
            case TY_HTTPS_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_http_test, "http_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_http_handle);
                break;
            }
            case TY_MQTT_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_mqtt_test, "mqtt_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_mqtt_handle);
                break;
            }
            case TY_SOCKET_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_adpt_tcpsock_test();
                break;
            }
            case TY_SSLSOCK_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_sslsock_test, "sslsock_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_sslsock_handle);
                break;
            }
            case TY_MSGQUEUE_TEST:
            {
                 test_messagequeue();
                 break;
            }
            case TY_MUTEX_TEST:
            {
                test_mutex();
                break;
            }
            case TY_FILESYSTEM_TEST:
            {
                filesystem_test();
                break;
            }
            case TY_SYSTEM_TIMER_TEST:
            {
                test_system_ptimer();
                break;
            }
            case TY_LOCALTIME_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
//                test_localtime();
                break;
            }
            case TY_SC_CHANNEL_TEST:
            {
                ty_sc_channel_test();
                break;
            }
            case TY_MAIN_TASK_ENTRY:
            {
                ty_wait_tcpip_ok(10*1000);
                int ret = -1;

                char load_secret[64] = {0};
                /*
                char *secret = "18993bb4b75940598a8ec57b7e6ba0d8";
                
                
                ret = ty_SaveDeviceSecret(secret);
                if(ret != 0)
                {
                    LOG ("save fail\r\n");
                }
                */
      
                ret = ty_LoadDeviceSecret(load_secret);
                if(ret != 0)
                {
                    LOG ("load fail\r\n");
                }
                else LOG ("load OK\r\n");
                
                ty_main_task_entry();
                break;
            }
            case TY_ESIM_PROFILE_ENABLE_TEST:
            {
                ProfileInfoList list = {0};
                Lpa_Ctx ctx = {0};
                ty_wait_tcpip_ok(10*1000);
                int ret = TyGetProfilesInfo(&ctx,&list, profileClass_test);
                if(ret)
                    break;  
                LOG(" profileSize=%d\r\n",list.profileSize);
                //89860116281550255010
                //ret = TyEnable (NULL, "89860116281550255010", opr_mark_a2 );
                //LOG("TyEnable ret=%d\r\n",ret);
                break;
            }
            case TY_LOCAL_TIME_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                test_localtime();
                break;
            }
            
            default:
                break;
       } 
        
    }while(0);
    return 0;
}

int appimg_enter(void *param)
{
    char * ty_param = "param test";
    LOG("my test enter");
    OPTION option;
    option = TY_MAIN_TASK_ENTRY;
    
    ty_plat_os_init();
    
	ty_esim_at_init ();
    
    #if 0   
    do
    {
        switch(option)
        {
            case TY_TASK_TEST:
            {
               ty_task_create(ty_TestEntry, "ty_test", 1024*800, ty_param,OSI_PRIORITY_NORMAL, &ty_task_handle);
               break;
            }
            case TY_HTTPS_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_http_test, "http_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_http_handle);
                break;
            }
            case TY_MQTT_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_mqtt_test, "mqtt_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_mqtt_handle);
                break;
            }
            case TY_SOCKET_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_adpt_tcpsock_test();
                break;
            }
            case TY_SSLSOCK_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                ty_task_create(ty_adpt_sslsock_test, "sslsock_test", 1024*800, NULL, OSI_PRIORITY_NORMAL, &ty_sslsock_handle);
                break;
            }
            case TY_MSGQUEUE_TEST:
            {
                 test_messagequeue();
                 break;
            }
            case TY_MUTEX_TEST:
            {
                test_mutex();
                break;
            }
            case TY_FILESYSTEM_TEST:
            {
                filesystem_test();
                break;
            }
            case TY_SYSTEM_TIMER_TEST:
            {
                test_system_ptimer();
                break;
            }
            case TY_LOCALTIME_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
//                test_localtime();
                break;
            }
            case TY_SC_CHANNEL_TEST:
            {
                ty_sc_channel_test();
                break;
            }
            case TY_MAIN_TASK_ENTRY:
            {
                ty_wait_tcpip_ok(10*1000);
                int ret = -1;

                char load_secret[64] = {0};
                /*
                char *secret = "18993bb4b75940598a8ec57b7e6ba0d8";
                
                
                ret = ty_SaveDeviceSecret(secret);
                if(ret != 0)
                {
                    LOG ("save fail\r\n");
                }
                */
                megaInit();
                ret = ty_LoadDeviceSecret(load_secret);
                if(ret != 0)
                {
                    LOG ("load fail\r\n");
                }
                else LOG ("load OK\r\n");
                
                ty_main_task_entry();
                break;
            }
            case TY_ESIM_PROFILE_ENABLE_TEST:
            {
                ProfileInfoList list = {0};
                Lpa_Ctx ctx = {0};
                ty_wait_tcpip_ok(10*1000);
                int ret = TyGetProfilesInfo(&ctx,&list, profileClass_test);
                if(ret)
                    break;  
                LOG(" profileSize=%d\r\n",list.profileSize);
                //89860116281550255010
                //ret = TyEnable (NULL, "89860116281550255010", opr_mark_a2 );
                //LOG("TyEnable ret=%d\r\n",ret);
                break;
            }
			case TY_LOCAL_TIME_TEST:
            {
                ty_wait_tcpip_ok(10*1000);
                test_localtime();
                break;
            }
			
            default:
                break;
       } 
        
    }while(0);
		
#endif
return 0;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
