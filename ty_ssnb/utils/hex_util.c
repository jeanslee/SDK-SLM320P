#include "hex_util.h"


//ASCIIתhex
int asc2hex(uint8_t *dest, const char *src, int srclen)
{
	uint8_t nibble[2];
	int i;
	int j;
	int destlen = srclen/2;
	if (!src || !src[0] || srclen%2) return 0;
	if (!dest) return srclen/2;

	for (i = 0; i < destlen; i ++)
	{
		nibble[0] = *src ++;
		nibble[1] = *src ++;
		for (j = 0; j < 2; j ++)
		{
			if (nibble[j] >= 'A' && nibble[j] <= 'F')
				nibble[j] = nibble[j] - 'A' + 10;
			else if (nibble[j] >= 'a' && nibble[j] <= 'f')
				nibble[j] = nibble[j] - 'a' + 10;
			else if (nibble[j] >= '0' && nibble[j] <= '9')
				nibble[j] = nibble[j] - '0';
			else
				return 0;
		}	
		dest[i] =  nibble[0] << 4;	// Set the high nibble
		dest[i] |= nibble[1];		// Set the low nibble
	}	
	return destlen;
}

int  byteorder(void)
{
	int  x = 1;
	return ( (*(char *)&x == 1) ? 1:0 );	
}

//short to char
void uint16ToChar( char* buf,uint16_t data )
{
	if(byteorder() == 1)
	{
		char tmpBuf[2];
		memcpy(tmpBuf,&data,2);

		buf[1] = tmpBuf[0];
		buf[0] = tmpBuf[1];
	}
	else
	{
		memcpy(buf,&data,2);
	}
}

//char to short
uint16_t charToUint16( char* buf )
{
	uint16_t temp = 0;
	
	if(byteorder() == 1)
	{
		char tmpBuf[2];
		tmpBuf[1] = buf[0];
		tmpBuf[0] = buf[1];

		memcpy(&temp,tmpBuf,2);
	}
	else
	{
		memcpy(&temp,buf,2);
	}

	return temp;
}

//char to int
unsigned int charToUint32( char * buf )
{
	unsigned int temp;

	if(byteorder() == 1)
	{
		char tmpBuf[4];
		tmpBuf[0] = buf[3];
		tmpBuf[1] = buf[2];
		tmpBuf[2] = buf[1];
		tmpBuf[3] = buf[0];

		memcpy(&temp,tmpBuf,4);
	}
	else
	{
		memcpy(&temp,buf,4);
	}
	return temp;

}

//int convert to char
void uint32ToChar( char* buf,unsigned int data)
{
	if(byteorder() == 1)
	{
		char tmpBuf[4];
		memcpy(tmpBuf,&data,4);
        buf[3] = tmpBuf[0];
        buf[2] = tmpBuf[1];
		buf[1] = tmpBuf[2];
		buf[0] = tmpBuf[3];
	}
	else
	{
		memcpy(buf,&data,4);
	}
}


void intToChar(int data, UCHAR* buf)
{
	if (byteorder() == 1)
	{
		UCHAR tmpBuf[4];
		memcpy(tmpBuf, &data, 4);
		buf[3] = tmpBuf[0];
		buf[2] = tmpBuf[1];
		buf[1] = tmpBuf[2];
		buf[0] = tmpBuf[3];
	}
	else
	{
		memcpy(buf, &data, 4);
	}
}

int hex2asc(uint8_t *hex, int hexLen,char *ascii,int destlen)
{
	if (NULL == hex || hexLen == 0 || destlen/2 < hexLen)
	{
		return 0;
	}
	
    int i, value;
    for( i = 0 ; i < hexLen ; i++) 
	{
        value = (hex[i]>>4);

        if(value > 9) 
		{
            value += 0x07;
        }

        ascii[2*i] = value + 0x30;

        value = ( hex[i] & 0x0F );
        if(value > 9) 
		{
            value += 0x07;
        }
        ascii[2*i+1] = value + 0x30;
    }
    return hexLen*2;
}


char charToHex(char ch)
{
    if (ch < 10)
    {
        return ch + '0';
    }
    else if (ch < 16)
    {
        return ch - 10 + 'A';
    }
    return '0';
}

char* byteToStr(UCHAR* src, int len)
{
	if (NULL == src || len <= 0)
    {
        return 0;
    }

    int i = 0;
    int nLen = 2 * len;
	char* pszDest = (char*)lpa_calloc(nLen+1,sizeof(char));
    if (NULL == pszDest)
	{
		return NULL;
	}
	
    while (i < len)
    {
        pszDest[2 * i] = charToHex((src[i] & 0xF0) >> 4);
        pszDest[2 * i + 1] = charToHex(src[i] & 0x0F);
        i = i + 1;
    }
	pszDest[nLen] = '\0';
    return pszDest;
}

int byte2int(UCHAR* src , int len) {
	int nResult = 0;
	for (int i = 0; i < len; i++) 
	{
		nResult += src[i] & 0xFF << ((len - i - 1) * 8);
	}
	return nResult;
}

int charToInt(char ch)
{
    int nTmp = 0;
    if (ch >= 'A' && ch <= 'F')
    {
        nTmp = ch - 'A' + 10;
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        nTmp = ch - 'a' + 10;
    } 
    else
    {
        nTmp = ch - '0';
    }

    return nTmp;
}


int strToByte(char* src, UCHAR** dest)
{
    if (NULL == src)
    {
        return 0;
    }
    int nSrcLen = strlen(src);
    
    if (0 == nSrcLen || 0 != nSrcLen % 2)
	{
		return 0;
	}

    int nLen = nSrcLen / 2;

    *dest = (UCHAR*)lpa_calloc(nLen+1, sizeof(UCHAR)); 
    (*dest)[nLen] = '\0';

    int i = 0;
	while (i < nSrcLen)
	{
		int nHigh = (charToInt(src[i]) << 4) & 0xFF;
		int nLow = charToInt(src[i + 1]) & 0x0F;
		(*dest)[i / 2] = nHigh + nLow;

		i = i + 2; 
	}
    return nLen;
}


