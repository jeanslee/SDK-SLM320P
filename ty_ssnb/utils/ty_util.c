#include"ty_util.h"
#include"md5.h"
#include"hex_util.h"
#include"time.h"
#include"hex_util.h"
#include"ty_os_adpt.h"


static char mDefaultBuff[MAX_SIGN_DATALEN] = {0};

#define random(x) rand()%(x)

char* parseIccid(char* iccid){
    if (iccid == NULL)
    {
        return NULL;
    }

    char* dest = (char*)lpa_calloc(strlen(iccid)+1,sizeof(char));
    
    uint8_t mBuff[20] = {0};
    int len = asc2hex(mBuff , iccid , strlen(iccid));

    UCHAR* mDest = (UCHAR*)lpa_calloc(len , sizeof(UCHAR));
    int i = 0;
    for (; i < len; i++)
    {
        int heigh = (mBuff[i] >> 4) & 0x0F;
        int low = mBuff[i] & 0x0F;

        dest[2*i] = charToHex(low);
        dest[2*i+1] = charToHex(heigh);
    }
    if (mDest)
    {
        lpa_free(mDest);
        mDest = NULL;
    }
    return dest;
}

#ifdef N58_PLAT
	#include"nwy_socket.h"
	#include"nwy_osi_api.h"
	int randomToByte(int len, UCHAR* dest)
	{
		srand(nwy_get_ms());
		int i = 0;
		for (i = 0; i < len; i++)
		{
			dest[i] = random(0xFF);
		}
		return len;
	}
	void getTimeStamp(char* timeBuff)
	{
		nwy_time_t julian_time;
		char timezone =0;
		nwy_get_time(&julian_time, &timezone);
		sprintf(timeBuff,"%04d%02d%02d%02d%02d%02d",julian_time.year,julian_time.mon,julian_time.day,julian_time.hour,julian_time.min,julian_time.sec);
	}
#elif defined NB12_PLAT
	
	int randomToByte(int len, UCHAR* dest)
	{
		srand(ty_rtc_get_sec(0));
		int i = 0;
		for (i = 0; i < len; i++)
		{
			dest[i] = random(0xFF);
		}
		return len;
	}

	void getTimeStamp(char* timeBuff)
	{
		ty_get_timestamp (timeBuff);
	}

#else 
	//获取len字节随机数，包含数字跟字符
	int randomToByte(int len, UCHAR* dest)
	{
		srand((int)time(0));
		int i = 0;
		for (i = 0; i < len; i++)
		{
			dest[i] = random(0xFF);
		}
		return len;
	}

	//纯数字随机数
	int numRandom(int len, char* dest)
	{
		srand((unsigned)time(NULL));
		for (int j = 0; j < len; j++)
			sprintf(dest + j, "%d", rand() % 10);
	}

#if 0
	//精确到秒
	void getTimeStamp(char* timeBuff)
	{
		time_t t;
		t = time(NULL);
		struct tm *lt;
		int ii = time(&t);
		t = time(NULL);
		lt = localtime(&t);
		strftime(timeBuff, 24, "%Y%m%d%H%M%S", lt);
	}
