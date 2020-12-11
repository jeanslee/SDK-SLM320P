#include "rtos_api.h"
#include "osi_log.h"
#include "osi_api.h"
#include "mg_date_api.h"
#include "cfw.h"
int flag = 0;
int tcb_hand =10;
int queue_task_send = 0;
int queue_task_recv = 0;
uint32_t ptimer = NULL;
static uint32_t* mutex = NULL;
uint32_t cb_h = 0;
uint32_t *test_queue = NULL;

typedef struct t_msg{
    uint32_t messageid;
    uint8_t data[8];
}t_msg_t;
t_msg_t t_g_msg;

static void Timeroutcallback(void *param)
{  
    static char n=1;  
    LOG("lf_debug timer out callback enter %d\n",n);
    if(ptimer != NULL)
    {
         n++;      
    }  
    if(n==8)
    {   
        ty_timer_delete(&ptimer);      
        LOG("lf_debug timer delete\n");
        flag = 1;
    }
}

static void test_TimerEntry(void *param)
{
    char m=1;  
    
    LOG( "lf_debug mg os timer enter");
    ty_timer_create(NULL, &ptimer, 1000, 1, Timeroutcallback);
    if(NULL == ptimer)
    {
		LOG("timer create failed\n");
		ty_task_delete(&tcb_hand);
    }
    ty_sleep(1000);
    if(ty_timer_start(&ptimer) == 0)
    {
		LOG("timer start failed\n");
		ty_task_delete(&tcb_hand);
    }
    else
    {
        LOG("lf_debug timer start success\n");  
    }
    ty_sleep(6000);
    ty_timer_stop(&ptimer);
    LOG("---->lf_debug timer stop\n");  
    LOG("---->lf_debug timer start again\n"); 
    ty_sleep(1000); 
    ty_timer_start(&ptimer);
    while(1)
    {
        if(flag){
        LOG("---->lf_debug timer task run %d\n", m);  
        m++;
        if(m == 5)
            {
                ty_task_delete(&tcb_hand);
            }
        }
        ty_sleep(2000);
    }
    ty_task_delete(&tcb_hand);
}

 void test_system_ptimer()
{
	LOG("test_system_ptimer enter\n");
	ty_task_create(test_TimerEntry, "ostimer",  1024, NULL,OSI_PRIORITY_ABOVE_NORMAL, &tcb_hand);
 }


void test_recvformqueEntry(void *param)
{
   t_msg_t *recvmsg;
   LOG("lf_debug -- recvformqueEntry enter \n");
   while(1)
   {
	 if(ty_get_msg_que(test_queue, (void *)&recvmsg, 200) == 0)
       {
//         LOG("lf_debug -- MG_osiMessageQueueTryGet failed or timeout\n");
          break;
       }
       LOG("lf_debug recvmsg->id = %d,recvmsg->data = %s\n", recvmsg->messageid, recvmsg->data);
       lpa_free(recvmsg);

   }
  LOG( "%s EXIT\n", __func__);

  ty_task_delete(&queue_task_recv);
}

static void test_sendtoqueEntry(void *param)
{
    t_msg_t *sendmsg;
    int count;
    LOG("lf_debug -- sendtoqueEntry enter \n");
    for(count = 0;count < 5;count++)
    {
        sendmsg = lpa_malloc(sizeof(t_msg_t));
        memset(sendmsg,0x00,sizeof(t_msg_t));
        sendmsg->messageid = count;  
        sprintf((char *)sendmsg->data,"hello %d",count);
        LOG("lf_debug -- sendmsg->data = %s,sendmsg->messageid = %d\n",sendmsg->data,sendmsg->messageid);
        if(test_queue == NULL)
        {
             LOG("lf_debug -- test_queue == NULL\n");
             break;

        }
        if(ty_put_msg_que(test_queue, (void *)&sendmsg, 200) == 0) 
        {
            LOG("lf_debug -- MessageQueuePut failed\n"); 
            break;
        }
		else
		{
			LOG("lf_debug -- MessageQueuePut success\n"); 
		}
        ty_sleep(500);
    }

    LOG( "%s EXIT\n", __func__);
    ty_task_delete(&queue_task_send);

}

