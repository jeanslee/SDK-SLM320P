#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void prvThreadEntry(void *param)
{
    unsigned char plain[16] = "abcdefghijklmnop";
    unsigned char key[16] = "1234567891234567";
    unsigned char cipher_ecb[17] = {0}, cipher_cbc[17] = {0}, cipher_cfb128[17] = {0}, cipher_ctr[17] = {0},
                  cipher_ofb[17] = {0}, cipher_base64[25] = {0};
    unsigned char plain_decrypt_ecb[17] = {0}, plain_decrypt_cbc[17] = {0}, plain_decrypt_cfb128[17] = {0},
                  plain_decrypt_ctr[17] = {0}, plain_decrypt_ofb[17] = {0},
                  plain_decrypt_base64[17] = {0};
    unsigned int keybits = 128, len = 16;
    unsigned char iv[16] = {0}, nonce_counter[16] = {0}, stream_block[16] = {0};
    size_t offset, olen;

    mbedtls_aes_context ctx;

/******************************************************ECB mode************************************************************/
    /* aes initialize */
    MG_mbedtls_aes_init(&ctx);
    /* set the encryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    /* encryption of ecb mode */
    MG_mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plain, cipher_ecb);
    cipher_ecb[len] = '\0';
    OSI_PRINTFI("ciphertext in ecb mode is %s", cipher_ecb);
    /* set the decryption key */
    MG_mbedtls_aes_setkey_dec(&ctx, key, keybits);
    /* decryption of ecb mode */
    MG_mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, cipher_ecb, plain_decrypt_ecb);
    plain_decrypt_ecb[len] = '\0';
    OSI_PRINTFI("plain after decryption in ecb mode is %s", plain_decrypt_ecb);
    /* aes free */
    MG_mbedtls_aes_free(&ctx);
/******************************************************ECB mode************************************************************/

/******************************************************CBC mode************************************************************/
    /* aes initialize */
    MG_mbedtls_aes_init(&ctx);
    /* set the encryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    memset(iv, 0, len);
    /* encryption of cbc mode */
    MG_mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, len, iv, plain, cipher_cbc);
    cipher_cbc[len] = '\0';
    OSI_PRINTFI("ciphertext in cbc mode is %s", cipher_cbc);
    /* set the decryption key */
    MG_mbedtls_aes_setkey_dec(&ctx, key, keybits);
    memset(iv, 0, len);
    /* decryption of cbc mode */
    MG_mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, len, iv, cipher_cbc, plain_decrypt_cbc);
    plain_decrypt_cbc[len] = '\0';
    OSI_PRINTFI("plain after decryption in cbc mode is %s", plain_decrypt_cbc);
    /* aes free */
    MG_mbedtls_aes_free(&ctx);
/******************************************************CBC mode************************************************************/

/******************************************************CFB128 mode*********************************************************/
    /* aes initialize */
    MG_mbedtls_aes_init(&ctx);
    /* set the encryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    memset(iv, 1, len);
    offset = 0;
    /* encryption of cfb128 mode */
    MG_mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT, len, &offset, iv, plain, cipher_cfb128);
    cipher_cfb128[len] = '\0';
    OSI_PRINTFI("ciphertext in cfb128 mode is %s", cipher_cfb128);
    /* set the decryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    memset(iv, 1, len);
    offset = 0;
    /* decryption of cfb128 mode */
    MG_mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, len, &offset, iv, cipher_cfb128, plain_decrypt_cfb128);
    plain_decrypt_cfb128[len] = '\0';
    OSI_PRINTFI("plain after decryption in cfb128 mode is %s", plain_decrypt_cfb128);
    /* aes free */
    MG_mbedtls_aes_free(&ctx);
/******************************************************CFB128 mode*********************************************************/

/******************************************************CTR mode************************************************************/
    /* aes initialize */
    MG_mbedtls_aes_init(&ctx);
    /* set the encryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    offset = 0;
    memset(nonce_counter, 0, len);
    /* encryption of ctr mode */
    MG_mbedtls_aes_crypt_ctr(&ctx, len, &offset, nonce_counter, stream_block, plain, cipher_ctr);
    //cipher_ctr[len] = '\0';
    //OSI_PRINTFI("ciphertext in ctr mode is %s", cipher_ctr);
    /* set the decryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    offset = 0;
    memset(nonce_counter, 0, len);
    /* decryption of cbc mode */
    MG_mbedtls_aes_crypt_ctr(&ctx, len, &offset, nonce_counter, stream_block, cipher_ctr, plain_decrypt_ctr);
    plain_decrypt_ctr[16] = '\0';
    OSI_PRINTFI("plain after decryption in ctr mode is %s", plain_decrypt_ctr);
    /* aes free */
    MG_mbedtls_aes_free(&ctx);
/******************************************************CTR mode************************************************************/

/******************************************************OFB mode************************************************************/
     /* aes initialize */
    MG_mbedtls_aes_init(&ctx);
    /* set the encryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    memset(iv, 2, len);
    offset = 0;
    /* encryption of ofb mode */
    MG_mbedtls_aes_crypt_ofb(&ctx, len, &offset, iv, plain, cipher_ofb);
    cipher_ofb[len] = '\0';
    OSI_PRINTFI("ciphertext in ofb mode is %s", cipher_ofb);
    /* set the decryption key */
    MG_mbedtls_aes_setkey_enc(&ctx, key, keybits);
    memset(iv, 2, len);
    offset = 0;
    /* decryption of ofb mode */
    MG_mbedtls_aes_crypt_ofb(&ctx, len, &offset, iv, cipher_ofb, plain_decrypt_ofb);
    plain_decrypt_ofb[len] = '\0';
    OSI_PRINTFI("plain after decryption in ofb mode is %s", plain_decrypt_ofb);
    /* aes free */
    MG_mbedtls_aes_free(&ctx);
/******************************************************OFB mode************************************************************/

/******************************************************base64 mode*********************************************************/
    /* encryption of base64 mode */
    MG_mbedtls_base64_encode(cipher_base64, sizeof(cipher_base64), &olen, plain, len);
    cipher_base64[24] = '\0';
    OSI_PRINTFI("ciphertext in base mode is %s", cipher_base64);
    /* decryption of base64 mode */
    MG_mbedtls_base64_decode(plain_decrypt_base64, sizeof(plain_decrypt_base64), &olen, cipher_base64, 24);
    plain_decrypt_base64[16] = '\0';
    OSI_PRINTFI("plain after decryption in base64 mode is %s", plain_decrypt_base64);
/******************************************************base64 mode*********************************************************/

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
