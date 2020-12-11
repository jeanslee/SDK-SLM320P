/**
 * @file demo.c
 *
 */

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'P', 'C')

/*********************
 *      INCLUDES
 *********************/
#include "ats_config.h"
#include "meig.h"
#include "cfw.h"
#include "cfw_event.h"
#include "osi_api.h"
#include <stdio.h>
#include <time.h>
#include "audio_player.h"
#include "audio_recorder.h"
#include "audio_decoder.h"
#include "audio_reader.h"
#include "audio_writer.h"
#include "nvm.h"

#define BT_SUCCESS 0
#define BT_ERROR 1
#define BT_PENDING 2
#define BT_BUSY 3
#define BT_NO_RESOURCE 4
#define BT_NOT_FOUND 5
#define BT_DEVICE_NOT_FOUND 6
#define BT_CONNECTION_FAILED 7
#define BT_TIMEOUT 8
#define BT_NO_CONNECTION 9
#define BT_INVALID_PARM 10
#define BT_NOT_SUPPORTED 11
#define BT_CANCELLED 12

#define BT_IN_PROGRESS 19
#define BT_RESTRICTED 20
#define BT_INVALID_HANDLE 30
#define BT_PACKET_TOO_SMALL 31
#define BT_NO_PAIR 32

#define BT_FAILED 0x41
#define BT_DISCONNECT 0x43
#define BT_NO_CONNECT 0x44
#define BT_IN_USE 0x45
#define BT_MEDIA_BUSY 0x46

#define BT_OFF 0x50

#define APP_BT_MSG 0
#define APP_BLE_MSG 1

//--------------------------------- Classic ------------------------------------

#define APP_BT_MSG_FLAG 0x80000000
#define APP_BT_MSG_BASE 0x00550000

#define APP_BT_ME_ON_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 1))
#define APP_BT_ME_OFF_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 2))
#define APP_BT_VISIBILE_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 3))
#define APP_BT_HIDDEN_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 4))
#define APP_BT_SET_LOCAL_NAME_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 5))
#define APP_BT_SET_LOCAL_ADDR_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 6))
#define APP_BT_INQ_DEV_NAME (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 7))
#define APP_BT_INQ_COMP_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 8))
#define APP_BT_INQUIRY_CANCEL (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 9))
#define APP_BT_DEV_PAIR_COMPLETE (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 10))
#define APP_BT_DELETE_DEVICE_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 11))
#define APP_BT_DEV_PIN_REQ (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 12))
#define APP_BT_SSP_NUM_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 13))
#define APP_BT_SPP_CONNECT_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 14))
#define APP_BT_SPP_DISCONNECT_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 15))
#define APP_BT_SPP_DATA_RECIEVE_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 16))


static lv_obj_t *gPages[PAGE_MAX];
static lv_obj_t *ok;
static lv_obj_t *bck;
static lv_obj_t *img_ok;
static lv_obj_t *ltime;
static lv_obj_t *sig;
static lv_obj_t *bat;
static lv_obj_t *f4g;
static lv_obj_t * list = NULL;
static lv_obj_t * list2;
static lv_obj_t *btn_group;
static lv_obj_t *btn_setting;
static lv_group_t *gGrp;
static lv_task_t * ui_task; 
static lv_style_t *gCstyle;
static lv_obj_t *warnLabel;

static bool show = true;
static bool incall= false;
static int prePage = PAGE_HOME;
static int curPage = PAGE_HOME;
uint8_t gStatus[3]={0};
static uint8_t gSettingGroupIndex;

static uint8_t gPwrTick;
static bool gKeyTest= false;

LV_IMG_DECLARE(png_ok);
LV_IMG_DECLARE(png_back);
LV_IMG_DECLARE(png_signal_none);
LV_IMG_DECLARE(png_signal);
LV_IMG_DECLARE(png_signal_1);
LV_IMG_DECLARE(png_signal_2);
LV_IMG_DECLARE(png_signal_3);
LV_IMG_DECLARE(png_battery_10);
LV_IMG_DECLARE(png_battery_20);
LV_IMG_DECLARE(png_battery_30);
LV_IMG_DECLARE(png_battery_40);
LV_IMG_DECLARE(png_battery_50);
LV_IMG_DECLARE(png_battery_60);
LV_IMG_DECLARE(png_battery_70);
LV_IMG_DECLARE(png_battery_80);
LV_IMG_DECLARE(png_battery_90);
LV_IMG_DECLARE(png_battery_100);
LV_IMG_DECLARE(png_member);
LV_IMG_DECLARE(png_group);
LV_IMG_DECLARE(png_setting);
LV_IMG_DECLARE(png_group_title);
LV_IMG_DECLARE(png_group_icon);
LV_IMG_DECLARE(png_call);
LV_IMG_DECLARE(png_call_left);
LV_IMG_DECLARE(png_call_right);
LV_FONT_DECLARE(cjk)        /*Declare a font*/
//LV_FONT_DECLARE(chinese_16)        /*Declare a font*/

