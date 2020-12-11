#ifndef _MG_CMD_API_H_
#define _MG_CMD_API_H_

#include "at_response.h"
#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#define CMD_STRING_MAX 256
#define CMD_HANDLE_NUM 20

typedef enum 
{
    Cus_AT_POC = 0,                     //AT+POC=
    Cus_AT_eSIM,                        //AT+eSIM=
    Cus_AT_SetJulinkConfig,             //AT+SetJulinkConfig=
    Cus_AT_GetJulinkConfig              //AT+GetJulinkConfig=
}CusATCommand;

typedef struct
{
    uint8_t cmd_id;                     //use from CusATComand
    void(*cb)(atCmdEngine_t*, char*);   //customer AT command handle function
}mgCmdHandle;

/*
 * /brief send text to UART
 *
 * /param [in] eng          must correspond with AT handle callback param, can't be NULL
 * /param [in] text         the text will be send to UART, can't be NULL
 */
void MG_sendUartText(atCmdEngine_t *eng, char* text);

/*
 * /brief Send OK to UART, and terminate AT command handle, send OK or ERROR only once
 *  normal terminate AT engine
 *
 * /param [in] eng          must correspond with AT handle callback param, can't be NULL
 */
void MG_sendUartOK(atCmdEngine_t *eng);

/*
 * /brief Send ERROR and ERROR CODE to UART, and terminate AT command handle, send OK or ERROR only once
 *  to normal terminate AT engine
 *
 * /param [in] eng          must correspond with AT handle callback param, can't be NULL
 * /param [in] errcode      error code will be send to UART, show by AT+CMEE
 */
void MG_sendUartError(atCmdEngine_t *eng, int errcode);

/*
 * /brief registe customer AT command handle callback
 *
 * /param [in] cmd_id       use from CusATComand
 * /param [in] cb           AT handle function, it must have two param, 
                                atCmdEngine_t* [in]     AT command handle
                                char* [in]              text in "AT+***=text"
 * /return true on success, or flase in failed
 */
bool MG_registCusATHandle(uint8_t cmd_id, void(*cb)(atCmdEngine_t*, char*));


/***Sample Example**

void test_AT_HANDLE(atCmdEngine_t* eng, char* text)
{
    //deal with text
    
    if (err)
    {
        MG_sendUartText(eng, err);
        MG_sendUartError(eng, errcode);     //send error and terminate AT handle
        return ;
    }
    
    //deal end
    
    MG_sendUartText(eng, text_end);
    MG_sendUartOK(eng);                     //send OK and terminate AT handle
    return ;
}

void main()
{
    MG_registCusATHandle(Cus_AT_POC, test_AT_HANDLE);   //regist handle in AT+POC=
}

****end*****/

#endif