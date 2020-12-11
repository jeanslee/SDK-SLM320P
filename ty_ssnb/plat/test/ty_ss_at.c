#include "ty_mqtt_basic_info.h"
#include "ty_devinfo.h"
#include "ty_ss_cloud.h"
#include "main_task.h"
#include "queue_task.h"

#ifdef TY_ESIM_TSM_SAM
#include "ty_http_interface.h"
#include "ty_apdu_parse.h"
#include "ty_esim.h"
#endif

#ifdef LPA_SMS
#include "lpa_sms_local.h"
#include "pdu.h"
#endif

#include "hex_util.h"
#include "ty_ss_cloud.h"


#include "ty_ss_at.h"


#define   USER_RTC_OFFSET   (40+ty_get_tick_ms()%60)
static char  HTTP_FOTA_TEST_HOSTNAME[128] = {0};// "http://ds.xjpeng.cn:9999/"
static char  HTTP_FOTA_TEST_URI[128] = {0};// "http://ds.xjpeng.cn:9999/"
static char  HTTP_FOTA_SIGALG[4] = {0};
static char  HTTP_FOTA_SIG[65] = {0};
static char  HTTP_PARAM_INPUT[128] = {0};
static char  HTTP_SAM_TSM_URL[128] = {0};
static char  HTTP_SAM_TSM_AID[128] = {0};
static int sleep_sec = 120;

extern const char *ty_ca_cert; //mqtt
extern const char *ty_http_ca_cert;//http

static char  start_version[24] = {0};
static char  target_version[24] = {0};
static char  sigvalue[64] = {0};


#ifdef ALIOS_HTTP
extern uint8_t rsp_buf[RSP_BUF_SIZE];
extern uint8_t req_buf[REQ_BUF_SIZE];
#endif

//AID nv test
sm_aid g_test_aid = {0};
int mTask_type = 2;
#ifdef LPA_SMS
static T_LpaLocalCtx *localCtx = NULL;
#endif
int  g_lwipsck_Handle = 0;


//#define HTTPS_FOTA_TEST_HOSTNAME "https://ds.xjpeng.cn:9998/"
extern int FOTA_PACKET_SZ;
extern int flash_status;
extern ty_cloud_nv_t   g_ty_cloud_nv;

#ifdef AT_LOG_PRINT
#define SS_LOG AT_LOG
#else
#define SS_LOG LOG
#endif

#ifdef LPA_SMS
void lpa_sms_print_profileinfo(T_LpaLocalCtx *ctx)
{
    int i =0;
    char output[128] = {0};
    if(!ctx)
    {
        SS_LOG("\r\n profile ERROR\r\n");
        ty_send_rsp_str_to_ext("\r\nERROR\r\n");
        return ;
    }

    if (ctx->profileCounts <= 0 )
    {
        SS_LOG("\r\nprofile count error\r\n");
        return ;
    }

    for(i = 0 ; i < ctx->profileCounts ; i++ )
    {
        T_LpaProfile profileEntity = ctx->profileList[i];
        char* mICCID = byteToStr(profileEntity.iccid,10);
        snprintf(output,sizeof(output),"%02X,%02X,%02X,%s\r\n",
            profileEntity.operator_mark,profileEntity.active_status,profileEntity.write_status,mICCID);
        if ( NULL != mICCID)
        {
            lpa_free(mICCID);
            mICCID = NULL;
        }
        SS_LOG (output);
        ty_send_rsp_str_to_ext(output);

    }
 
}
#endif

ssize_t malloc_free_test(void)
{
    char *str=NULL;
    str=(char *)lpa_malloc(13);
    if(!str){
        return -1;
        }
    memset(str,0,13); 
    strcpy(str,"malloc test!"); 
    SS_LOG("str:%s\r\n", str);
    lpa_free(str);
    SS_LOG("str[address]:%s[%x]\r\n", str, str);
    str=NULL;
    SS_LOG("str[address]:%s[%x]\r\n", str, str); 
    return 1;
}

void lwip_http_test(void *args)
{
    SS_LOG("\r\n lwip_tcp_test 1\r\n"); 
    
    ty_work_lock(1);

    SS_LOG("\r\n lwip_tcp_test 2\r\n"); 
    
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
            ty_assert(0);

    SS_LOG("\r\n lwip_tcp_test 3\r\n"); 
    
    //int http_get_fota(const char *host,const char* fotaURI,int put,const char * sigalg,const char *sig)
    http_get_fota (HTTP_FOTA_TEST_HOSTNAME,HTTP_FOTA_TEST_URI,args,atoi(HTTP_FOTA_SIGALG),HTTP_FOTA_SIG);

    SS_LOG("\r\n lwip_tcp_test 4\r\n"); 

    //ty_work_unlock();

    SS_LOG("\r\n lwip_tcp_test 5\r\n"); 

    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n lwip_tcp_test 6\r\n"); 
}

void lwip_http_test_tsm_cap(void *args)
{
    SS_LOG("\r\n lwip_tcp_test 1\r\n"); 
    
    ty_work_lock(1);

    SS_LOG("\r\n lwip_tcp_test 2\r\n"); 
    
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
            ty_assert(0);

    SS_LOG("\r\n lwip_tcp_test 3\r\n"); 
    
    //int http_get_fota(const char *host,const char* fotaURI,int put,const char * sigalg,const char *sig)
    http_get_tsm_cap (HTTP_FOTA_TEST_HOSTNAME,HTTP_FOTA_TEST_URI,args,atoi(HTTP_FOTA_SIGALG),HTTP_FOTA_SIG,"{}");

    SS_LOG("\r\n lwip_tcp_test 4\r\n"); 

    //ty_work_unlock();

    SS_LOG("\r\n lwip_tcp_test 5\r\n"); 

    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n lwip_tcp_test 6\r\n"); 
}



static ty_http_handle_t http_hd  = NULL;