static void handleBckKey(){
    int st = curPage;//mg_get_state();
    if(st == PAGE_SETTING){
        mg_switch_main();
    }else if(st == PAGE_CGROUP){
        mg_switch_main();
    }else if(st == PAGE_GROUP){
        mg_switch_cgroup();
    }
}
static void handleEnterKey(){
    int st = curPage;//mg_get_state();

}
static bool handleAnswerKey(){
    int st = curPage;//mg_get_state();

    return false;
}
static bool handlePowerKey(){
    int st = curPage;//mg_get_state();

    return false;
}
static void btn_click_handler(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED) {
        int st = curPage;//mg_get_state();
        OSI_LOGD(0, "btn_click_handler state:%x",st);
        if(btn == bck){
            if(st == PAGE_SETTING){
                mg_switch_main();
            }else if(st == PAGE_CGROUP){
                mg_switch_main();
            }else if(st == PAGE_GROUP){
                mg_switch_cgroup();
            }        
        }
        else if(btn == ok){
        }
    }
}

static void clearanim(lv_anim_t * a){
	lv_obj_del(a->var);
}

static void showMessageBox(lv_obj_t *parent,char *msg){
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_obj_t * mbox = lv_mbox_create(parent, NULL);     
    //lv_mbox_set_style(mbox,
    lv_mbox_set_text(mbox, msg);     
    lv_obj_set_width(mbox, hres - LV_DPI);     
    static const char * mbox_btns[] = {"OK", ""};     
    //lv_mbox_add_btns(mbox, mbox_btns);    /*The default action is close*/     
    lv_obj_align(mbox, parent, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);
    lv_mbox_start_auto_close(mbox,3000);
}

static void showMessageBox1s(lv_obj_t *parent,char *msg){
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_obj_t * mbox = lv_mbox_create(parent, NULL);     
    //lv_mbox_set_style(mbox,
    lv_mbox_set_text(mbox, msg);     
    lv_obj_set_width(mbox, hres - LV_DPI);     
    static const char * mbox_btns[] = {"OK", ""};     
    //lv_mbox_add_btns(mbox, mbox_btns);    /*The default action is close*/     
    lv_obj_align(mbox, parent, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);
    lv_mbox_start_auto_close(mbox,1000);
}

static void showKeyMessage(lv_obj_t *parent,int key){

	 OSI_LOGD(0, "showKeyMessage event:%d",key);
    //lv_mbox_set_style(mbox,
    char msg[16]={0};
    switch(key){

        case LV_KEY_0:
            sprintf(msg,"0");
            break;
        case LV_KEY_1:
            sprintf(msg,"1");
            break;
        case LV_KEY_2:
            sprintf(msg,"2");
            break;
        case LV_KEY_3:
            sprintf(msg,"3");
            break;
        case LV_KEY_4:
            sprintf(msg,"4");
            break;
        case LV_KEY_5:
            sprintf(msg,"5");
            break;
        case LV_KEY_6:
            sprintf(msg,"6");
            break;
        case LV_KEY_7:
            sprintf(msg,"7");
            break;
        case LV_KEY_8:
            sprintf(msg,"8");
            break;
        case LV_KEY_9:
            sprintf(msg,"9");
            break;
 	case LV_KEY_A:
            sprintf(msg,"A");
            break;
 	case LV_KEY_B:
            sprintf(msg,"B");
            break;			
 	case LV_KEY_C:
            sprintf(msg,"C");
            break;
 	case LV_KEY_D:
            sprintf(msg,"D");
            break;
 	case LV_KEY_E:
            sprintf(msg,"E");
            break;
 	case LV_KEY_F:
            sprintf(msg,"F");
            break;								
        case LV_KEY_DEL:
            sprintf(msg,"Delete");
            break;
        case LV_KEY_DOT:
            sprintf(msg,"Dot");
            break;
        case LV_KEY_PLUS:
            sprintf(msg,"Plus");
            break;
        case LV_KEY_ENTER:
            sprintf(msg,"Enter");
            break;
        case LV_KEY_BACK:
            sprintf(msg,"Back");
            break;
        case LV_KEY_MONEY:
            sprintf(msg,"Money");
            break;
        case LV_KEY_SCAN:
            sprintf(msg,"Scan");
            break;
        case LV_KEY_STORE:
            sprintf(msg,"Store");
            break;
		case LV_KEY_POWER:
            sprintf(msg,"Power");
            break;
		case LV_KEY_NEXT:  //9
#if defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320P) || defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320)	
		sprintf(msg,"Next");
#else
            sprintf(msg,"2");
#endif
			lv_group_focus_prev(gGrp);
            break;		
		case LV_KEY_PREV:  //11
#if defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320P) || defined(CONFIG_BOARD_MG_MODULE_NAME_SLM320)	
		sprintf(msg,"Prev");
#else		
            sprintf(msg,"8");
#endif
			lv_group_focus_next(gGrp);
            break;
	case LV_KEY_LEFT:
            sprintf(msg,"Left");
            break;
	case LV_KEY_RIGHT:
            sprintf(msg,"Right");
            break;

	case LV_KEY_UP:
            sprintf(msg,"Up");
            break;
	case LV_KEY_DOWN:
            sprintf(msg,"Down");
            break;
	case LV_KEY_HOME:		
            sprintf(msg,"Home");
            break;
        default:
            return;
    }
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_obj_t * mbox = lv_mbox_create(parent, NULL);     
    lv_mbox_set_text(mbox, msg);     
    lv_obj_set_width(mbox, hres - LV_DPI);     
    //static const char * mbox_btns[] = {"OK", ""};     
    //lv_mbox_add_btns(mbox, mbox_btns);    /*The default action is close*/     
    lv_obj_align(mbox, parent, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);
    lv_mbox_start_auto_close(mbox,1000);
}

