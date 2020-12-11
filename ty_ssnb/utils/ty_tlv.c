#include"ty_tlv.h"

int toTLVByTag(UCHAR* tag, int tagLen, UCHAR*data, int dataLen, UCHAR** dest) {
	if (NULL == tag || strcmp((const char*)tag, "") == 0)
	{
		return toTLV(data, dataLen, dest);
	}
	UCHAR* mBodyBuff = NULL;
	int mBodyLen = toTLV(data, dataLen, &mBodyBuff);
	if (mBodyLen < 0)
	{
		return -1;
	}

	int mLen = mBodyLen + tagLen;
	*dest = (UCHAR*)lpa_calloc(mLen, sizeof(UCHAR));

	memcpy(*dest, tag, tagLen);
	memcpy(*dest + tagLen, mBodyBuff, mBodyLen);

	if (NULL != mBodyBuff)
	{
		lpa_free(mBodyBuff);
		mBodyBuff = NULL;
	}

	return mLen;
}

int toTLV(UCHAR* data, int len, UCHAR** dest) {
	int offset = 0;
	if (len > 65535)
	{
		*dest = (UCHAR*)lpa_calloc(4 + len, sizeof(UCHAR));
		if (*dest == NULL)
		{
			return -1;
		}

		(*dest)[offset++] = 0x83;
		(*dest)[offset++] = (len >> 16) & 0xFF;
		(*dest)[offset++] = (len >> 8) & 0xFF;
		(*dest)[offset++] = len & 0xFF;

		memcpy(*dest + offset, data, len);

		return len + 4;

	}
	else if (len > 255)
	{
		(*dest) = (UCHAR*)lpa_calloc(3 + len, sizeof(UCHAR));
		if (*dest == NULL)
		{
			return -1;
		}
		(*dest)[offset++] = 0x82;
		(*dest)[offset++] = (len >> 8) & 0xFF;
		(*dest)[offset++] = len & 0xFF;
		memcpy(*dest + offset, data, len);

		return len + 3;
	}
	else if (len > 127)
	{
		(*dest) = (UCHAR*)lpa_calloc(2 + len, sizeof(UCHAR));
		(*dest)[offset++] = 0x81;
		(*dest)[offset++] = len & 0xFF;
		memcpy(*dest + offset, data, len);

		return len + 2;
	}
	else {
		(*dest) = (UCHAR*)lpa_calloc(1 + len, sizeof(UCHAR));
		(*dest)[0] = len & 0xFF;
		if (NULL != data && len > 0)
		{
			memcpy(*dest + 1, data, len);
		}

		return len + 1;
	}
}

int indexOfByTag(UCHAR* data,int dataLen,UCHAR* tag,int tagLen,int offset){
    int i = 0;
    int index = -1;
    for ( i = offset ; i < dataLen;i++)
    {
        if (memcmp(tag,data+i,tagLen) == 0 )
        {
            index = i;
            break;
        }
    }
    return index;
}


int determineTLV(UCHAR* data,int dataLen,UCHAR* beginTag,int beginLen,UCHAR* endTag,int endLen,UCHAR** dest){
    if (NULL == data || dataLen <=0 || NULL == beginTag || beginLen<=0 || NULL ==endTag || endLen<=0)
    {
        return -1;
    }
    int beginIndex = indexOfByTag(data,dataLen,beginTag,beginLen,0);
    int endIndex = indexOfByTag(data,dataLen,endTag,endLen,beginIndex);
    
    while (beginIndex<=dataLen && beginIndex>=0 && endIndex>=0)
    {
        if( beginIndex < endIndex){
            while ( beginIndex < endIndex && beginIndex >=0 )
            {
                int mLen = endIndex - beginIndex - beginLen;
                int offset = beginIndex + beginLen;
                int hexLen = data[offset]&0xFF;

                if (hexLen == mLen-1)
                {
                    int len = mLen -1;
                    *dest = (UCHAR*)lpa_calloc(len,sizeof(UCHAR));
                    memcpy(*dest , data+offset+1 , len);

                    return len;
                }else{
                    endIndex = indexOfByTag(data,dataLen,endTag,endLen,endIndex+1);
                }
            }
            beginIndex = indexOfByTag(data,dataLen,beginTag,beginLen,beginIndex+1);
            endIndex = indexOfByTag(data,dataLen,endTag,endLen,beginIndex);
        }else{
            endIndex = indexOfByTag(data,dataLen,endTag,endLen,endIndex+1);
        }
    }
    return 0 ;
}