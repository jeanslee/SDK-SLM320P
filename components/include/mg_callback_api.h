#ifndef _MG_CALLBACK_API_H_
#define _MG_CALLBACK_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MG_MAX_REPORT_EVENT 20

enum response_event
{
    EV_CS_DOMAIN_REGISTER = 6001,       //CS域(通话)注册
    EV_CS_DOMAIN_NO_REGISTER,           //CS域去注册
    EV_PS_DOMAIN_REGISTER,              //PS域(数据)注册
    EV_PS_DOMAIN_NO_REGISTER,           //PS域去注册
    EV_PDP_AUTO_ACTIVATION,             //PDP开机自动激活成功
    EV_PDP_AUTO_DEACTIVATION,           //PDP被动去激活
    EV_PDP_ACTIVE_ACTIVATION,           //PDP主动激活成功
    EV_PDP_ACTIVE_DEACTIVATION,         //PDP主动去激活
    EV_NEW_CALL,                        //新通话上报
    EV_NEW_SMS,                         //新短信上报
    EV_CALL_ON_CALLING,                 //对方通话接通
    EV_CALL_HUNG_UP                     //对方通话挂断
};

typedef struct
{
    uint32_t event;
    void(*cb)(void);
}mgCallback;

void MG_sendStatuEvent(uint32_t event_id);

void MG_waitStatusReportInit();

void MG_waitStatusReportThread();

/* 
 *\brief binding custom callback func in corresponding response event   
 *\param [in] event         event id like "EV_NEW_SMS"
 *\param [in] callback_cb   custom callback func when event response it will be call
 *\return true on success, or flase in event_id
*/
bool MG_registCallback(uint32_t event, void(*callback_cb)(void));


/*
example

void MG_test()
{
    MG_osiTracePrintf(0, "This is the callback after event");
}

MG_registCallback(EV_NEW_CALL, MG_test);

*/

#endif