static void show_settings(lv_obj_t*btn,lv_event_t event){
    if(event == LV_EVENT_PRESSED){
        mg_switch_setting();
    }
}

static void show_group(lv_obj_t*btn,lv_event_t event){
    if(event == LV_EVENT_PRESSED){
        mg_switch_group(btn->index);
    }
}

static void show_cgroup(lv_obj_t*btn,lv_event_t event){
    if(event == LV_EVENT_PRESSED){
        mg_switch_cgroup();
    }
}

static void item_click_handler(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED){
        OSI_LOGD(0, "item_click_handler event:%x",event);
        uint8_t index = btn->index;
    }
}

static void gitem_click_handler(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED){
        OSI_LOGD(0, "gitem_click_handler event:%x",event);
        mg_switch_group(btn->index);
    }
}

static void setting_click_handler(lv_obj_t * btn, lv_event_t event)
{

    OSI_LOGD(0, "setting_click_handler event:%x",event);
    if(event == LV_EVENT_PRESSED){
        gSettingGroupIndex=btn->index;

    }
}

#define EXAMPLE_FILE_NAME "/example.pcm"
static void prvRecordWriter(void)
{
    auWriter_t *writer = (auWriter_t *)auFileWriterCreate(EXAMPLE_FILE_NAME);
    auRecorder_t *recorder = auRecorderCreate();
    auRecorderStartWriter(recorder, AUDEV_RECORD_TYPE_MIC, AUSTREAM_FORMAT_PCM, NULL, writer);

    osiThreadSleep(3000);

    auRecorderStop(recorder);
    auRecorderDelete(recorder);
    auWriterDelete(writer);

    int file_size = vfs_file_size(EXAMPLE_FILE_NAME);
    OSI_LOGI(0, "record writer file size/%d", file_size);
}
static void prvPlayFile(void)
{
    auPlayer_t *player = auPlayerCreate();
    auFrame_t frame = {.sample_format = AUSAMPLE_FORMAT_S16, .sample_rate = 8000, .channel_count = 1};
    auDecoderParamSet_t params[2] = {{AU_DEC_PARAM_FORMAT, &frame}, {0}};
    auPlayerStartFile(player, AUSTREAM_FORMAT_PCM, params, EXAMPLE_FILE_NAME);

    auPlayerWaitFinish(player, OSI_WAIT_FOREVER);
    auPlayerDelete(player);
}

static void btMsgHandler(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    if (pEvent == NULL)
        return;

    OSI_LOGI(0, "BT#_AppBtMsgHandler: id=0x%x, param1=0x%x, param2=0x%x", pEvent->id, pEvent->param1, pEvent->param2);

    int status = pEvent->param1;
    void *pData = (void *)(pEvent->param2);

    switch (pEvent->id){
        case APP_BT_INQ_DEV_NAME:
            {
                if (BT_SUCCESS == status){
                    showMessageBox1s(gPages[PAGE_CIT],"BT OK");
                }
                else
                    showMessageBox1s(gPages[PAGE_CIT],"BT ERR");
                MG_BT_Stop();
            }
            break;
    }
}