void lwip_http_post_test(void *args)
{
    char *params = (char*)args;
    SS_LOG("\r\n lwip_http_post_test 1\r\n"); 
    
    ty_work_lock(1);

    SS_LOG("\r\n lwip_http_post_test 2\r\n"); 
    
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
            ty_assert(0);

    SS_LOG("\r\n lwip_http_post_test 3\r\n"); 

   SS_LOG("params=%s",params);
   char testPram[1500]  = {0};
   memset(testPram,0x31,sizeof(testPram)-1);

   SS_LOG("testPram=%d",strlen(testPram));
    
   if(http_hd == NULL)
   {
        ty_http_cfg cfg = {0};
        cfg.cert = ty_http_ca_cert;
        cfg.host = HTTP_FOTA_TEST_HOSTNAME;
        cfg.keepalvie = true;
        http_hd = ty_http_setup (&cfg);
   }
   char *resp = NULL;
   if(http_hd)
        resp = ty_http_post_request (http_hd, HTTP_FOTA_TEST_URI, params,10000);
   
   //char *resp = http_post (HTTP_FOTA_TEST_HOSTNAME,HTTP_FOTA_TEST_URI,testPram,ty_http_ca_cert);
   if(resp)
   {
       //SS_LOG("resp=%s",resp);
       ty_send_rsp_str_to_ext(resp);
       lpa_free(resp);
   }
   else
   {
       ty_send_rsp_str_to_ext("http post fail");
   }
    SS_LOG("\r\n lwip_http_post_test 4\r\n"); 

    //ty_work_unlock();

    SS_LOG("\r\n lwip_http_post_test 5\r\n"); 

    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n lwip_tcp_test 6\r\n"); 
}


void fota_http_post_test(void *args)
{
    char *params = (char*)args;
    SS_LOG("\r\n lwip_http_post_test 1\r\n"); 
    
    ty_work_lock(1);

    SS_LOG("\r\n lwip_http_post_test 2\r\n"); 
    
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
            ty_assert(0);

    SS_LOG("\r\n lwip_http_post_test 3\r\n"); 

    ty_FotaUploadErrMsg (HTTP_FOTA_TEST_HOSTNAME, HTTP_FOTA_TEST_URI, 3);
    
    SS_LOG("\r\n lwip_http_post_test 4\r\n"); 

    //ty_work_unlock();

    SS_LOG("\r\n lwip_http_post_test 5\r\n"); 

    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n lwip_tcp_test 6\r\n"); 
}
#ifdef ALIOS_HTTP
void lwip_ali_http_post_test(void *args)
{
     char *params = (char*)args;
     SS_LOG("\r\n lwip_http_post_test 1\r\n"); 
     
     ty_work_lock(1);
    
     SS_LOG("\r\n lwip_http_post_test 2\r\n"); 
     
     //wait PDP active
     if(ty_wait_tcpip_ok(2*60) == 0)
             ty_assert(0);
    
     SS_LOG("\r\n lwip_http_post_test 3\r\n"); 
    
    SS_LOG("params=%s",params);
    char testPram[1500]  = {0};
    memset(testPram,0x31,sizeof(testPram)-1);
    
    SS_LOG("testPram=%d",strlen(testPram));
    //char response[512] = {0};
    memset(rsp_buf,0,RSP_BUF_SIZE);
    int ret = 0;//linksdk_http_post (HTTP_FOTA_TEST_HOSTNAME, HTTP_FOTA_TEST_URI, params,rsp_buf);

    if(ret == 0)
    {
        //SS_LOG("resp=%s",resp);
        ty_send_rsp_str_to_ext(rsp_buf);
        
    }
    else
    {
        ty_send_rsp_str_to_ext("http post fail");
    }
     SS_LOG("\r\n lwip_http_post_test 4\r\n"); 
    
     //ty_work_unlock();
    
     SS_LOG("\r\n lwip_http_post_test 5\r\n"); 
    
     ty_task_delete(&g_lwipsck_Handle);
    
     SS_LOG("\r\n lwip_tcp_test 6\r\n"); 

}
#endif

void  tsm_test(void * arg) {

    //sam??ì¨μ?2?êy
   // char data[] = "{\"body\":{\"counts\":1,\"expectSW\":\"\",\"apduResult\":\"\",\"stepCode\":\"01\"},\"taskNo\":\"123\",\"imei\":\"1234512\",\"msgId\":\"202008171202570120123\",\"sign\": \"123\"}";
    //tsm??ì¨μ?2?êy
    char data[] = "{\"appAid\":\"202122232425262728292A2B2C2D2E2F\",\"body\":{\"apduResult\":\"123456\",\"expectSW\":\"9000\",\"stepCode\":\"04\"},\"msgId\":\"202008171442197414113\",\"seid\":\"19000002080000000009\",\"sign\":\"sign\",\"taskNo\":\"omrvp0oH1xuD74cU7HMk\"}";
  //  char data[] = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
    char response[2048] = { 0 };
    
//    http_post("yfzx.whty.com.cn", "/test-icbc/hfbank-interface/goods/findTemplateList", data, response);
   // http_post("yfzx.whty.com.cn", "/dev-iot-rq/services/iot-tsm/api/tsm/download", data, response);
    
    
    //int res = linksdk_http_post("yfzx.whty.com.cn", "/dev-iot-rq/services/iot-sam/api/sam/issue", data, response);
    #ifdef LINKSDK_POST_TEST
    char *resp = linksdk_http_post("yfzx.whty.com.cn", "/dev-iot-rq/services/iot-tsm/api/tsm/download", data);
    if(resp != NULL)
    {
        //SS_LOG("resp=%s",resp);
        ty_send_rsp_str_to_ext(resp);
        lpa_free(resp);
        resp = NULL;
        
    }
    else
    {
        ty_send_rsp_str_to_ext("http post fail");
    }
    #endif
    //printf("res = 0X%04X,respons:%s\r\n",res,response);

}

void sam_http_post_test(void *args)
{
                   
    ty_work_lock(1);
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
           ty_assert(0);
    SS_LOG("\r\n sam_http_post_test 3\r\n"); 

    char* url = "https://yfzx.whty.com.cn/dev-iot-rq/services/iot-sam/api/sam/issueRange";
    char* imei = "868020040115532";
    char* taskNo = (char*)args;
    SS_LOG("\r\n sam_http_post_test 4 ,taskNo = %s\r\n",taskNo); 

    ty_task_sam task = {0};
    task.app_aid = "4E9A534E6C148868";
    task.sam_url= url;
    task.retry_strategy = 1;

    SS_LOG("\r\n sam_http_post_test 5 ,mTask_type = %d\r\n",mTask_type); 
    #ifdef TY_ESIM_TSM_SAM
    int ret = ty_MgrModuleSE(&task,(char*)args , mTask_type);
    SS_LOG("\r\n ty_MgrModuleSE ret = [%d]\r\n",ret); 
    // int ret = Ty_Http_SamIssue_applet(url,imei,taskNo);
    // ty_printf("\r\n ty_download_loop ret = [%d]\r\n",ret); 
    if (ret == 0)
    {
        SS_LOG("http post success!");
    }else{
        SS_LOG("http post fail");
    }
    #endif
    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n sam_http_post_test 6\r\n"); 
}


