/**
  ******************************************************************************
  * @file    simid.c 
  * @author  
  * @version V1.0
  * @date    2019-8-15
  * @brief   Security SIMid I/O module.
  ******************************************************************************
  */



#include "simid.h"
#include "simidplatform.h"


#define simid_log    opencpu_printf
#define simid_malloc malloc
#define simid_free   free

/* Private variables ----------------------------------------------------------*/
/* Public variables -----------------------------------------------------------*/

/* Private function decleration -----------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/




unsigned char CLA = 0x81;
unsigned char INS = 0xF1;
unsigned char P1 = 0x00; 
unsigned char P2 = 0x00; 
unsigned char encrypt_type = 0x51;
unsigned char decrypt_type = 0x52;

char* cleartext = "NBYFN21X10XXXX";
unsigned char cleartext_len = 28;


void ctiot_print_result_code(unsigned int code)
{
	simid_log("\r\nresult code:");

	if( code == CTIOT_SIMID_SUCCESS )
	{
		simid_log("CTIOT_SIMID_SUCCESS\r\n");
		return;
	}
	
	if( code & CTIOT_SIMID_ERROR_PARAMS )
	{
		simid_log("CTIOT_SIMID_ERROR_PARAMS\r\n");
	}
    if( code & CTIOT_SIMID_ERROR_MALLOC )
	{
		simid_log("CTIOT_SIMID_ERROR_MALLOC\r\n");
	}
	if( code & CTIOT_SIMID_ERROR_ENCRYPT )  
	{
		simid_log("CTIOT_SIMID_ERROR_ENCRYPT\r\n");
	}
	if( code & CTIOT_SIMID_ERROR_OPEN_CHANNEL )
	{
		simid_log("CTIOT_SIMID_ERROR_OPEN_CHANNEL\r\n");
	}
	if( code & CTIOT_SIMID_ERROR_CLOSE_CHANNEL )
	{
		simid_log("CTIOT_SIMID_ERROR_CLOSE_CHANNEL\r\n");
	}
}

/*****************************************************************
 *Function: HexToStr
 *Description:
 *Input:  <pbDest> output
 		  <pbSrc> input
 		  <inLen> input
 *Return: length
*****************************************************************/

static int HexToStr(char *pbDest, unsigned char *pbSrc, int inLen)
{
	int i, dHex;

	for (i=0; i<inLen; i++)
	{	
		dHex=pbSrc[i];
		sprintf(&pbDest[2*i], "%02X", dHex);
	}

  return 2*i;
}



/*****************************************************************
 *Function: ctiot_generate_package
 *Description:
 *Input:  <version> input
 		  <algorithm_type> input
 		  <package> input
 *Return: length
*****************************************************************/
static int ctiot_generate_package(unsigned char version, unsigned char algorithm_type, char *package)
{
	char primevalPackage[256];
	unsigned char Lc = 0;
	unsigned char Le = 0x00;

	cleartext_len = strlen(cleartext);
	
	memset(primevalPackage, 0 ,sizeof(primevalPackage));
	sprintf(primevalPackage, "%s%02X", primevalPackage, version);
	sprintf(primevalPackage, "%s%02X", primevalPackage, encrypt_type);
	sprintf(primevalPackage, "%s%02X", primevalPackage, algorithm_type);
	sprintf(primevalPackage, "%s%02X", primevalPackage, cleartext_len);
	
	HexToStr(primevalPackage + 8, (unsigned char*)cleartext, cleartext_len);
	Lc = 4 + cleartext_len;
	//memset(package, 0 ,sizeof(package));
	sprintf(package, "%s%02X", package, CLA);
	sprintf(package, "%s%02X", package, INS);
	sprintf(package, "%s%02X", package, P1);
	sprintf(package, "%s%02X", package, P2);
	sprintf(package, "%s%02X", package, Lc);
	sprintf(package, "%s%s", package, primevalPackage);
	sprintf(package, "%s%02X", package, Le);
	
	return strlen(package);
}


 /*****************************************************************
 *Function: ctiot_get_simid_cleartext
 *Description:
 *Input:  <buff> ouput cleartext
 		  <len>  the buff length, len >= 256
 *Return: result code
 *****************************************************************/
 int ctiot_get_simid_cleartext(char *buff, unsigned int len)
{
	unsigned char version       = 0x00;
	unsigned char algorithm_type = 0x01;
	
	if( buff == NULL )
		return CTIOT_SIMID_ERROR_PARAMS;

	if( len < 256 )
		return CTIOT_SIMID_ERROR_PARAMS;
	
	ctiot_generate_package(version, algorithm_type, buff);

	return CTIOT_SIMID_SUCCESS;
}

 /*****************************************************************
 *Function:  ctiot_process_ciphertext
 *Description: 
 *Input:  <buff> output processed ciphertext string
          <len>  the buff length
 *Return: result code
 *****************************************************************/

 int ctiot_process_simid_ciphertext(char *buff, unsigned int len)
 {
	 int errcode = CTIOT_SIMID_SUCCESS;
	 int length;
 
	 if( buff == NULL )
	 {
		 return CTIOT_SIMID_ERROR_PARAMS;
	 }
 
	 length = strlen(buff)-4;
 
	 if( length <= 0 )
	 {
		 return CTIOT_SIMID_ERROR_PARAMS;
	 }
 
	 if( buff[length] == '9' && buff[length+1] == '0' && buff[length+2] == '0' && buff[length+3] == '0' )
	 {
		 length = strlen(buff)+1+strlen(cleartext)*2+2;
		 if( length > len )
		 {
		 	return CTIOT_SIMID_ERROR_PARAMS;
		 }
		 
		 char *temp = simid_malloc(length);
		 if( temp == NULL )
		 {
			 return CTIOT_SIMID_ERROR_MALLOC;
		 }
		 
		 memset(temp, 0, length);
		 temp[0] = (cleartext_len*2)/10+'0';
		 temp[1] = (cleartext_len*2)%10+'0';
		 HexToStr(temp+2, (unsigned char*)cleartext, cleartext_len);
		 memcpy(temp+2+cleartext_len*2, buff, strlen(buff)-4);
		 strcpy(buff,temp);
		 simid_free(temp);
	 }
	 else
	 {
	 	 simid_log("fail err code:%s\r\n", &buff[length]);
		 return CTIOT_SIMID_ERROR_ENCRYPT;
	 }
 
	 return errcode;
 }
 
 /*****************************************************************
 *Function: ctiot_get_encrypt_string
 *Description:
 *Input:  <buff> output processed ciphertext string
          <len>  the buff length, len >= 256

 *Return:
 *****************************************************************/