static void cit_click_handler(lv_obj_t * btn, lv_event_t event)
{

    OSI_LOGD(0, "cit_click_handler event:%x",event);
    if(event == LV_EVENT_PRESSED){
        char imei[16];
        int len ;
        int ind=btn->index;
        uint8_t status;
        char buf[64];
        uint8_t rat,siglevel,biterr;
        uint8_t nBcs = 0;
        uint8_t nBcl = 0;
        uint32_t nBv = 0;
        char snstr[46]={0};
        int re=0;
        switch(ind){
            case 0:
                showMessageBox(gPages[PAGE_CIT],GMR_ID);
                break;
            case 1:
                memset(imei,0,16);               
                len= nvmReadImei(0, (nvmImei_t *)imei);
                if(len < 0)
                    showMessageBox(gPages[PAGE_CIT],"no imei");
                else
                    showMessageBox(gPages[PAGE_CIT],imei);
                break;
            case 2:
				
                re =nvmReadSN(0,snstr,24);
                if(re==-1)
                showMessageBox(gPages[PAGE_CIT],"SN no");
                else
                showMessageBox(gPages[PAGE_CIT],snstr);
                break;
            case 3:
				//osiThreadSleep(3500);
				showMessageBox(gPages[PAGE_CIT],"Lcd Test");
                start_lcd_test();
                break;
            case 4:
                status = MG_cfwGetSimStatus(0);
                if(status == 1)
                    showMessageBox(gPages[PAGE_CIT],"sim ok");
                else
                    showMessageBox(gPages[PAGE_CIT],"NO SIM");
                break;
            case 5:
				#if PLAY_TEST_WITH_MIC
                showMessageBox1s(gPages[PAGE_CIT],"Start");
                prvRecordWriter();                
                showMessageBox1s(gPages[PAGE_CIT],"Stop");
                prvPlayFile();
				#else
				MG_auTonePlayCustom(9000, 9000, 10000);
				#endif
                break;

            case 6:
                if(MG_NwGetSignalQuality(&rat,&siglevel,&biterr)){
                    memset(buf,0,sizeof(buf));
                    sprintf(buf,"signal level:%d\nbit err:%d",(int)siglevel,(int)biterr);
                    showMessageBox(gPages[PAGE_CIT],buf);
                }else{
                    showMessageBox(gPages[PAGE_CIT],"got exception");
                }
                break;

            case 7:
                if (!MG_BT_GetState()){
                    if (!MG_AppRegisterBtMsgCB(APP_BT_MSG, btMsgHandler)){
                        showMessageBox1s(gPages[PAGE_CIT],"err");
                    }
                    if (BT_PENDING == MG_BT_Start()){
						showMessageBox1s(gPages[PAGE_CIT],"BT OK");
                       
                    }
                }else{
                    showMessageBox1s(gPages[PAGE_CIT],"already open");
                }


                break;

            case 8:
                drvChargerGetInfo(&nBcs, &nBcl,&nBv);
                memset(buf,0,sizeof(buf));
                if(nBcs == 2)
                    sprintf(buf,"Charging\nLevel:%d\nVoltage:%d",(int)nBcl,(int)nBv);
                else
                    sprintf(buf,"Level:%d\nVoltage:%d",(int)nBcl,(int)nBv);

                showMessageBox(gPages[PAGE_CIT],buf);
                break;

            case 9:
                if(!gKeyTest){
                    gKeyTest = true;
                    showMessageBox(gPages[PAGE_CIT],"Start key test");
                }else{
                    gKeyTest = false;
                    showMessageBox(gPages[PAGE_CIT],"Stop key test");
                }
                break;
        }
        


    }
}

static void slider_event_handler(lv_obj_t * slider, lv_event_t event) {      
    if(event == LV_EVENT_VALUE_CHANGED) {         
        int16_t v = lv_slider_get_value(slider);         
        OSI_LOGD(0, "slider_event_handler:%d",v);
    } 
}

static void aud_slider_event_handler(lv_obj_t * slider, lv_event_t event) {      
    if(event == LV_EVENT_VALUE_CHANGED) {         
        unsigned value = lv_slider_get_value(slider);         
        OSI_LOGD(0, "aud_slider_event_handler:%d",value);
        if(slider->index == 3){
            MG_audevSetVoiceVolume(value);
        }
        else if(slider->index == 1){
            MG_audevSetPlayVolume(value);
        }

    } 
}


static void mg_show_audslider(unsigned max,unsigned value){   
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);
    lv_obj_t * slider = lv_slider_create(scr, NULL);
    lv_obj_set_size(slider, lv_obj_get_width(scr)-20, LV_DPI / 5);
    lv_obj_align(slider, scr, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider, 0, max);
    lv_slider_set_value(slider, value, false);

    lv_anim_t a;
    a.var            = slider;
    a.start          = 0;
    a.end            = 1;
    a.exec_cb        = (lv_anim_exec_xcb_t)NULL;
    a.path_cb        = lv_anim_path_linear;
    a.ready_cb       = clearanim;
    a.act_time       = -1000;
    a.time           = 0;
    a.playback       = 0;
    a.playback_pause = 0;
    a.repeat         = 0;
    a.repeat_pause   = 0;
    lv_anim_create(&a);
}
 