void tsm_http_post_test(void *args)
{
    
    SS_LOG("\r\n tsm_http_post_test 1\r\n");                   
    
    ty_work_lock(1);

    SS_LOG("\r\n tsm_http_post_test 2\r\n"); 
    
    //wait PDP active
    if(ty_wait_tcpip_ok(2*60) == 0)
           ty_assert(0);
    SS_LOG("\r\n tsm_http_post_test 3\r\n"); 

    // char* taskNo = "omrvp0oH1xuD74cU7HMk";
    
    ty_task_tsm task = {0};
    task.app_aid = HTTP_SAM_TSM_AID;//"202122232425262728292A2B2C2D2E2F";
    task.tsm_url= HTTP_SAM_TSM_URL;
    task.retry_strategy = 3;

    SS_LOG("\r\n tsm_http_post_test 4 ,TSM_URL = %s\r\n",(char*)task.tsm_url); 
    int taskType = atoi((char*)args);
    SS_LOG("\r\n tsm_http_post_test 4 ,taskType = %d\r\n",taskType); 

    char* taskNo = "gF7Kxwp0OrxC70FF39Gp";
    #ifdef TY_ESIM_TSM_SAM
    // int ret = Ty_Http_TSM_Applet(HTTP_SAM_TSM_URL,aid,(char*)args);
    int ret = ty_MgrModuleTSM(&task , taskNo , taskType);

    SS_LOG("\r\n ty_MgrModuleSE ret = [%d]\r\n",ret); 
    if (ret == 0)
    {
        SS_LOG("Ty_Http_TSM_Applet success!");
    }else{
        SS_LOG("Ty_Http_TSM_Applet fail");
    }
    #endif

    ty_task_delete(&g_lwipsck_Handle);

    SS_LOG("\r\n tsm_http_post_test 6\r\n"); 
}


static void user_rtc_timeout_cb(void *para)
{
	SS_LOG("user RTC callback\n");
}

static int at_get_next_int(char **line,int *value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;

    }
    ret = at_tok_nextint(line, value); // 
    if (ret < 0)
    {
        return ret;
    } 

    return 0;
}

static int at_get_next_str(char **line,char **value)
{
    int ret;
    if (!at_tok_hasmore(line))  
    {
        return -1;
    }
    ret  = at_tok_nextstr(line, value); // ""
    if (ret < 0)
    {
        return -2;
    }

    return 0;
}

static int at_get_http_host_uri_params(char **line)
{
    int ret;
    char *url = NULL;
    do
    {
        ret = at_get_next_str (line,&url);
        if (ret != 0 || url == NULL || strlen(url) > sizeof(HTTP_FOTA_TEST_HOSTNAME)-1)
        {
            break;
        }
        SS_LOG("lwip_http_test url=%s",url);
        memset(HTTP_FOTA_TEST_HOSTNAME,0,sizeof(HTTP_FOTA_TEST_HOSTNAME));
        memcpy(HTTP_FOTA_TEST_HOSTNAME,url,strlen(url));
        
        char *uri = NULL;
        ret = at_get_next_str (line,&uri);
        if (ret != 0 || uri == NULL || strlen(uri) > sizeof(HTTP_FOTA_TEST_URI)-1)
        {
            break;
        }
        SS_LOG("lwip_http_test uri=%s",uri);
        memset(HTTP_FOTA_TEST_URI,0,sizeof(HTTP_FOTA_TEST_URI));
        memcpy(HTTP_FOTA_TEST_URI,uri,strlen(uri));
    }while(0);
    return ret;
}

static int at_get_fota_version_params(char **line)
{
    int ret;
    char *startv = NULL;
    char *targetv = NULL;
    do
    {
        ret = at_get_next_str (line,&startv);
        if (ret != 0 || startv == NULL || strlen(startv) > sizeof(start_version)-1)
        {
            break;
        }
        SS_LOG(" start_version=%s\r\n",startv);
        memset(start_version,0,sizeof(start_version));
        memcpy(start_version,startv,strlen(startv));
        
        ret = at_get_next_str (line,&targetv);
        if (ret != 0 || targetv == NULL || strlen(targetv) > sizeof(target_version)-1)
        {
            break;
        }
        SS_LOG("target_version =%s\r\n",targetv);
        memset(target_version,0,sizeof(target_version));
        memcpy(target_version,targetv,strlen(targetv));
    }while(0);
    return ret;
}


static int at_get_sigalg_value_params(char **line)
{
    int ret;
    char *url = NULL;
    do
    {
        char *sigalg = NULL;
        ret = at_get_next_str (line,&sigalg);
        if (ret != 0 || sigalg == NULL )
        {
            break;
        }
        SS_LOG("lwip_http_test sigalg=%s\r\n",sigalg);
        memset(HTTP_FOTA_SIGALG,0,sizeof(HTTP_FOTA_SIGALG));
        memcpy(HTTP_FOTA_SIGALG,sigalg,strlen(sigalg));
        
        
        char *sig = NULL;
        ret = at_get_next_str (line,&sig);
        if (ret != 0 || sig == NULL )
        {
            break;
        }
        //SS_LOG("lwip_http_test sig=%s\r\n",sig);
        memset(HTTP_FOTA_SIG,0,sizeof(HTTP_FOTA_SIG));
        memcpy(HTTP_FOTA_SIG,sig,strlen(sig));
        
        SS_LOG("lwip_http_test sig=%s\r\n",HTTP_FOTA_SIG);
        
    }while(0);
    return ret;
}

static int at_http_host_url_params(char **line)
{
    int ret;
    char *url = NULL;
    do
    {
        ret = at_get_next_str (line,&url);
        if (ret != 0 || url == NULL || strlen(url) > sizeof(HTTP_SAM_TSM_URL)-1)
        {
            break;
        }
        SS_LOG("lwip_http_test url=%s\r\n",url);
        memset(HTTP_SAM_TSM_URL,0,sizeof(HTTP_SAM_TSM_URL));
        memcpy(HTTP_SAM_TSM_URL,url,strlen(url));
        
        // char *uri = NULL;
        // ret = at_get_next_str (line,&uri);
        // if (ret != 0 || uri == NULL || strlen(uri) > sizeof(HTTP_FOTA_TEST_URI)-1)
        // {
        //     break;
        // }
        // ty_printf("lwip_http_test uri=%s",uri);
        // memset(HTTP_FOTA_TEST_URI,0,sizeof(HTTP_FOTA_TEST_URI));
        // memcpy(HTTP_FOTA_TEST_URI,uri,strlen(uri));
    }while(0);
    return ret;
}