void test_messagequeue(void)
{
    int count;
    char *sendmsg[5] = {"abcd", "efgh", "ijkl", "xyzd", "asdf"};
    char recvmsg[80] = {0};
    int ret = -1;
	test_queue =(uint32_t*)ty_create_msg_Que(5, 5);
   //add message to queue 
    for(count = 0; count<5;count++){
        
        LOG("-- sendmsg[%d] = %s\n", count, sendmsg[count]);
        ret = ty_put_msg_que(test_queue, sendmsg[count], 200);
        if(ret != 1){
            LOG("ty_put_msg_que ret = %d",ret);
            break;    
            }
    }
  //get message  from queue
   for(count = 0; count <6; count++){
    
        ret = ty_get_msg_que(test_queue, recvmsg, 200);
        if(0== ret){
            LOG("%s : ty_get_msg_que no message!\n", __func__);
            break;
            }
        
        LOG("ty_get_msg_que ret = %d recvmsg = %s", ret, recvmsg);
        ty_sleep(300);
        }
    ty_delete_msg_que(test_queue);
}

static void threadfunc(void *param)
{
    ty_mutex_lock(mutex, 200);
    int32_t num ;
    osiThread_t *pthread = MG_osiThreadCurrent();  
    for(num = 0;num < 3; num++)
    {
        ty_sleep(2000);
        LOG("lf_debug mutex --pthread = 0x%x,num = %d\n",pthread,num);
    }
    ty_mutex_unlock(mutex);
	while(1)
	{
		ty_sleep(1000);
	}
}

void test_mutex(void)
{
    LOG("lf_debug -- test_mutex enter\n");
    mutex = (uint32_t*)ty_create_mutex();
    int count;
    char threadname[100] = {0x00}; 
    for(count = 0;count < 3;count++)
    {
        memset(threadname,0x00,sizeof(threadname));
        sprintf(threadname,"thread-%d",count);
        ty_task_create(threadfunc, threadname, 1024, NULL, OSI_PRIORITY_NORMAL, &cb_h);
    }
}

void ptest_get_data_time(void)
{
//    UINT32 data_sec=0;
//    opencpu_rtc_get_int_time(&data_sec);
//    opencpu_printf("DATA_TIME:%d\n", data_sec);
}


void ptest_time_transform(void)
{
//    ST_TIME st;
//    char str_time[50];
//    memset(str_time, 0, 50); 
//    ty_get_timestamp(str_time);
//    LOG("STR_TIME:%s\n", str_time);
}

void ptest_get_systemtime(void)
{
//    ST_TIME st;
//    opencpu_getsystemtime(&st);
//    opencpu_printf("TIME:%04d/%02d/%02d,%02d:%02d:%02d\n",\
//        st.uYear, st.uMonth, st.uDay, st.uHour, st.uMinute, st.uSecond);
//    opencpu_printf("DayOfWeek:%d\n", st.uDayOfWeek);
} 

void ptest_set_systemtime(void)
{
//    ST_TIME st;
//    st.uYear = 2019;
//    st.uMonth = 6;
//    st.uDay = 11;
//    st.uHour = 17;
//    st.uMinute = 4;
//    st.uSecond = 10;
//    if (true == opencpu_setsystemtime(&st))
//    {
//        opencpu_printf("system time set ok\n");
//    }
//    else
//    {
//        opencpu_printf("system time set fail\n");
//    }
}

void ptest_get_rtc_time(ssize_t index)
{
//    if (index == 1)
//    {
//        ptest_set_systemtime();
//    }
//    else if (index == 2)
//    {
//        ptest_get_systemtime();
//    }
//    else if (index == 3)
//    {
//    LOG("time get test!\r\n");
//        ptest_time_transform();
//    }
//    else if (index == 4)
//    {
//        ptest_get_data_time();
//    }
//    else
//    {
//        LOG("command error\n");
//    }

//    index  = 0;
}
//HANDLE phTask_app1 = 0, phTask_app2 = 0;
//HANDLE* papp_mutex;
//HANDLE Papp_mutex;
static void puser_app_task1(void)
{
    while (1)
    {

//        ty_mutex_lock(papp_mutex, NULL);
//        LOG("puser app task1 runing\n");
//       ty_mutex_unlock(papp_mutex);
//        ty_sleep(1000);
//        opencpu_mutex_take(Papp_mutex);
//        opencpu_printf("user app task1 runing\n");
//        opencpu_mutex_give(Papp_mutex);
//        opencpu_sleep(1000);
    }
}


static void puser_app_task2(void)
{
    while (1)
    {

//        ty_mutex_lock(papp_mutex, NULL);
//        LOG("puser app task2 runing\n");
//         ty_mutex_unlock(papp_mutex);
//        ty_sleep(1000);

//        opencpu_mutex_take(Papp_mutex);
//        opencpu_printf("user app task2 runing\n");
//        opencpu_mutex_give(Papp_mutex);
//        opencpu_sleep(1000);
    }
}