static void create_common(){
    lv_theme_t * th = lv_theme_material_init(210, NULL);
    /*Set the surent system theme*/
    lv_theme_set_current(th);
//
//    lv_obj_t * scr = lv_page_create(NULL, NULL);
//    lv_disp_load_scr(scr);
    lv_indev_t *kp_indev = lvGuiGetKeyPad();
    gGrp = lv_group_create();

    /*Assig the input device(s) to the created group*/
    lv_indev_set_group(kp_indev, gGrp);

	lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    sig=lv_img_create(scr,NULL);
    lv_img_set_src(sig,&png_signal_none);
    lv_obj_set_pos(sig, 2,0);

    f4g=lv_label_create(scr,NULL);
    //lv_label_set_style(f4g, LV_LABEL_STYLE_MAIN, gCstyle);
    lv_label_set_text(f4g,"4G");
    lv_obj_align(f4g, sig, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

    ltime=lv_label_create(scr,NULL);
    lv_label_set_text(ltime,"16:04:00");

    bat=lv_img_create(scr,NULL);
    lv_img_set_src(bat,&png_battery_100);
    lv_obj_set_opa_scale_enable(bat,true);
    lv_obj_set_opa_scale(bat,122);

    lv_obj_align(bat, scr, LV_ALIGN_IN_TOP_RIGHT, -2, 0);
    //lv_obj_align(ltime, bat, LV_ALIGN_OUT_LEFT_MID, -2, 0); 
    lv_obj_align(ltime, bat, LV_ALIGN_OUT_LEFT_MID, -6, 0); 

    lv_coord_t h = lv_obj_get_height(sig);
//    lv_coord_t h2 = lv_obj_get_height(bck);

    for(int i=0;i<PAGE_MAX;i++){
        gPages[i]= lv_page_create(scr,NULL);
        lv_obj_set_size(gPages[i],hres,vres-h);
        lv_obj_align(gPages[i],scr,LV_ALIGN_IN_BOTTOM_MID,0,0);
        OSI_LOGD(0, "create :%d,%p",i,gPages[i]);
        lv_obj_set_hidden(gPages[i],true);
    }

    ok = lv_btn_create(scr,NULL);
    lv_obj_set_size(ok,34,18);
    img_ok = lv_img_create(ok, NULL);
    lv_img_set_src(img_ok, &png_ok);
    lv_obj_set_opa_scale_enable(img_ok,true);
    lv_obj_set_opa_scale(img_ok,200);
//    lv_obj_set_pos(img_ok, 8, 113);
    lv_obj_align(ok,scr,LV_ALIGN_IN_BOTTOM_LEFT,0,0);

    bck = lv_btn_create(scr,NULL);
    lv_obj_set_size(bck,34,18);
    lv_obj_t * img_bck = lv_img_create(bck, NULL);
    lv_img_set_src(img_bck, &png_back);
    lv_obj_set_opa_scale_enable(img_bck,true);
    lv_obj_set_opa_scale(img_bck,200);
//    lv_obj_set_pos(img_back, 137, 113);
    lv_obj_align(bck,scr,LV_ALIGN_IN_BOTTOM_RIGHT,0,0);

    lv_obj_set_event_cb(ok, btn_click_handler);
    lv_obj_set_event_cb(bck, btn_click_handler);
}
static void createHomePage(lv_style_t *style1){
    lv_obj_t *dial = lv_btn_create(gPages[PAGE_HOME],NULL);
    int y = 100;
    lv_obj_set_size(dial,48,40);
//    lv_obj_align(dial,page,LV_ALIGN_CENTER,0,0);
    lv_obj_set_pos(dial, 3, y);

    lv_obj_t * img_dial = lv_img_create(dial, NULL);
    lv_img_set_src(img_dial, &png_member);
    lv_obj_set_opa_scale_enable(img_dial,true);
    lv_obj_set_opa_scale(img_dial,200);

    lv_obj_t *dial_label=lv_label_create(gPages[PAGE_HOME],NULL);
    lv_label_set_style(dial_label, LV_LABEL_STYLE_MAIN, style1);
    lv_obj_set_pos(dial_label, 14, y+43);
    lv_label_set_text(dial_label,"支付宝");


    btn_group = lv_btn_create(gPages[PAGE_HOME],NULL);
    lv_obj_set_size(btn_group ,48,40);
    lv_obj_set_pos(btn_group, 66, y);
    lv_group_add_obj(gGrp, btn_group); /*Add the object to the group*/
    lv_obj_set_event_cb(btn_group, show_cgroup);     

    lv_obj_t * img_grp = lv_img_create(btn_group, NULL);
    lv_img_set_src(img_grp, &png_group);
    lv_obj_set_opa_scale_enable(img_grp,true);
    lv_obj_set_opa_scale(img_grp,200);

    lv_obj_t *group_label=lv_label_create(gPages[PAGE_HOME],NULL);
    lv_label_set_style(group_label, LV_LABEL_STYLE_MAIN, style1);
    lv_label_set_text(group_label,"群组");
    lv_obj_set_pos(group_label, 76, y+43);

    btn_setting = lv_btn_create(gPages[PAGE_HOME],NULL);
    lv_obj_set_size(btn_setting,48,40);
    lv_obj_set_pos(btn_setting, 128, y);
    lv_group_add_obj(gGrp, btn_setting); /*Add the object to the group*/

    lv_obj_set_event_cb(btn_setting, show_settings);     

    lv_obj_t * img_setting = lv_img_create(btn_setting, NULL);
    lv_img_set_src(img_setting, &png_setting);
    lv_obj_set_opa_scale_enable(img_setting,true);
    lv_obj_set_opa_scale(img_setting,200);

    lv_obj_t *setting_label=lv_label_create(gPages[PAGE_HOME],NULL);
    lv_label_set_style(setting_label, LV_LABEL_STYLE_MAIN, style1);
    lv_label_set_text(setting_label,"设置");
    lv_obj_set_pos(setting_label, 138, y+43);

    lv_obj_set_hidden(gPages[PAGE_HOME],true);
    //lv_obj_set_hidden(ok,true);
    //lv_obj_set_hidden(bck,true);
    curPage = PAGE_HOME;
}

static char list_index=0;
bool mg_proc_key(int key,int state){
    bool proc = false;
    unsigned vol = MG_audevGetVoiceVolume();
    if(state == 1){
        if(gKeyTest){
            showKeyMessage(gPages[PAGE_CIT],key);
            return;
        }
        switch(key){
            case LV_KEY_ENTER:
                handleEnterKey();
                //proc = true;
                break;
            case LV_KEY_ESC:
                handleBckKey();
                proc = true;
                break;
            case LV_KEY_ANSWER:
                proc = handleAnswerKey();
                break;
            case LV_KEY_POWER:
                proc = handlePowerKey();
                gPwrTick = 1;
                break;
            case LV_KEY_VOLUME_UP:
                MG_audevSetVoiceVolume((vol+1)%100);
                mg_show_audslider(100,vol);
                proc = true;
                break;
            case LV_KEY_VOLUME_DOWN:
                MG_audevSetVoiceVolume((vol-1)%100);
                mg_show_audslider(100,vol);
                proc = true;
                break;
            case LV_KEY_RIGHT:
                if(curPage == PAGE_HOME){
                    lv_group_focus_next(gGrp);
                    proc = true;
                }
                break;
            case LV_KEY_LEFT:
                if(curPage == PAGE_HOME){
                    lv_group_focus_prev(gGrp);
                    proc = true;
                }
                break;
            case LV_KEY_NEXT:
                if(curPage == PAGE_CIT){
					if(list_index<=9)
					list_index++;
                    osiTracePrintf(OSI_LOG_TAG, "scroll list NEXT %d",list_index);
					if(list_index>9)
					{
						lv_list_up(list2);
						lv_group_focus_prev(gGrp);
						break;
					}
                    lv_list_up(list2);
                    //lv_page_scroll_ver(gPages[PAGE_CIT],-20);
                }
                break;
            case LV_KEY_PREV:
                if(curPage == PAGE_CIT){

					if(list_index==10)
					{
						list_index=9;
					}
					
					if(list_index<=0)
					{
					    lv_group_focus_next(gGrp);
						break;
					}
					if(list_index>=1)
					{
					    list_index--;
					}
                    osiTracePrintf(OSI_LOG_TAG, "scroll list PREV %d",list_index);
					
					
                    lv_list_down(list2);
                    //lv_page_scroll_ver(gPages[PAGE_CIT],-20);
                }
                break;
        }
    }
    else{
        switch(key){
            case LV_KEY_POWER:
                gPwrTick = 0;
                break;
        }
    }
    return proc;

}
void mg_update_ui()
{

    lv_img_dsc_t *bat_pngs[10]={
        &png_battery_10,
        &png_battery_20,
        &png_battery_30,
        &png_battery_40,
        &png_battery_50,
        &png_battery_60,
        &png_battery_70,
        &png_battery_80,
        &png_battery_90,
        &png_battery_100
    };
    osiTracePrintf(OSI_LOG_TAG, "power tick %x",gPwrTick);
    if(gPwrTick >0){
        gPwrTick++;
        if(gPwrTick > 10){
            MG_osiTracePrintf(OSI_LOG_TAG, "power off");
            if (0 == CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF)){
                MG_osiThreadSleep(100);
                MG_osiShutdown(OSI_SHUTDOWN_POWER_OFF);
            }
        }
    }

	int timeoffset = MG_SysGetTimeZone();	
	time_t lt = osiEpochSecond()+ timeoffset;
    //time_t lt = MG_osiLocalSecond() ;
    struct tm ltm;
    gmtime_r(&lt, &ltm);
    char buf[16]={0};
    sprintf(buf,"%02d:%02d:%02d",ltm.tm_hour,ltm.tm_min,ltm.tm_sec);
    lv_label_set_text(ltime,buf);

    uint8_t nBcs = 0;
    uint8_t nBcl = 0;
	uint32_t nBv = 0;
    static uint8_t sChgScrollDisplay = 0xff;
    
    drvChargerGetInfo(&nBcs, &nBcl,&nBv);
    uint8_t t = nBcl/10;
    if(t >= 10)
        t=9;
    else if(t<0)
        t = 0;
    if(gStatus[0] != t)
        gStatus[0] = t;
    
    if(nBcs == 2)
    {
        if((sChgScrollDisplay == 0xff) || (sChgScrollDisplay == 9) )
        {
            sChgScrollDisplay = t;
        }
        else 
        {            
             t =++sChgScrollDisplay;
        }        
    }
    else
    {
           sChgScrollDisplay = 0xff;
    }
    lv_img_set_src(bat,bat_pngs[t]);

    uint8_t siglevl = 0;
    uint8_t nBitError = 0;
    MG_cfwNwGetSignalQuality(&siglevl, &nBitError, 0);
    if (siglevl > 113)
    {
        siglevl = 0;
    }
    else if ((siglevl <= 113) && (siglevl >= 51))
    {
        siglevl = (uint8_t)(31 - (siglevl - 51) / 2);
    }
    else
    {
        siglevl = 31;
    }
    if(gStatus[1] != siglevl){
        gStatus[1] = siglevl;

        if(siglevl<=10)
            lv_img_set_src(sig,&png_signal_1);
        else if(siglevl<=20)
            lv_img_set_src(sig,&png_signal_2);
        else
            lv_img_set_src(sig,&png_signal_3);
    }

    int status = MG_cfwGetSimStatus(CFW_SIM_0);
    /*
    CFW_NW_STATUS_INFO nw_status;

    CFW_GprsGetstatus(&nw_status,CFW_SIM_0);
    uint8_t status = nw_status.ConnMode;
    */

    //OSI_LOGD(0, "nw status:%d,battery level:%x",status,t);
    if(status == 0){
        if(gStatus[2] != status){
            gStatus[2] = (uint8_t)status;
            lv_img_set_src(sig,&png_signal_none);
            lv_label_set_text(f4g,"X");
        }
    }
    else{
        if(gStatus[2] != status){
            gStatus[2] = (uint8_t)status;
            lv_label_set_text(f4g,"4G");
        }
    }
}

