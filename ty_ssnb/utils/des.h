



#include "ty_os_adpt.h"

unsigned int des3_ecb_encrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen);

unsigned int des3_ecb_decrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen);

unsigned int des3_cbc_encrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen,
                              unsigned char *piv);

unsigned int des3_cbc_decrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen,
                              unsigned char *piv);