void puser_del_task(void)
{
//        ty_mutex_destroy(papp_mutex);
//        LOG("11111111papp_mutex free!\r\n");
//        papp_mutex = NULL;

//            opencpu_mutex_destory(Papp_mutex);
//            Papp_mutex = NULL;
        
//    if (phTask_app1 != NULL)
//    {
//      ty_task_delete(&phTask_app1);
//         opencpu_del_task(phTask_app1);
//        phTask_app1 = NULL;
//    }
//    
//    if (phTask_app2 != NULL)
//    {
//      ty_task_delete(&phTask_app2);
//         opencpu_del_task(phTask_app2);
//        phTask_app2 = NULL;
//    }


//    LOG("puser app task stop\n");
}

void ptest_app_task(ssize_t task_index)
{
//         papp_mutex = (HANDLE*) ty_create_mutex();
//        Papp_mutex = opencpu_mutex_create();
        switch (task_index)
        {
            case 1:
   
//              ty_task_create((ty_task_func_t)puser_app_task1, "user_app_task1", 512, NULL, 235, &phTask_app1);
//                phTask_app1 = opencpu_create_task((OPENCPU_PTASK_ENTRY)puser_app_task1, NULL, 512, 235,"puser_app_task1");
//                LOG("puser app task1 is created\n");
                break;
            case 2:
       
//                ty_task_create((ty_task_func_t)puser_app_task2, "user_app_task2", 512, NULL, 235, &phTask_app2);
//                 phTask_app2 = opencpu_create_task((OPENCPU_PTASK_ENTRY)puser_app_task2, NULL, 512, 235,"puser_app_task2");
//                LOG("puser app task2 is created\n");
                break;

            case 3:
//                puser_del_task();       

           default:
                break;
        }
        task_index = -1;
        ty_sleep(100);      
}

static void time_ThreadEntry(void *param)
{
    
    char  timestamp[24] = {0};
    int64_t utcsecond = 0;
    
//    utcsecond  = ty_rtc_get_sec(0);
    
//    ty_get_timestamp(timestamp);

    LOG( "utcsecond = %ld timestamp = %s",utcsecond, timestamp);
    

//    ty_sleep(2000);
//    ty_task_delete(&cb_h);
}

void test_localtime(void)
{   
    LOG("enter %s\n", __func__);
    char  timestp[24] = {0};
    ty_get_timestamp(timestp);
    LOG("%s : %s", __func__, timestp);
    int64_t secd = ty_rtc_get_sec(0);
    LOG("%s : RTC secd :%d", __func__, secd);
    
}


void filesystem_test(void)
{
    bool ret;
    char write_data[]="123456 hello world 123456";
    char read_data[50];

    ret = ty_flash_check_factory();
    if(ret == true)
    {
        LOG("first check: factory mode\n");
    }
    else if (ret == false)
    {
        LOG("first check: user mode\n");
    }
    ret = ty_flash_write(write_data, sizeof(write_data));
    if(-1 == ret)
    {
        LOG("test file write fail\n");
        return;
    }
    ret = ty_flash_write_backup(write_data, sizeof(write_data));
    if(-1 == ret)
    {
        LOG("test file backup fail\n");
        return;
    }  

    ret = ty_flash_check_factory();
    if(ret == true)
    {
        LOG("second check: factory mode\n");
    }
    else if (ret == false)
    {
         LOG("second check: user mode\n");
    }
    
    memset(read_data, 0, sizeof(read_data)); 
    ret = ty_flash_read(read_data, sizeof(write_data));
    if (-1 == ret)
    {
        LOG("test file read fail\n");	
    }
    LOG("test file read result %d, %s\n", ret,read_data);
    memset(read_data, 0, sizeof(read_data)); 
    ret = ty_flash_read_backup(read_data, sizeof(write_data));
    if (-1 == ret)
    {
        LOG("test file read fail\n");	
    }
    LOG("test file read bakeup result %d, %s\n", ret, read_data);
}

static void mutex_test_task1(void)
{
    while (1)
    {
//        ty_mutex_lock(mutex_app, 100);
//        opencpu_printf("mutex_test_task1 runing\n");
//        ty_mutex_unlock(mutex_app);
//        opencpu_sleep(1000);
    }
}