static int at_get_nvaid_test_value_params(char **line)
{
    int ret;
    char *url = NULL;
    do
    {
        char *aid = NULL;
        ret = at_get_next_str (line,&aid);
        if (ret != 0 || aid == NULL )
        {
            break;
        }
        SS_LOG(" aid =%s",aid );
        memset(g_test_aid.app_aid,0,sizeof(g_test_aid.app_aid));
        memcpy(g_test_aid.app_aid,aid,strlen(aid));
        
        
        char *aid_ver = NULL;
        ret = at_get_next_str (line,&aid_ver);
        if (ret != 0 || aid_ver == NULL )
        {
            break;
        }
        SS_LOG("aid_ver=%s",aid_ver);
        memset(g_test_aid.app_version,0,sizeof(g_test_aid.app_version));
        memcpy(g_test_aid.app_version,aid_ver,strlen(aid_ver));

        
        SS_LOG("g_test_aid.app_version=%s",g_test_aid.app_version);
        int app_status,app_life_cycle_state;
        ret = at_get_next_int (line,&app_status);
        if (ret != 0 )
        {
            break;
        }
        g_test_aid.app_status = app_status;
        SS_LOG("app_status=%d",g_test_aid.app_status);

        ret = at_get_next_int (line,&app_life_cycle_state);
        if (ret != 0 || aid_ver == NULL )
        {
            break;
        }
        g_test_aid.app_life_cycle_state = app_life_cycle_state;
        SS_LOG("app_life_cycle_state=%d",g_test_aid.app_life_cycle_state);
    }while(0);
    return ret;
}

static void list_all_aids()
{
    Aid_List *list = NULL;
    ty_LoadAppletInfoList (&list);
    if(list)
    {
       int i =0;
       char output[256] = {0};
       if (list->aid_size <= 0 || !list->list)
       {
           ty_AppletInfoListFree (list);
           ty_send_rsp_str_to_ext("\r\n aid_size ==0\r\n");
           return ;
       }
    
       for(i = 0 ; i < list->aid_size ; i++ )
       {
           sm_aid *aid = (sm_aid *)&list->list[i];

           snprintf(output,sizeof(output),"\r\n%02X,%02X,%s,%s\r\n",
               aid->app_status,aid->app_life_cycle_state,aid->app_version,aid->app_aid);
           ty_send_rsp_str_to_ext(output);
       }
       ty_AppletInfoListFree (list);
    }
}

static void save_aid(char **line)
{
    int ret = at_get_nvaid_test_value_params (line);
    if(ret)
    {
        ty_send_rsp_str_to_ext("\r\n aid param input error\r\n");
        return;
    }
    SS_LOG("%s app_version=%s",__FUNCTION__,g_test_aid.app_version);
    
    ty_SaveAppletInfo (&g_test_aid);
    SS_LOG("%s app_version=%s",__FUNCTION__,g_test_aid.app_version);
}


