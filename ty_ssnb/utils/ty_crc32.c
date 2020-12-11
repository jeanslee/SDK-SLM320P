#include "ty_crc32.h"


uint32_t POLYNOMIAL = 0xEDB88320 ;
int have_table = 0 ;
uint32_t table[256]= {0};


void make_table()
{
    int i, j, crc ;
    have_table = 1 ;
    for (i = 0 ; i < 256 ; i++)
        for (j = 0, table[i] = i ; j < 8 ; j++)
            table[i] = (table[i]>>1)^((table[i]&1)?POLYNOMIAL:0) ;
}


uint32_t ty_crc32(uint32_t crc, char *buff, int len)
{
    if (!have_table) make_table() ;
    crc = ~crc;
    for (int i = 0; i < len; i++)
        crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
    return ~crc;
}