/**
 * Create a demo application
 */
void mg_demo_create(void)
{
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.font = &cjk;
    gCstyle = &style1;

	create_common();

    createHomePage(&style1);
    mg_switch_cit();

    OSI_LOGD(0, "visible page:%d",mg_get_state());
}

void mg_switch(uint8_t page){
    for(uint8_t i=0;i<PAGE_MAX;i++){
        if(i != page){
            //OSI_LOGD(0, "mg_switch hide page:%x,%p",i,gPages[i]);
            lv_obj_set_hidden(gPages[i],true);
        }else{
            OSI_LOGD(0, "mg_switch show page:%x,%p",i,gPages[i]);
            lv_obj_set_hidden(gPages[i],false);
        }
    }
}

void mg_switch_main(){
    prePage = PAGE_INVALID;
    curPage = PAGE_HOME;
    mg_switch(PAGE_HOME);

    lv_group_remove_all_objs(gGrp);
    lv_group_add_obj(gGrp, btn_group); /*Add the object to the group*/
    lv_group_add_obj(gGrp, btn_setting); /*Add the object to the group*/
}

void mg_switch_back(){
    if(prePage == PAGE_INVALID)
        return;
    if(prePage == PAGE_HOME)
        mg_switch_main();
    else if(prePage == PAGE_GROUP)
        mg_switch_group(0);
    else if(prePage == PAGE_SETTING)
    {
        mg_switch_setting();
        for(uint8_t i=0;i<gSettingGroupIndex;i++)
            lv_group_focus_next(gGrp);
    }
    else if(prePage == PAGE_CGROUP)
        mg_switch_cgroup();

}