//only called by user_hook_flash_restore when wakeup from deep sleep
void  test_read_user_nv_flash()
{
	//flash is NULL,use g_ty_cloud_nv default value
	if(ty_flash_check_factory())
	{
	    SS_LOG("ty_flash_check_factory ~~~~~~~~~~~~~~\r\n"); 
		//SS_LOG("+DBGINFO:ty cloud NV read is NULL\r\n");
        //AID区域初始化为全FF
        memset(&g_ty_cloud_nv.nv_aids,0xFF,sizeof(g_ty_cloud_nv.nv_aids));
        //保存预置的SAM应用AID信息
        sm_aid aid = {0};
        memcpy(aid.app_aid,SAM_AID,strlen(SAM_AID));
        memcpy(aid.app_version,"2",strlen("2"));
        aid.app_life_cycle_state = 1;
        aid.app_status = e_app_status_no_personal;
        ty_SaveAppletInfoNoNv (&aid);
	}
	else
	{
		ty_flash_read((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        unsigned int crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
        if(crc32 != g_ty_cloud_nv.crc32)
        {
            SS_LOG("crc error %x %x\r\n",crc32 ,g_ty_cloud_nv.crc32); 
            flash_status = 2;
            ty_flash_read_backup ((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
        }
        else 
        {
            flash_status = 1;
		    SS_LOG("crc32 OK\r\n"); 
        }
	}
}

void  test_save_user_nv_flash(int error)
{
	g_ty_cloud_nv.write_to_flash_num++;
    g_ty_cloud_nv.crc32 = ty_crc32(0, (unsigned char *)&g_ty_cloud_nv, 
        sizeof(g_ty_cloud_nv)-sizeof(unsigned int));
	
    ty_flash_write_backup((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    if(error)
    g_ty_cloud_nv.crc32++;
    ty_flash_write((unsigned char *)&g_ty_cloud_nv,sizeof(ty_cloud_nv_t));
    
    test_read_user_nv_flash ();
	//SS_LOG("+DBGINFO:ty cloud NV WRITE\r\n"); 
}




int  at_proc_lpa_app_sms_test(char *at_buf,char **prsp_cmd)
{
    int ret;
    int op; // 1 read ,2  write
    int smsIndex; //smsIndex
	char *line = at_buf;
    char userdata[320] = {0};
    char *pdu = NULL;
    int i;
    do
    {
        ret = at_tok_nextint(&line, &op); // 
        if (ret < 0)
        {
            ret = -1;
            break;
        }        

        switch(op)
        {
            SS_LOG("op =%d\r\n",op);
            case TEST_AT_PROFILE_PRINT:
            {
                #ifdef LPA_SMS
                ret = LpaSmsLocalInit (&localCtx);
                if(!ret)
                    lpa_sms_print_profileinfo (localCtx);
                else
                {
                    SS_LOG("LpaSmsLocalInit ERROR=%d \r\n",ret);
                }
                #endif
                break;
            }

        //SS_LOG("op =%d\r\n",op);
        case TEST_AT_PDU_SMS_WRITE:
        {
            ret = at_get_next_str (&line,&pdu);
            if (ret != 0)
            {
                break;
            }   
            SS_LOG("pdu len=%d\r\n",strlen(pdu));
            SS_LOG("pdu =%s \r\n",pdu);
            #ifdef LPA_SMS
            ret = Ty_Imp_SmsPduWrite (pdu);
            SS_LOG("Ty_Imp_SmsPduWrite =%d \r\n",ret);
            #endif
            break;
        }
        case  TEST_AT_PDU_SMS_READ:
        {
            ret = at_get_next_int (&line,&smsIndex);
            if (ret != 0)
            {
                break;
            }
            SS_LOG("smsIndex =%d\r\n",smsIndex);
            #ifdef LPA_SMS
            ret = Ty_Imp_SmsPduRead (smsIndex,userdata);
            if(ret != 0)
            {
                SS_LOG("read fail=%d\r\n",ret);
                break;
            }
            #endif
            printf_hex_buf (userdata);
            break;
        }
        case  TEST_AT_HTTPS_POST_SPLIT:
        {
            char *url = NULL;
            ret = at_get_next_str (&line,&url);
            if (ret != 0 || url == NULL || strlen(url) > sizeof(HTTP_FOTA_TEST_HOSTNAME)-1)
            {
                break;
            }

            SS_LOG("lwip_http_test url=%s\r\n",url);
            memset(HTTP_FOTA_TEST_HOSTNAME,0,sizeof(HTTP_FOTA_TEST_HOSTNAME));
            memcpy(HTTP_FOTA_TEST_HOSTNAME,url,strlen(url));

            char *uri = NULL;
            ret = at_get_next_str (&line,&uri);
            if (ret != 0 || uri == NULL || strlen(uri) > sizeof(HTTP_FOTA_TEST_URI)-1)
            {
                break;
            }
            SS_LOG("lwip_http_test uri=%s\r\n",uri);
            memset(HTTP_FOTA_TEST_URI,0,sizeof(HTTP_FOTA_TEST_URI));
            memcpy(HTTP_FOTA_TEST_URI,uri,strlen(uri));

            ret = at_get_next_int (&line,&FOTA_PACKET_SZ);
            if (ret != 0 || FOTA_PACKET_SZ < 0 || FOTA_PACKET_SZ > 2048)
            {
                FOTA_PACKET_SZ = 512;
                break;
            }
            SS_LOG("lwip_http_test FOTA_PACKET_SZ=%d\r\n",FOTA_PACKET_SZ);

            ret = at_get_sigalg_value_params (&line);
            if (ret != 0 )
            {
                break;
            }
            
            ty_task_create((ty_task_func_t)lwip_http_test_tsm_cap,"lwip_http_test_tsm",4096*2,NULL,10,&g_lwipsck_Handle);
            break;
        }
        case TEST_AT_FOTA_FW_DOWNLOAD_SPLIT:
        {           
            
            SS_LOG("*************TEST**********\r\n");
            char *url = NULL;
            ret = at_get_next_str (&line,&url);
            if (ret != 0 || url == NULL || strlen(url) > sizeof(HTTP_FOTA_TEST_HOSTNAME)-1)
            {
                break;
            }
            SS_LOG("lwip_http_test url=%s\r\n",url);
            memset(HTTP_FOTA_TEST_HOSTNAME,0,sizeof(HTTP_FOTA_TEST_HOSTNAME));
            memcpy(HTTP_FOTA_TEST_HOSTNAME,url,strlen(url));

            char *uri = NULL;
            ret = at_get_next_str (&line,&uri);
            if (ret != 0 || uri == NULL || strlen(uri) > sizeof(HTTP_FOTA_TEST_URI)-1)
            {
                break;
            }
            SS_LOG("lwip_http_test uri=%s\r\n",uri);
            memset(HTTP_FOTA_TEST_URI,0,sizeof(HTTP_FOTA_TEST_URI));
            memcpy(HTTP_FOTA_TEST_URI,uri,strlen(uri));

            ret = at_get_next_int (&line,&FOTA_PACKET_SZ);
            if (ret != 0 || FOTA_PACKET_SZ < 0 || FOTA_PACKET_SZ > 8192)
            {
                FOTA_PACKET_SZ = 512;
                break;
            }
            SS_LOG("lwip_http_test FOTA_PACKET_SZ=%d\r\n",FOTA_PACKET_SZ);

            ret = at_get_sigalg_value_params (&line);
            if (ret != 0 )
            {
                break;
            }
             SS_LOG("*************TEST**********\r\n");
            ty_task_create((ty_task_func_t)lwip_http_test,"lwip_http_test",4096*2,NULL,10,&g_lwipsck_Handle);
            break;
        }
        case TEST_AT_TASK_DOWN_JSON_PARSER:
        {
            char *json = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"task_no\":\"no1\",\"task_type\":\"1\",\"trans_data\":{\"FOTA_param\":{\"start_version\":\"1.11\",\"target_version\":\"1.1\",\"FOTA_URL\":\"https://ds.xjpeng.cn:9998/xyDelta\",\"sign_type\":\"1\",\"sign_value\":\"1f449283e12c3b30c25a9ed341036561\",\"retry_strategy\":\"3\"}}}}";
            ty_cloudmsg_task_down task = {0}; 
            ret = ty_cloud_task_down_parser (json, &task);
            SS_LOG("ty_cloud_task_down_parser ret=%d",ret);
            break;
        }
        case TEST_AT_PRINT_COMPILE_DATETIME:
        {
            SS_LOG("BUILD:%s %s\r\n",__DATE__,__TIME__);
            break;
        }

        case TEST_AT_LPA_OPTIMIZE_TASK:
        {
            #ifdef LPA_OPTIMIZE
            SS_LOG("ty_optimize_init test\r\n");
            ty_task_create((ty_task_func_t)at_lpa_app_optimize,"at_lpa_app_optimize",4096*2,NULL,10,&g_lpa_app_Handle);
            #endif
            break;
        }      
        case TEST_AT_MBEDTLS_DEBUG_CONFIG:
        {
            int threshold = 0;
            ret = at_get_next_int (&line,&threshold);
            if (ret != 0)
            {
                break;
            }        
            SS_LOG("debug_set_threshold threshold=%d\r\n",threshold);
            #ifdef TY_MBEDTLS_CA
            #ifdef MBEDTLS_DEBUG_C
            mbedtls_debug_set_threshold (threshold);
            #endif
            #endif
            break;
        } 

        case TEST_AT_DSP_LOCK:
        {
            SS_LOG("ty_work_lock test\r\n");
            int lock = 0;
            ret = at_get_next_int (&line,&lock);
            if (ret != 0)
            {
                break;
            }          
            SS_LOG("ty_work_lock lock=%d\r\n",lock);
            ty_work_lock(lock);
            break;
        }
        case TEST_AT_DSP_UNLOCK:
        {
            ty_work_unlock ();
            SS_LOG("ty_work_unlock test\r\n");
            break;
        }
        case TEST_AT_RTC_WAKUP_FUCNTION:
        {
//            ty_rtc_timer_create(RTC_TIMER_USER1,USER_RTC_OFFSET,user_rtc_timeout_cb,NULL);
            //ty_fast_power_off(0);
            break;
        }
        case TEST_AT_PRINT_MODULE_INFO:
        {
            ty_module_inf info = {0};
            ty_LoadModueInfo (&info);
            SS_LOG("imei=%s\r\n",info.bind_imei);
            SS_LOG("firmware_version = %s\r\n",info.firmware_version);
            SS_LOG("product_type = %s\r\n",info.product_type);
            break;
        }
        case 15:
        {
            char *fwver = NULL;
            ret = at_get_next_str (&line,&fwver);
            if (ret != 0 || fwver == NULL)
            {
                break;
            }
            SS_LOG("fwver=%s\r\n",fwver);
            ty_module_inf info = {0};
            info.firmware_version = fwver;
            ret = ty_UpdateModueInfo (&info);
            SS_LOG("ty_UpdateModueInfo=%d\r\n",ret);
            break;
        }
        case TEST_AT_LOAD_DEVICESECRET_INFO:
        {
            char deviceSecret[64] = {0};
            if(ty_LoadDeviceSecret(deviceSecret) != 0)
               break;
            SS_LOG("deviceSecret=%s\r\n",deviceSecret);
            break;
        }
        case TEST_AT_UPDATE_DEVICESECRET_INFO:
        {
            char *devsct = NULL;
            char deviceSecret[64] = {0};
            ret = at_get_next_str (&line,&devsct);
            if (ret != 0 || devsct == NULL)
            {
                break;
            }
            memcpy(deviceSecret,devsct,strlen(devsct));
            ret = ty_SaveDeviceSecret(deviceSecret);
            SS_LOG("ty_SaveDeviceSecret=%d\r\n",ret);
            break;
        }
        case 18:
        {
            SS_LOG("18 == op\r\n");
            //ty_task_create((ty_task_func_t)ty_queue_task_loop,"ty_queue_task",0x2000,NULL,11,&g_lpa_app_QueueHandle);
            //ty_task_create((ty_task_func_t)ty_main_task_loop,"ty_main_task",0x2000,(void*)&_maintask,10,&g_lpa_app_Handle);
            break;
            //
        }
        case TEST_AT_MQTT_REPORT_UP:
        {
            int ret = ty_at_mqtt_report_up ();
            SS_LOG("ty_at_mqtt_report_up=%d\r\n",ret);
            break;
        }
        case TEST_AT_PUT_MSG_TO_QUEUE_TASK:
        {
            char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
            ty_queue_msg msg = {0};
            msg.type = E_QueueMsg_Local;
            
            char *data = lpa_calloc (1, strlen(pub_payload)+1);
            memcpy(data,pub_payload,strlen(pub_payload));
            msg.msg.localmsg.data = data;
            msg.msg.localmsg.type = E_LocalMsg_TestUP;
            SS_LOG("ty_queue_task_put_msg \n");
            ty_queue_task_put_msg(&msg);
            break;
            
        } 
        case TEST_AT_PUT_MSG_TO_MAIN_TASK:
        {
            char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
            ty_queue_msg msg = {0};
            msg.type = E_QueueMsg_Local;
            char *data = lpa_calloc (1, strlen(pub_payload)+1);
            memcpy(data,pub_payload,strlen(pub_payload));
            msg.msg.localmsg.data = data;
            msg.msg.localmsg.type = E_LocalMsg_TestUP;
            ty_main_task_put_taskup_msg (&msg);
            break;
        } 
        case TEST_AT_HTTPS_POST_KEEPALIVE:
        {
            ret = at_get_http_host_uri_params (&line);
            if (ret != 0)
            {
                break;
            }
            char *params = NULL;
            ret = at_get_next_str (&line,&params);
            if (ret != 0 || params == NULL )
            {
                break;
            }
            memset(HTTP_PARAM_INPUT,0,sizeof(HTTP_PARAM_INPUT));
            memcpy(HTTP_PARAM_INPUT,params,strlen(params));
            ty_task_create((ty_task_func_t)lwip_http_post_test,"lwip_http_post_test",4096*2,(void*)HTTP_PARAM_INPUT,10,&g_lwipsck_Handle);
            break;
        }
        case TEST_AT_SAM_HTTPS:
        {
            SS_LOG("23 == op\n");
            char *taskNo = NULL;
            int ret = at_get_next_str (&line,&taskNo);
            if (ret != 0 || taskNo == NULL)
            {
                break;
            }
            SS_LOG("taskNo == %s\n",taskNo);
            memset(HTTP_PARAM_INPUT,0,sizeof(HTTP_PARAM_INPUT));
            memcpy(HTTP_PARAM_INPUT,taskNo,strlen(taskNo));
            ret = at_get_next_int (&line,&mTask_type);
            if (ret != 0 )
            {
                break;
            }
            SS_LOG("mTask_type == %d\n",mTask_type);
            ty_task_create((ty_task_func_t)sam_http_post_test,"sam_http_post_test",4096*2,(void*)HTTP_PARAM_INPUT,10,&g_lwipsck_Handle); 
            break;
        }
        case TEST_AT_TSM_HTTPS:
        {
            SS_LOG("TEST_AT_TSM_HTTPS == op\r\n");
            // tsm_http_post_test(NULL);
            ret = at_http_host_url_params (&line);
            if (ret != 0)
            {
                break;
            }

            char *taskNo = NULL;
            ret = at_get_next_str (&line,&taskNo);
            if (ret != 0 || taskNo == NULL)
            {
                break;
            }

                        
            memset(HTTP_PARAM_INPUT,0,sizeof(HTTP_PARAM_INPUT));
            memcpy(HTTP_PARAM_INPUT,taskNo,strlen(taskNo));
            
            SS_LOG("tsm_http_post_test taskNo = %s\r\n",taskNo);
            int task_type = 4;
            ret = at_get_next_int (&line,&task_type);
            if (ret != 0 || taskNo == NULL)
            {
                break;
            }
            SS_LOG("tsm_http_post_test task_type = %d\r\n",task_type);


            char *AID = NULL;
            ret = at_get_next_str (&line,&AID);
            if (ret != 0 || AID == NULL)
            {
                break;
            }
            SS_LOG("tsm_http_post_test AID = %s\r\n",AID);

            memset(HTTP_SAM_TSM_AID,0,sizeof(HTTP_SAM_TSM_AID));
            memcpy(HTTP_SAM_TSM_AID,AID,strlen(AID));

            ty_queue_msg msg = {0};
            ty_cloudmsg_task_down task = {0};
            static char taskNoS[64] = {0};
            memset(taskNoS,0,sizeof(taskNoS));
            memcpy(taskNoS,taskNo,strlen(taskNo));
            task.id = "1";
            task.version = "1.0";
            task.task_No = taskNoS;
            task.task_type = task_type;
            task.root = NULL;
            task.params.tsm.app_aid = HTTP_SAM_TSM_AID;//"202122232425262728292A2B2C2D2E2F";
            task.params.tsm.retry_strategy = 1;
            task.params.tsm.session_key= "11";
            task.params.tsm.tsm_url= HTTP_SAM_TSM_URL;
            msg.type = E_QueueMsg_Cloud;
            msg.msg.cloudmsg = task;
            ty_queue_task_put_msg (&msg);
            break;
            //ty_task_create((ty_task_func_t)tsm_http_post_test,"tsm_http_post_test",4096*2,(void*)HTTP_PARAM_INPUT,10,&g_lwipsck_Handle); 
        }
        case TEST_AT_GET_APPLET_STATUS_VERSION:
        {
            char *aid = NULL;
            ret = at_get_next_str (&line,&aid);
            if (ret != 0 || aid == NULL)
            {
                break;
            }
            SS_LOG("aid = %s\r\n",aid);
            memset(HTTP_PARAM_INPUT,0,sizeof(HTTP_PARAM_INPUT));
            memcpy(HTTP_PARAM_INPUT,aid,strlen(aid));

            char seid[64] = {0};
            memset(seid , 0 , 64);
            #ifdef TY_ESIM_TSM_SAM
            Ty_getSeid(seid);
            SS_LOG("Ty_getSeid seid = %s\r\n",seid);


            int appState;
            char version[20] = {0};
            ret = Ty_getAppVersion_appState(HTTP_PARAM_INPUT , version , &appState);
            SS_LOG("Ty_getAppVersion_appState ret = %d\r\n",ret);
            if (ret == 0)
            {
                SS_LOG("appState = %d ,version = %s\r\n",appState,version);
                UCHAR stateBuff = appState;
                SS_LOG("stateBuff = %d ",stateBuff);
            }

            int space;
            ret = Ty_getRestSpace(&space);
             SS_LOG("Ty_getRestSpace ret = %d\r\n",ret);
            if (ret == 0)
            {
                SS_LOG("space = %d\r\n",space);
            }
            #endif
            break;
        }
        case 26:
        {
            SS_LOG("===> 26 == op \r\n");
            #ifdef TY_ESIM_TSM_SAM
            char apduStr[] = "01A4040008A000000003000000#8150000008C61612536B621E4A#";
            char expectStr[] = "9000#9000#9000#9000#9000#9000";

            TArrayList mArrayList = {0};
            int ret = split(expectStr , "#" , &mArrayList);
            SS_LOG("split ret = %d ,mArrayList size = [%d]\r\n",ret , mArrayList.size);
            freeStringList(&mArrayList);
            #endif
            break;
        }
        case TEST_AT_HTTPS_POST_KEEPALIVE_CLOSE:
        {
            if(http_hd)
                ty_http_close (http_hd);
            http_hd = NULL;
            SS_LOG("ty_http_close \r\n");
            break;
        }
        case TEST_AT_PRINT_AID_INFO:
        {   
            list_all_aids ();
            break;
        }
        case TEST_AT_SAVE_AID_INFO:
        {   
            save_aid(&line);
            break;
        }
        case TEST_AT_ALIOS_HTTP_POST:
        {   
            ret = at_get_http_host_uri_params (&line);
            if (ret != 0)
            {
                break;
            }
            char *params = NULL;
            ret = at_get_next_str (&line,&params);
            if (ret != 0 || params == NULL )
            {
                break;
            }
            
            memset(HTTP_PARAM_INPUT,0,sizeof(HTTP_PARAM_INPUT));
            memcpy(HTTP_PARAM_INPUT,params,strlen(params));
            #ifdef ALIOS_HTTP
            ty_task_create((ty_task_func_t)lwip_ali_http_post_test,"lwip_http_post_test",4096*2,(void*)HTTP_PARAM_INPUT,10,&g_lwipsck_Handle);
            #endif
            break;
        }
        case TEST_AT_LINKSDK_HTTP_POST:
        {
            ty_task_create((ty_task_func_t)tsm_test,"tsm_test",4096*2,(void*)HTTP_PARAM_INPUT,10,&g_lwipsck_Handle);
            break;
        }
        case TEST_AT_LOADSIM_PROFILE:
        {
            ty_module_SIM * moduleSIMList = NULL;
            ty_LoadModuleSIM (&moduleSIMList);
            if(!moduleSIMList)
            {   
                SS_LOG("ty_LoadModuleSIM fail\n");
                break;
            }
            int i;
            SS_LOG("ty_LoadModuleSIM size=%d\n",moduleSIMList->size);
            for(i = 0;i< moduleSIMList->size;i++)
            {
                SS_LOG("bind_iccid=%s \n",moduleSIMList->code_no_list[i].bind_iccid);
                SS_LOG("use_iccid=%d \n",moduleSIMList->code_no_list[i].use_iccid);
                SS_LOG("write_iccid=%d \n",moduleSIMList->code_no_list[i].write_iccid);
            }

            ty_LoadModuleSIMFree (moduleSIMList);
            moduleSIMList = NULL;
            break;
        }
        case TEST_AT_ERASE_ALLAID_INFOS:
        {
            ty_EraseAllAppletInfo ();
            break;
        }
        case TEST_AT_DUMP_MEMLEAK_INFO:
        {
#if MEMLEAK_CHECK
            dbg_mem_stat();
            dbg_heap_dump("");
#endif
            break;
        }
        case TEST_AT_MALLOC_ERR_TEST:
        {
            int i = 0;
            while(1)
            {
                lpa_malloc(5);
                SS_LOG("lpa_malloc=%d \r\n",i++);
            }
            break;
        }
        case TEST_AT_PUT_TSMSAM_TASK_UP_TO_MAINTASK:
        {

            SS_LOG("put_samtsm_task_up_msg \n");
            ret = put_samtsm_task_up_msg ("123", "1.0", "12346", 4, 0);
            SS_LOG("put_samtsm_task_up_msg ret=%d \r\n",ret);
            break;
        }
        case TEST_AT_PUT_FOTATASK_TO_QUEUETASK:
        {
            SS_LOG("fota test\r\n");
            ret = at_get_http_host_uri_params (&line);
            if (ret != 0)
            {
                SS_LOG(" at_get_http_host_uri_params error\r\n");
                break;
            }
            ret = at_get_fota_version_params (&line);
            if (ret != 0)
            {
                SS_LOG(" at_get_fota_version_params error\r\n");
                break;
            }
            ty_queue_msg msg = {0};
            ty_cloudmsg_task_down task = {0};
            static char taskNoS[64] = {0};
            memset(taskNoS,0,sizeof(taskNoS));
            memcpy(taskNoS,"123",strlen("123"));
            task.id = "1";
            task.version = "1.0";
            task.task_No = taskNoS;
            task.task_type = 1;
            task.root = NULL;
            task.params.fota.fota_url = HTTP_FOTA_TEST_HOSTNAME;
            task.params.fota.modrsp_url = HTTP_FOTA_TEST_URI; 
            task.params.fota.start_version = start_version;
            task.params.fota.target_version = target_version;
            task.params.fota.retry_strategy = 3;
            task.params.fota.sign_type = 1;
            task.params.fota.sign_value= "4a66aefd186067a6ee9752b0c66323f1";
            msg.type = E_QueueMsg_Cloud;
            msg.msg.cloudmsg = task;
            ty_queue_task_put_msg (&msg);
            break;
        }
        case TEST_AT_MQTT_OFFLINE:
        {
           ty_at_mqtt_offline();
           break;
        }
        case TEST_AT_MQTT_ONLINE:
        {
           ret = ty_at_mqtt_resume();
           SS_LOG("ty_at_mqtt_resume ret=%d\r\n",ret);
           break;
        }
        case 41:
        {
            SS_LOG("flash_status=%d\r\n",flash_status);
            break;
        }
        case 42:
        {
            test_read_user_nv_flash ();
            SS_LOG("test_read_user_nv_flash flash_status=%d\r\n",flash_status);
            break;
        }
        case 43:
        {
            int error = 0;
            ret = at_get_next_int (&line,&error);
            test_save_user_nv_flash ( error);
            SS_LOG("test_save_user_nv_flash flash_status=%d\r\n",flash_status);
            break;
        }
        case 44:
        {
            save_user_nv_flash ();
            SS_LOG("save_user_nv_flash flash_status=%d\r\n",flash_status);
            break;
        }
        case 45:
        {
            g_ty_cloud_nv.g_cur_state++;
            save_user_nv_bk_flash ();
            SS_LOG("save_user_nv_bk_flash flash_status=%d\r\n",flash_status);
            break;
        }  
        case 46:
        {
            ret = malloc_free_test();
            SS_LOG("malloc_free_test ret= %d\r\n", ret);
            break;
        }
        case 47:
        {
            SS_LOG("***********TASK TEST************\r\n");
            int task_index = 0;
  
            ret = at_get_next_int (&line,&task_index);
            if (ret != 0)
            {
                break;
            }
   
            SS_LOG("task_index = %d\r\n",task_index);
            ptest_app_task(task_index);
            SS_LOG("*********END TEST************\r\n");
            break;

        }
       case 48:
        {
            SS_LOG("***********TEST************\r\n");
            ssize_t index = 0;
            ret = at_get_next_int (&line,&index);
            if (ret != 0)
            {
                break;
            }
            ptest_get_rtc_time(index);
            SS_LOG("*********END TEST************\r\n");
            break;

        }

        case 49:
        {
            SS_LOG("***********TIMER TEST************\r\n");
            ssize_t index = 0;
            ssize_t set_timer = 0;
            
            ret = at_get_next_int (&line, &index);
            if (ret != 0)
            {
                break;
            }

            test_system_ptimer(index);
            SS_LOG("*********END TEST************\r\n");
            break;

        }
        case 50:
        {
            SS_LOG("***********QUEUE TEST************\r\n");
            //            ssize_t index = 0;
            //            ssize_t set_timer = 0;

            //Queue_Test();

            SS_LOG("*********END TEST************\r\n");
            break;

        }
         case 51:
        {
            SS_LOG("***********QUEUE TEST************\r\n");
            SS_LOG("***********QUEUE TEST************\r\n");



            //            ssize_t index = 0;
            //            ssize_t set_timer = 0;

             filesystem_test();

            SS_LOG("*********END TEST************\r\n");
            break;

        }
        case 52:
            {

            SS_LOG("***********UPDATE TEST************\r\n");
            SS_LOG("***********UPDATE TEST************\r\n");

            break;
        }
        case 53:
            {
             SS_LOG("***********MUTEX TEST************\r\n");
                ssize_t m_index = 0;
                
                ret = at_get_next_int (&line, &m_index);
                if (ret != 0)
                {
                    break;
                }

                mutex_test(m_index);
                SS_LOG("***********END TEST ************\r\n");
                break;
            }
        case 54:
                SS_LOG("***********MQTT TEST************\r\n");
                //mqtt_test();
                SS_LOG("***********END TEST ************\r\n");
                break;
        case 55:
                SS_LOG("***********HTTP TEST************\r\n");
                //http_test();
                SS_LOG("***********END TEST************\r\n");
                break;
             
        
       }
    }while(0); 
	return 0;
}

void ty_at_test_entry()
{
    ty_register_app_at_req("AT+SMSTEST",at_proc_lpa_app_sms_test);
}
