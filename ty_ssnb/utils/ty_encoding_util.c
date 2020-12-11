#include"ty_encoding_util.h"

int encodeRequest(char* appKey, char* srcData, char** dest)
{
	if (NULL == appKey || NULL == srcData)
	{
		return -1;
	}
	UCHAR* key = NULL;
	int keyLen = 0;
	int dataLen = strlen(srcData);

	keyLen = strToByte(appKey, &key);
	if (NULL == key)
	{
		return -1;
	}

	//char testRandom[] = "57564950505049525755575057565157";
	char testRandom[33] = { 0 };
	numRandom(32 , testRandom);
	LOG("testRandom = %s \r\n", testRandom);

	UCHAR* mRandom = NULL;
	int mRandomLen = strToByte(testRandom, &mRandom);
	if (NULL == mRandom)
	{
		return -1;
	}

	UCHAR mProcessKey[16] = { 0 };
	memset(mProcessKey, 0, 16);
	int mProcessKeyLen = des3_ecb_encrypt(mProcessKey, mRandom, mRandomLen, key, keyLen);

	char* mPaddingData = NULL;
	int paddingLen = 8 - strlen(srcData) % 8;

	char* hexData = NULL;
	if (paddingLen > 0)
	{
		mPaddingData = (char*)lpa_calloc(dataLen + paddingLen, sizeof(char));
		if (mPaddingData == NULL)
		{
			return -1;
		}

		memcpy(mPaddingData, srcData, dataLen);
		mPaddingData[dataLen] = 0x80;

		hexData = byteToStr(mPaddingData, dataLen + paddingLen);
	}
	else {
		hexData = byteToStr(srcData, dataLen);
	}
	if (NULL == hexData)
	{
		return -1;
	}

	int mEncLen = strlen(hexData);
	UCHAR* mEncData = (UCHAR*)lpa_calloc(mEncLen, sizeof(UCHAR));
	if (mEncData == NULL)
	{
		return -1;
	}
	mEncLen = des3_ecb_encrypt(mEncData, hexData, strlen(hexData), mProcessKey, mProcessKeyLen);

	int mLen = mRandomLen + mEncLen;
	UCHAR* mByteBuff = (UCHAR*)lpa_calloc(mLen, sizeof(UCHAR));
	if (mByteBuff == NULL)
	{
		return -1;
	}
	memcpy(mByteBuff, mRandom, mRandomLen);
	memcpy(mByteBuff + mRandomLen, mEncData, mEncLen);
	
	*dest = byteToStr(mByteBuff, mLen);
	if (NULL == *dest)
	{
		return -1;
	}
	if (key)
	{
		lpa_free(key);
		key = NULL;
	}
	if (mPaddingData)
	{
		lpa_free(mPaddingData);
		mPaddingData = NULL;
	}
	if (hexData)
	{
		lpa_free(hexData);
		hexData = NULL;
	}
	if (mEncData)
	{
		lpa_free(mEncData);
		mEncData = NULL;
	}
	if (mByteBuff)
	{
		lpa_free(mByteBuff);
		mByteBuff = NULL;
	}
	if (mRandom)
	{
		lpa_free(mRandom);
		mRandom = NULL;
	}

	return mLen;
}

static int removePadding(UCHAR* data,int dataLen,UCHAR** dest)
{
    if (NULL == data || dataLen == 0)
    {
        return -1;
    }
    int index = -1;
    for (int i = dataLen -1; i >=0; i--)
    {
        if (data[i]=='0')
        {
            continue;
        }else if (data[i]=='8')
        {
            index = i;
            break;
        }      
    }
    //LOG("index =  %d \r\n", index);
    if (index == -1)
    {
        return -1;
    }
    int len = index+1;
    *dest = (UCHAR*)lpa_calloc(len, sizeof(UCHAR));
    memcpy(*dest , data , index);
    return index;
}

int decodeResponse(char* appKey , char*srcData , char** dest)
{
    UCHAR* data = NULL;
    int len = strToByte(srcData,&data);

    UCHAR* key = NULL;
    int keyLen = strToByte(appKey,&key);

    UCHAR random[16] = {0};
    memset(random,0,16);
    memcpy(random,data,16);
    
#ifdef DEBUG
    char* mRandomStr = byteToStr(random,16);
    LOG("random =  %s , %d \r\n", mRandomStr, __LINE__);
    if (mRandomStr)
    {
        lpa_free(mRandomStr);
        mRandomStr = NULL;
	}
#endif

    int nLen = len-16; 
    UCHAR* nData = (UCHAR*)lpa_calloc(nLen,sizeof(UCHAR));
    memset(nData , 0 , nLen);
    memcpy(nData , data + 16 , nLen);
 
#ifdef DEBUG
     char* mEncodeDataStr = byteToStr(nData,nLen);
     LOG("mEncodeDataStr =  %s \r\n", mEncodeDataStr);
     if (mEncodeDataStr)
     {
        lpa_free(mEncodeDataStr);
        mEncodeDataStr = NULL;
     }
#endif

    UCHAR mProcessKey[16] = {0};
    memset(mProcessKey , 0 , 16);
    int mProcessKeyLen = des3_ecb_encrypt(mProcessKey , random , 16 , key , keyLen);

#ifdef DEBUG
     LOG("des3_ecb_decrypt  mProcessKeyLen =  %d\r\n", mProcessKeyLen);
     char* processKey = byteToStr(mProcessKey,mProcessKeyLen);
     LOG("processKey =  %s\r\n", processKey);
     if (processKey)
     {
        lpa_free(processKey);
        processKey = NULL;
     }
#endif  
    UCHAR* mDecodeData = (UCHAR*)lpa_calloc(nLen,sizeof(UCHAR));
    int ret = des3_ecb_decrypt(mDecodeData , nData , nLen , mProcessKey , 16);

    UCHAR* mHexData = NULL;
    int hexLen = removePadding(mDecodeData,nLen,&mHexData);
    if (hexLen > 0)
    {
        hexLen = strToByte(mHexData,(UCHAR**)dest);
    }
    if (data)
    {
        lpa_free(data);
        data = NULL;
    }
    if (key)
    {
        lpa_free(key);
        key = NULL;
    }
    if (nData)
    {
        lpa_free(nData);
        nData = NULL;
    }
    if (mDecodeData)
    {
        lpa_free(mDecodeData);
        mDecodeData = NULL;
    }
    if (mHexData)
    {
        lpa_free(mHexData);
        mHexData = NULL;
    }
    return 0;
}