void mutex_test(ssize_t mutex_index)
{
//     
//    switch(mutex_index)
//     {
//        case 1:
//            mutex_app = (HANDLE*)ty_create_mutex();

//            break;
//        case 2:

//            ty_task_create((ty_task_func_t)mutex_test_task1, "mutex_test_task1", 512, NULL, 230, &mutex_hand_task1);
//            LOG("mutex_hand_task1 = %d\r\n", mutex_hand_task1);
//            break;
//            
//            
//        case 3:
//        
//        ty_task_delete(&mutex_hand_task1);
//        ty_mutex_destroy(mutex_app);
//        
//            break;
//            
//        default:
//            break;
//    }
}

#if 0
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/platform.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"

#define mbedtls_printf LOG

#define assert_exit(cond, ret) \
    do { if (!(cond)) { \
        LOG("  !. assert: failed [line: %d, error: -0x%04X]\n", __LINE__, -ret); \
        goto cleanup; \
    } } while (0)



void * COS_MALLOC(int nSize)
{
    //LOG("%s %d \r\n",__FUNCTION__,__LINE__);  
    return osiMalloc(nSize);
}

void COS_FREE(void *ptr)
{
    //LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    osiFree(ptr);
}

const char *rsa_pub = 
"-----BEGIN CERTIFICATE-----\r\n\
MIIDBjCCAe4CCQDr64lxszE1ZTANBgkqhkiG9w0BAQsFADBFMQswCQYDVQQGEwJB\r\n\
VTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0\r\n\
cyBQdHkgTHRkMB4XDTIwMTAyODA1MzYzMloXDTIxMTAyODA1MzYzMlowRTELMAkG\r\n\
A1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0\r\n\
IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n\
AKdajEF8fatzDpQ8TMoksI9dfVUiPv73ADO23WlgAiPAC8vO8C8dAliIMPm5tC9Z\r\n\
vVC5ApjC3GVuqadNxRJqe/e3+9nFw653yfC4mH+iVuxafhTXBmHM3QUd4yebOrNY\r\n\
9y2PIjCAD3ZgQXdcqm3jXQ/kmLAjwF757ZKbKdlEo9jy1GAXmqA8lpv9O0/kNtU2\r\n\
CbEud/9khdG6xCvL4U38iPJF52SRsTOgJmZp2Ll7zeWB6DN/MlfXS0bHgC45QSOX\r\n\
/KQe5zd7U091jEFisYm+bT0Hrs1ubberEFD7/2KgEFpewPC941hVTxdmcUm7tL3f\r\n\
jStOinsrNRcPjPmhFvvwXykCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAnQ5bFC43\r\n\
+KHpENe0ehNVCpOOu7U/vPHSplvwLmCgsBW9RzxMDLoTowjWwkoMuCI11JIlbtC9\r\n\
MsDiCkjzA2Cy/Ff/Rkz7JNLSQ8G9/jxbFxvU839J6u1wf5oe5e5sHhrTSAKTCNJ3\r\n\
c3Gn4fixDWhr7t269kl+0iwRY7YxKv3iNDUG5kurA6Msf3hd5+8eiZvXu2D8Z16u\r\n\
r+fMHycztfPrXADVn2JKUf1kvnqfmXMn4PYr2kUeoiMz9NNtQa3oKewlea2URw9X\r\n\
B8unwCYPHk52F6augg+SqPWhOZKlkHhXWzr1tkao9X7IviYfX9ymhPBGWSK759Kl\r\n\
lQfNN0MzB7PFtA==\r\n\
-----END CERTIFICATE-----";