void mg_switch_cgroup(){
    prePage = mg_get_state();
    mg_switch(PAGE_CGROUP);
    curPage = PAGE_CGROUP;

    lv_group_remove_all_objs(gGrp);
    //lv_group_add_obj(gGrp, bck);
    //lv_group_add_obj(gGrp, ok);

    if(list != NULL){
        lv_obj_del(list);
        list = NULL;
    }
    list = lv_list_create(gPages[PAGE_CGROUP], NULL);
    lv_obj_set_height(list, lv_obj_get_height(gPages[PAGE_CGROUP]));
    lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, gCstyle);

    mg_cgmember_add("未加入任何组",false);
}

void mg_switch_group(int index){
    prePage = mg_get_state();
    mg_switch(PAGE_GROUP);
    curPage = PAGE_GROUP;

    OSI_LOGD(0, "mg_switch_group");
    lv_group_remove_all_objs(gGrp);
    //lv_group_add_obj(gGrp, bck);

    if(list != NULL){
        lv_obj_del(list);
        list = NULL;
    }
    list = lv_list_create(gPages[PAGE_GROUP], NULL);
    lv_obj_set_height(list, lv_obj_get_height(gPages[PAGE_GROUP]));
    lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, gCstyle);
    int count = 0;//;
    if(count>0){

    }else{
        mg_gmember_add("离线",false,0);
    }
}
void mg_switch_setting(){
    prePage = mg_get_state();
    mg_switch(PAGE_SETTING);
    curPage = PAGE_SETTING;

    lv_group_remove_all_objs(gGrp);
    //lv_group_add_obj(gGrp,bck);
    //lv_group_add_obj(gGrp,ok);

    if(list2 != NULL){
        lv_obj_del(list2);
        list2 = NULL;
    }

    list2 = lv_list_create(gPages[PAGE_SETTING], NULL);
    lv_obj_set_height(list2, lv_obj_get_height(gPages[PAGE_SETTING]));
    lv_list_set_style(list2, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list2, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_obj_align(list2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_list_set_style(list2, LV_LIST_STYLE_BTN_REL, gCstyle);

    lv_obj_t * list_btn;    
    list_btn = lv_list_add_btn(list2, &png_group_icon, "显示设置");     
    list_btn->index = 0;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, setting_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "声音设置");     
    list_btn->index = 1;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, setting_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "GPS设置");     
    list_btn->index = 2;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, setting_click_handler);
}

