/**
  ******************************************************************************
  * @file    simidplatform.c 
  * @author  
  * @version V1.0
  * @date    2019-8-15 
  * @brief   Security SIMid platform.
  ******************************************************************************
  */

#include <string.h>
#include "simid.h"
#include "stdio.h"
//thirdparty modem headfiles



/**************************** memory heap api ****************************/
/*****************************************************************
*Function: simid_malloc
*Description: thirdparty modem achieve the function
*Input:  size
*Return: the valid heap pointer
*****************************************************************/

/* void *simid_malloc(size_t size)
{
	void *p;

	if( size <= 0 )
		return NULL;
	
	//use define 

	
	return p;
} */
/*****************************************************************
*Function: simid_free
*Description: thirdparty modem achieve the function
*Input:  < p > the valid heap pointer
*Return: void
*****************************************************************/

/* void simid_free(void *p)
{
	if( p == NULL )
		return;
	
	//use define 
	
} */



 /*****************************************************************
 *Function: ctiot_open_simid_channel
 *Description: thirdparty modem achieve the function
 *Input:  <sessionID> output channel session id
          <AID>       input application id
 *Return: result code
 
 *****************************************************************/
int ctiot_open_simid_channel(unsigned char *sessionID, const char *AID)
{
/* 	NBStatus_t ret = CTIOT_SIMID_ERROR_PARAMS;
		
 	if( sessionID == NULL || AID == NULL )
		return CTIOT_SIMID_ERROR_PARAMS;

	//use define 

	return ret;	 */
}

/*****************************************************************
*Function: ctiot_close_simid_channel
*Description: thirdparty modem achieve the function
*Input:  <sessionID> input channel session id
*Return: result code

*****************************************************************/
int ctiot_close_simid_channel(unsigned char sessionID)
{
/* 	NBStatus_t ret = CTIOT_SIMID_ERROR_PARAMS;
	
	//use define 

	return ret; */
}


/*****************************************************************
*Function: ctiot_access_simid_channel
*Description: thirdparty modem achieve the function
*Input:   <sessionID> input channel session id
		  <cleartex>  input cleartex
		  <ciphertext> output ciphertext
		  <ciphertext> output ciphertext length
*Return: result code
         
*****************************************************************/
int ctiot_access_simid_channel(unsigned char sessionID, char *cleartext, char *ciphertext, unsigned int *ciphertextLen)
{
/* 	NBStatus_t ret = CTIOT_SIMID_ERROR_PARAMS;
	
	if( cleartext == NULL || ciphertext == NULL || ciphertextLen == NULL )
		return CTIOT_SIMID_ERROR_PARAMS;
	
	//use define 

	return ret; */
}