const char *rsa_pri = 
"-----BEGIN RSA PRIVATE KEY-----\r\n\
MIIEpAIBAAKCAQEAp1qMQXx9q3MOlDxMyiSwj119VSI+/vcAM7bdaWACI8ALy87w\r\n\
Lx0CWIgw+bm0L1m9ULkCmMLcZW6pp03FEmp797f72cXDrnfJ8LiYf6JW7Fp+FNcG\r\n\
YczdBR3jJ5s6s1j3LY8iMIAPdmBBd1yqbeNdD+SYsCPAXvntkpsp2USj2PLUYBea\r\n\
oDyWm/07T+Q21TYJsS53/2SF0brEK8vhTfyI8kXnZJGxM6AmZmnYuXvN5YHoM38y\r\n\
V9dLRseALjlBI5f8pB7nN3tTT3WMQWKxib5tPQeuzW5tt6sQUPv/YqAQWl7A8L3j\r\n\
WFVPF2ZxSbu0vd+NK06Keys1Fw+M+aEW+/BfKQIDAQABAoIBAEE6gcBkjXbt+Zi9\r\n\
iBCiDE9U2Sz9lCA8lc+STupriWn1yKM/nU4/uFJZgv1S7HAoQinccq6oBM9kOqwD\r\n\
G/T5nCgJhhhzhLnT5tn0mTvmQosNGmd1B//Gob8Ezyz4zABrhKw1RdYxk9VH3zxu\r\n\
2d7K59Lx/rFovepG79pcsAsHPfWoztPjT5oTsTCEhsMXb3BLnga4m8DVhMnZh84a\r\n\
wSMzXN5NyYeP+3qQV68cyQwifRlQXd5mtWQafq/4raU8MWD9SgZqTpN5BNmorcfj\r\n\
dbn6EEy5G/9lW9EYYdovliDXNMjlnE7jMTiDIIFcvOsoRIMYYQgpf8UU1C0ANxR6\r\n\
rS5qEaECgYEAz1Mxw0m/Op9d6M2Op/DVPIdk0ssFZbHCCYrDNjbAIk4lX9EBGl8t\r\n\
zVJIu7WBo8fCQviSsBUfoJaF9W9DrgEdXqhQk+HhH6dX3DRMrhzytQQC0KXpTQs8\r\n\
yVtuiHlSppckL50puUmTtjopHnIKF9F4F9d3A0h/hi89VcENCm+6CS8CgYEAzqT4\r\n\
Fg+pbEzmmv3ShreHiv6/RyT3FOgbMBbyOhBgPRf0Mbwy1WvcE6b3QpvLP+LD0p8t\r\n\
abLiSsYmd3HP5/0M6DBbi2Yz5Q7Ya/n954y1KuK8dlJ6/ks3U9d477s6b3Q7Zb8E\r\n\
z0MRM3pAJZ/rgL+JfdIXv9DkCK8DUoOGUoemVycCgYEAnHjXl+Iy11WsFKX/QrS6\r\n\
2uH8sDLVsfjEz1rRFjugKsP75SLiyDqJYOr3ReQpIakD7nOYkhh/ND4mgCQnWk4x\r\n\
El3J4rKZq8AqyWEXdlkWvJN4ZGyzdZhDmlzNQWb/gAeWIfFlqhfjyaZ/niDHiU5o\r\n\
auYNRocS5/0DN5Np+jR0nbUCgYEAt/+2k1ZAwpk64RlPgvsaas5onT2x8GC4DLrx\r\n\
1nfMabnRixOLo/OVk74g/qNgeDM/sGRh+HL7Ah+sEsE5odT10C125goL/VKZrJN9\r\n\
3NayXkxkCWtp7tIkorgDDFuHLcSuOuu6qZ6LS234fkgGV661C16uHPMwiPblG2zX\r\n\
htcYaxMCgYAgu0YiMVq6NTcSorwxA62l87ySFWxp1onLzLOQR+mD2IvbgPQtlcNU\r\n\
LgLMaaDx7f4TAOTrAj2bt6JSooJXvckI40nhVmUWJCbSDVmmGiRnRUbw/moZeYpg\r\n\
Mg63Nc4/tAAgr+wBJp7dqTc9xt0LjcUglUZKogn5G1kpSQjuvKjdjg==\r\n\
-----END RSA PRIVATE KEY-----";


static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
    mbedtls_printf("%s\r\n", info);
    char * hex = lpa_calloc(1,len*2+1);
    if(!hex)
        return ;
    hex2asc (buf, len, hex, len*2+1);
    ty_printf_hexbuf (hex,  strlen(hex));
    lpa_free(hex);
    hex = NULL;
}
    