#else

	//精确到毫秒
	void getTimeStamp(char* timeBuff)
	{
		ty_get_timestamp(timeBuff);
		//SYSTEMTIME	st;
		//GetLocalTime(&st);
		//sprintf(timeBuff,"%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}
#endif

#endif



void freeStringList(TArrayList* list){
    if (NULL == list || list->size <=0)
    {
       return;
    }
    int i = 0 ;
    for (; i < list->size ;i++)
    {
        if (list->mArray[i].data)
        {
            lpa_free(list->mArray[i].data);
            list->mArray[i].data = NULL;
        }
    }
    if (list->mArray)
    {
       lpa_free(list->mArray);
       list->mArray = NULL;
    }
    list->size = 0;
}


int exchange(TArrayList* mList,int i){
    char* cha1 = mList->mArray[i].data;  
    char* cha2 = mList->mArray[i+1].data;

    char* tmp = (char*)lpa_calloc(strlen(cha1)+1,sizeof(char));
    memcpy(tmp , cha1 , strlen(cha1));
    
    if (mList->mArray[i].data)
    {
        lpa_free(mList->mArray[i].data);
        mList->mArray[i].data = NULL;
    }
    
    mList->mArray[i].data = (char*)lpa_calloc(strlen(cha2)+1,sizeof(char));
    memcpy(mList->mArray[i].data , cha2 , strlen(cha2));

    if (mList->mArray[i+1].data)
    {
        lpa_free(mList->mArray[i+1].data);
        mList->mArray[i+1].data = NULL;
    }
    mList->mArray[i+1].data = (char*)lpa_calloc(strlen(tmp)+1,sizeof(char));
    memcpy(mList->mArray[i+1].data , tmp , strlen(tmp));

    lpa_free(tmp);
    return 0;
}

int exchangeString(char* strBuff1,char* strBuff2){
    int len1 = strlen(strBuff1);
    int len2 = strlen(strBuff2);

    char* tmp = (char*)lpa_calloc(len1 + 1,sizeof(char));
    memcpy(tmp , strBuff1 , len1);
    
    if (strBuff1)
    {
        lpa_free(strBuff1);
        strBuff1 = NULL;
    }
    
    strBuff1 = (char*)lpa_calloc(len2 + 1,sizeof(char));
    memcpy(strBuff1 , strBuff2 , len2);

    if (strBuff2)
    {
        lpa_free(strBuff2);
        strBuff2 = NULL;
    }
    strBuff2 = (char*)lpa_calloc(len1 + 1,sizeof(char));
    memcpy(strBuff2 , tmp , len1);

    lpa_free(tmp);
    return 0;
}

int sort(TArrayList* mList)
{
    if (NULL == mList || mList->size<=0)
    {
        return -1;
    }
    for (int i = 0; i < mList->size-1; i++)
    {
        for (int j = 0; j < mList->size-1-i; j++)
        {
            char* cha1 = mList->mArray[j].data;
            char* cha2 = mList->mArray[j+1].data;
            if (strcmp(cha1,cha2) > 0 )
            {
                exchange(mList,j);
            } 
        }
    }
    return 0;   
}


int getSort(TArrayList* mList , char* key,char** dest){
    if (NULL == mList || mList->size <= 0 || NULL == key)
    {
        return -1;
    }
    memset(mDefaultBuff , 0 , MAX_SIGN_DATALEN);
    int offset = 0;
    for (int i = 0; i < mList->size; i++)
    {
       offset += snprintf(mDefaultBuff + offset,MAX_SIGN_DATALEN - offset, "%s&", mList->mArray[i].data);
    }
    offset += snprintf(mDefaultBuff + offset,MAX_SIGN_DATALEN - offset,"key=%s",key);
    int ret = enctypt_md5((unsigned char*)mDefaultBuff,dest);
    return ret;
}


int addStringList(char* data,TArrayList* mList){
    if (NULL == data)
    {
        return -1;
    }
    int size = mList->size;
    if (size == 0)
    {
        mList->mArray = (TString*)lpa_calloc(1,sizeof(TString));
    }else{
        TString* pTemp  = (TString*)lpa_calloc(size,sizeof(TString));
        memcpy(pTemp, mList->mArray , size *sizeof(TString));
        if (mList->mArray)
        {
            lpa_free(mList->mArray);
            mList->mArray = NULL;
        }
        mList->mArray = (TString*)lpa_calloc(size + 1,sizeof(TString));
        memcpy(mList->mArray, pTemp, size * sizeof(TString));
        if (NULL != pTemp)
        {
            lpa_free(pTemp);
            pTemp = NULL;
        }
    }
    int dataLen = strlen(data);
    mList->mArray[size].data = (char*)lpa_calloc(dataLen + 1,sizeof(char));
    memcpy(mList->mArray[size].data , data , dataLen);

    mList->size = size + 1;
    return 0;
}


int enctypt_md5(unsigned char* data,char** dest)
{
	unsigned char decrypt[16];  
    MD5_CTX md5;
	MD5Init(&md5);         		
	MD5Update(&md5,data,strlen((char *)data));
	MD5Final(&md5,decrypt); 

    *dest = byteToStr(decrypt,16);
    return 0;
}


/* 
 * 将字符转换为数值 
 * */  
int c2i(char ch)  
{  
        // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
        if(isdigit(ch))  
                return ch - 48;  
  
        // 如果是字母，但不是A~F,a~f则返回  
        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
                return -1;  
  
        // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
        // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
        if(isalpha(ch))  
                return isupper(ch) ? ch - 55 : ch - 87;  
  
        return -1;  
}  
  
/* 
 * 功能：将十六进制字符串转换为整型(int)数值 
 * */  
int hex2dec(char *hex)  
{  
        int len;  
        int num = 0;  
        int temp;  
        int bits;  
        int i;  
          
        // 此例中 hex = "1de" 长度为3, hex是main函数传递的  
        len = strlen(hex);  
  
        for (i=0, temp=0; i<len; i++, temp=0)  
        {  
                // 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
                // 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
                // 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
                temp = c2i( *(hex + i) );  
                // 总共3位，一个16进制位用 4 bit保存  
                // 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
                // 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
                // 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
                bits = (len - i - 1) * 4;  
                temp = temp << bits;  
  
                // 此处也可以用 num += temp;进行累加  
                num = num | temp;  
        }  
  
        // 返回结果  
        return num;  
} 

/*返回str2第一次出现在str1中的位置(下表索引),不存在返回-1*/
int indexOf(char *str1,char *str2)  
{  
    char *p=str1;  
    int i = 0;  
    p=strstr(str1,str2);  
    if(p==NULL)  
        return -1;  
    else{  
        while(str1!=p)  
        {  
            str1++;  
            i++;  
        }  
    }  
    return i;  
}  
/*返回str1中最后一次出现str2的位置(下标),不存在返回-1*/  
int lastIndexOf(char *str1,char *str2)  
{  
    char *p=str1;  
    int i=0,len=strlen(str2);  
    p=strstr(str1,str2);  
    if(p==NULL)return -1;  
    while(p!=NULL)  
    {  
        for(;str1!=p;str1++)i++;  
        p=p+len;  
        p=strstr(p,str2);  
    }  
    return i;  
}  

void substring(char *dest,char *src,int start,int end)  
{  
    int i=start;  
    if(start>strlen(src))return;  
    if(end>strlen(src))  
        end=strlen(src);  
    while(i<end)  
    {     
        dest[i-start]=src[i];  
        i++;  
    }  
    dest[i-start]='\0';  
    return;  
}  

int split(char* src,char* separator ,TArrayList* apduList)
{
    char *pNext;
    int count = 0;
    if (src == NULL || strlen(src) == 0)
         return -1;
    if (separator == NULL || strlen(separator) == 0)
         return -1;

    pNext = strtok(src,separator);
    while(pNext != NULL) 
    {
        addStringList(pNext,apduList);
        count++;
        pNext = strtok(NULL,separator);  
    }  
    apduList->size = count;
    return 0;
}

