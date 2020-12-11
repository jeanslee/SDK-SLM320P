/**
  ******************************************************************************
  * @file    simid.h 
  * @author  ztj
  * @version V1.0
  * @date    2019-5-10 17:34:28
  * @brief   Head file of 
	Security SIMid I/O module.
  ******************************************************************************
  */
	
#ifndef __SIMID__H__
#define __SIMID__H__

#ifdef __cplusplus   
extern "C" {
#endif


/****************************** ctiot simid error code *****************************/
#define	CTIOT_SIMID_SUCCESS                0
#define CTIOT_SIMID_ERROR_PARAMS          0x01
#define CTIOT_SIMID_ERROR_MALLOC          0x02
#define CTIOT_SIMID_ERROR_ENCRYPT         0x04
#define CTIOT_SIMID_ERROR_OPEN_CHANNEL    0x08
#define CTIOT_SIMID_ERROR_CLOSE_CHANNEL   0x10


/****************************** ctiot simid api ************************************/
/***********************************************
*Function: ctiot_get_simid_cleartext
*Description:
*Input:  <buff> ouput cleartext
		 <len>	the buff length, len >= 256
*Return: result code
************************************************/

void ctiot_print_result_code(unsigned int code);


/***********************************************
*Function: ctiot_get_simid_cleartext
*Description:
*Input:  <buff> ouput cleartext
		 <len>	the buff length, len >= 256
*Return: result code
***********************************************/

int ctiot_get_simid_cleartext(char *buff, unsigned int len);

/**********************************************
*Function:	ctiot_process_ciphertext
*Description: 
*Input:  <buff> output processed ciphertext string
		 <len>	the buff length
*Return: result code
**********************************************/

int ctiot_process_simid_ciphertext(char *buff, unsigned int len);


/********************************************
*Function: ctiot_get_encrypt_string
*Description:
*Input:  <buff> output processed ciphertext string
		 <len>	the buff length, len >= 256

*Return:
*********************************************/

int ctiot_get_simid_ciphertext(char *buff, unsigned int len);


/*********************************************
*Function: ctiot_open_simid_channel
*Description: thirdparty modem achieve the function
*Input:  <sessionID> output channel session id
		 <AID>		 input application id
*Return: result code

**********************************************/

int ctiot_open_simid_channel(unsigned char *sessionID, const char *AID);


/*********************************************
*Function: ctiot_close_simid_channel
*Description: thirdparty modem achieve the function
*Input:  <sessionID> input channel session id
*Return: result code

**********************************************/

int ctiot_close_simid_channel(unsigned char sessionID);


/*********************************************
*Function: ctiot_access_simid_channel
*Description: thirdparty modem achieve the function
*Input:   <sessionID> input channel session id
		  <cleartex>  input cleartex
		  <ciphertext> output ciphertext
		  <ciphertext> output ciphertext length
*Return: result code
         
**********************************************/

int ctiot_access_simid_channel(unsigned char sessionID, char *cleartext, char *ciphertext, unsigned int *ciphertextLen);


char* libapp_test(char* dest, const char* source);


#ifdef __cplusplus
}
#endif


#endif

