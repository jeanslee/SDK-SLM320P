#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')
#include "mbedtls/aes.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const unsigned char aes_test_xts_data_unit[][16] =
{
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

static const unsigned char aes_test_xts_key[][32] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
      0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
      0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 },
    { 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
      0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
      0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
      0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 },
};

static const unsigned char aes_test_xts_ct32[][32] =
{
    { 0x91, 0x7c, 0xf6, 0x9e, 0xbd, 0x68, 0xb2, 0xec,
      0x9b, 0x9f, 0xe9, 0xa3, 0xea, 0xdd, 0xa6, 0x92,
      0xcd, 0x43, 0xd2, 0xf5, 0x95, 0x98, 0xed, 0x85,
      0x8c, 0x02, 0xc2, 0x65, 0x2f, 0xbf, 0x92, 0x2e },
    { 0xc4, 0x54, 0x18, 0x5e, 0x6a, 0x16, 0x93, 0x6e,
      0x39, 0x33, 0x40, 0x38, 0xac, 0xef, 0x83, 0x8b,
      0xfb, 0x18, 0x6f, 0xff, 0x74, 0x80, 0xad, 0xc4,
      0x28, 0x93, 0x82, 0xec, 0xd6, 0xd3, 0x94, 0xf0 },
    { 0xaf, 0x85, 0x33, 0x6b, 0x59, 0x7a, 0xfc, 0x1a,
      0x90, 0x0b, 0x2e, 0xb2, 0x1e, 0xc9, 0x49, 0xd2,
      0x92, 0xdf, 0x4c, 0x04, 0x7e, 0x0b, 0x21, 0x53,
      0x21, 0x86, 0xa5, 0x97, 0x1a, 0x22, 0x7a, 0x89 },
};

static const unsigned char aes_test_xts_pt32[][32] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 },
    { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
      0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 },
};

static void prvThreadEntry(void *param)
{
    int len, ret;
    unsigned char key[32];
    unsigned char buf[64];
    const unsigned char *aes_tests;
    mbedtls_aes_xts_context ctx_xts;

    /*
     * XTS mode
     */
    MG_mbedtls_aes_xts_init( &ctx_xts );

    const unsigned char *data_unit;

    memset( key, 0, sizeof( key ) );
    memcpy( key, aes_test_xts_key[0], 32 );
    data_unit = aes_test_xts_data_unit[0];

    len = sizeof( *aes_test_xts_ct32 );
    OSI_LOGE(0, "pjw--prepare mbedtls_aes_xts_setkey_enc");
    ret = MG_mbedtls_aes_xts_setkey_enc( &ctx_xts, key, 256 );
    if( ret != 0)
    { 
        OSI_LOGE(0, "failed1");
        goto exit;
    }
    memcpy( buf, aes_test_xts_pt32[0], len );
    aes_tests = aes_test_xts_ct32[0];

    OSI_LOGE(0, "pjw--prepare mbedtls_aes_crypt_xts");
    ret = MG_mbedtls_aes_crypt_xts( &ctx_xts, MBEDTLS_AES_ENCRYPT, len, data_unit, buf, buf );
    buf[32] = '\0';
    OSI_PRINTFI("ciphertext in xts mode is %s", buf);
    
    OSI_LOGE(0, "pjw--prepare mbedtls_aes_xts_setkey_dec");
    ret = MG_mbedtls_aes_xts_setkey_dec( &ctx_xts, key, 256 );
    if( ret != 0)
    {
        OSI_LOGE(0, "failed2");
        goto exit;
    }
    memcpy( buf, aes_test_xts_ct32[0], len );
    aes_tests = aes_test_xts_pt32[0];

    OSI_LOGE(0, "pjw--prepare mbedtls_aes_crypt_xts");
    ret = MG_mbedtls_aes_crypt_xts( &ctx_xts, MBEDTLS_AES_DECRYPT, len, data_unit, buf, buf );
    //OSI_PRINTFI("plain after encryption in xts mode is %s", buf);
    if( memcmp( buf, aes_tests, len ) != 0 )
    {
        OSI_LOGE(0, "failed3");
        goto exit;
    }
    else
        OSI_LOGE(0, "succeed"); 

    MG_mbedtls_aes_xts_free( &ctx_xts );
exit:    
    MG_osiThreadExit();
}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "mg example enter");
    MG_osiThreadCreate("mg-aes", prvThreadEntry, NULL, OSI_PRIORITY_NORMAL, 1024*2);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "mg example exit");
}