int ctiot_get_simid_ciphertext(char *buff, unsigned int len)
{
	unsigned char sessionID=0;
	char *applicationID ="A00000004374506173732E496F54";
	unsigned char ret;
	int errcode = CTIOT_SIMID_SUCCESS;

	if( buff == NULL )
	{
		return CTIOT_SIMID_ERROR_PARAMS;
	}

	//1. open channel
	ret = ctiot_open_simid_channel(&sessionID, applicationID);
	if( ret == CTIOT_SIMID_SUCCESS )
	{
		simid_log("simid open channel OK\r\n");
	
		char *mcleartext = simid_malloc(256);
		if( mcleartext == NULL )
		{
			errcode = CTIOT_SIMID_ERROR_MALLOC;
			goto exit;
		}
		memset(mcleartext, 0, 256);
		
		//2. get cleartext
		if( ctiot_get_simid_cleartext(mcleartext, 256) != CTIOT_SIMID_SUCCESS)
		{
			simid_free(mcleartext);
			errcode = CTIOT_SIMID_ERROR_PARAMS;
			goto exit;
		}
	
		char *ciphertextStr = simid_malloc(256);
		if( ciphertextStr == NULL )
		{
			simid_free(mcleartext);
			errcode = CTIOT_SIMID_ERROR_MALLOC;
			goto exit;
		}
		memset(ciphertextStr, 0, 256);
	
		//3. access channel
		unsigned int encryptLen=0;
		ret = ctiot_access_simid_channel(sessionID, mcleartext, ciphertextStr, &encryptLen);
		simid_free(mcleartext);
		if( ret == CTIOT_SIMID_SUCCESS )	
		{
			simid_log("encryptStr:%s\r\n",ciphertextStr);
			if( len < encryptLen+1 )
			{
				simid_free(ciphertextStr);
				errcode = CTIOT_SIMID_ERROR_PARAMS;
				goto exit;
			}

			//4. process enctrypt string
			ctiot_process_simid_ciphertext(ciphertextStr, 256);
			strcpy(buff, ciphertextStr);
			simid_log("simid ciphertextStr:%s\r\n",ciphertextStr);
			simid_free(ciphertextStr);
		}
		else
		{
			simid_free(ciphertextStr);
			errcode = CTIOT_SIMID_ERROR_ENCRYPT;
			goto exit;
		}
	}
	else
	{
		return CTIOT_SIMID_ERROR_OPEN_CHANNEL;
	}

exit:

	//5. close channel
	ret = ctiot_close_simid_channel(sessionID);
	if( ret == CTIOT_SIMID_SUCCESS )
	{
		simid_log("simid close channel OK\r\n");
	}
	else
	{
		errcode |= CTIOT_SIMID_ERROR_CLOSE_CHANNEL;
	}

	return errcode;
}


char* libapp_test(char* dest, const char* source )
{
    char* r=dest;

    if((dest != NULL) && (source != NULL))
    {
        while((*r++ = *source++)!='\0');
    }
    return dest;
}