static void dump_rsa_key(mbedtls_rsa_context *ctx)
{
    size_t olen;
    uint8_t buf[516];
    mbedtls_printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
    mbedtls_mpi_write_string(&ctx->N , 16, buf, sizeof(buf), &olen);
    mbedtls_printf("N: %s\n", buf); 

    mbedtls_mpi_write_string(&ctx->E , 16, buf, sizeof(buf), &olen);
    mbedtls_printf("E: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->D , 16, buf, sizeof(buf), &olen);
    mbedtls_printf("D: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->P , 16, buf, sizeof(buf), &olen);
    mbedtls_printf("P: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->Q , 16, buf, sizeof(buf), &olen);
    mbedtls_printf("Q: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->DP, 16, buf, sizeof(buf), &olen);
    mbedtls_printf("DP: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->DQ, 16, buf, sizeof(buf), &olen);
    mbedtls_printf("DQ: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->QP, 16, buf, sizeof(buf), &olen);
    mbedtls_printf("QP: %s\n", buf);
    mbedtls_printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
}


int test_mbedtls_ctr_drbg_random( void *p_rng, unsigned char *output, size_t output_len )
{
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    return 0;
}

void* test_calloc (size_t n, size_t size)
{
   return osiMalloc (n*size);

}
void test_free(void* ptr)
{
    osiFree (ptr);
}


//openssl dgst -sign server.key -sha256 -out sign.bin msg.txt
//hexdump -C sign.bin
int test3(void)
{
    int ret = 0;
    uint8_t msg[11];
    uint8_t hash[32]  = {0};
    uint8_t sig[2048/8];
    uint8_t *pers = "simple_rsa_sign";
    //dtls_init();
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    memset(msg,0x31,sizeof(msg));
    msg[10] = 0x0a;
    //#define USE_entropy
    (void)mbedtls_platform_set_calloc_free(test_calloc, test_free);
    int (*f_rng)(void *, unsigned char *, size_t) = test_mbedtls_ctr_drbg_random;

    LOG("%s %d \r\n",__FUNCTION__,__LINE__);

    
    mbedtls_ctr_drbg_context ctr_drbg= {0};
    mbedtls_ctr_drbg_init(&ctr_drbg);

    #ifdef USE_entropy
    mbedtls_entropy_context entropy= {0};
    f_rng = mbedtls_ctr_drbg_random;
    mbedtls_entropy_init(&entropy);
    #endif
    
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);


    #ifdef USE_entropy
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, 
                              (const uint8_t *) pers, strlen(pers));
    #endif

    LOG("%s %d \r\n",__FUNCTION__,__LINE__);
    assert_exit(ret == 0, ret);
    mbedtls_printf("\n  . setup rng ... ok\n\n");

    mbedtls_x509_crt chain = {0};
    ret = mbedtls_x509_crt_parse (&chain, rsa_pub, strlen(rsa_pub)+1);
    assert_exit(ret == 0, ret);

    mbedtls_rsa_context *rsa = mbedtls_pk_rsa( chain.pk );

    ret = mbedtls_rsa_check_pubkey (rsa);
    assert_exit(ret == 0, ret);

    mbedtls_printf("   mbedtls_pk_rsa   check_pubkey ... ok\n\n");


    mbedtls_pk_context pk = {0};
    ret = mbedtls_pk_parse_key (&pk,rsa_pri, strlen(rsa_pri)+1, NULL, 0);
    assert_exit(ret == 0, ret);

    mbedtls_printf("    mbedtls_pk_parse_key ... ok\n\n");


    mbedtls_rsa_context *rsa_pri_ = mbedtls_pk_rsa(pk);


    ret = mbedtls_rsa_check_privkey (rsa_pri_);
    assert_exit(ret == 0, ret);

    mbedtls_printf("   mbedtls_pk_rsa   check_privkey ... ok\n\n");

    
    
    LOG("%s %d \r\n",__FUNCTION__,__LINE__);    
    //ty_sleep(10);
    assert_exit(ret == 0, ret);
    mbedtls_printf("  1. rsa generate keypair ... ok\n");
    //ty_sleep(10);
    dump_rsa_key(rsa_pri_);
    //RSA¨®????????? ¨º?3?sig?????¨¢1?

    rsa_pri_->padding  =0 ;

    mbedtls_sha256( msg, sizeof(msg), hash ,0);
    dump_buf("  2. hash:", hash, sizeof(hash));
    ret = mbedtls_rsa_pkcs1_sign(rsa_pri_, f_rng, &ctr_drbg, 
                                    MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256,
                                    sizeof(hash), hash, sig);
    assert_exit(ret == 0, ret);
    dump_buf("  2. rsa generate signature:", sig, sizeof(sig));
    //RSA1????¨¦?? ¡¤¦Ì??0?¨°?¨¦?¡è3¨¦1|
    rsa->padding  =0 ;
    ret = mbedtls_rsa_pkcs1_verify(rsa, f_rng, &ctr_drbg, 
                                        MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 
                                        sizeof(hash), hash, sig);
    assert_exit(ret == 0, ret);
    mbedtls_printf("  3. rsa verify signature ... ok\n\n");
    //ty_sleep(10);

cleanup:
    mbedtls_ctr_drbg_free(&ctr_drbg);
#ifdef USE_entropy
    mbedtls_entropy_free(&entropy);
#endif
    return ret;
}
#endif