void mg_switch_cit(){
    prePage = mg_get_state();
    mg_switch(PAGE_CIT);
    curPage = PAGE_CIT;

    lv_group_remove_all_objs(gGrp);
    //lv_group_add_obj(gGrp,bck);
    //lv_group_add_obj(gGrp,ok);

    if(list2 != NULL){
        lv_obj_del(list2);
        list2 = NULL;
    }

    list2 = lv_list_create(gPages[PAGE_CIT], NULL);
    lv_obj_set_height(list2, lv_obj_get_height(gPages[PAGE_CIT])-20);
    lv_list_set_style(list2, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list2, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_obj_align(list2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_list_set_style(list2, LV_LIST_STYLE_BTN_REL, gCstyle);

    lv_obj_t * list_btn;    
    list_btn = lv_list_add_btn(list2, &png_group_icon, "版本号");     
    list_btn->index = 0;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "IMEI");     
    list_btn->index = 1;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "SN");     
    list_btn->index = 2;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "SIM");     
    list_btn->index = 4;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "AUDIO");     
    list_btn->index = 5;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "网络信号");     
    list_btn->index = 6;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "蓝牙");     
    list_btn->index = 7;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "电池");     
    list_btn->index = 8;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "按键");     
    list_btn->index = 9;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);

    list_btn = lv_list_add_btn(list2, &png_group_icon, "LCD");     
    list_btn->index = 3;
    lv_group_add_obj(gGrp,list_btn);
    lv_obj_set_event_cb(list_btn, cit_click_handler);
}


void mg_cgmember_add(char*name,bool click,int index){
    lv_obj_t * list_btn;
    list_btn = lv_list_add_btn(list, &png_group_icon,name); 
    list_btn->index = index;
    lv_group_add_obj(gGrp, list_btn); 
    
    if(click)
        lv_obj_set_event_cb(list_btn, gitem_click_handler);
    else
        lv_obj_set_click(list_btn,false);
}

void mg_gmember_add(char*name,bool online,uint8_t index){
    lv_obj_t * list_btn;
    list_btn = lv_list_add_btn(list, &png_group_icon,name); 
    list_btn->index = index;
    osiTracePrintf(OSI_LOG_TAG, "%s:%s",__func__,name);
    if(online){
        lv_group_add_obj(gGrp, list_btn); 
        lv_obj_set_event_cb(list_btn, item_click_handler);
    }
    else
        lv_obj_set_click(list_btn,false);
}
int mg_get_state(){

    for(uint8_t i = 0;i<PAGE_MAX;i++){
        if(!lv_obj_get_hidden(gPages[i]))
            return i;
    }
    return -1;
}
osiThread_t *mg_net_thread(){
    return netGetTaskID();
}
