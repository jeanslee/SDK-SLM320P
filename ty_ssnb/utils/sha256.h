#ifndef __SHA256_H__
#define __SHA256_H__

#include"stdint.h"

/**
 * \brief          SHA-256 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
} iot_sha256_context;

void utils_sha256_init(iot_sha256_context *ctx);
void utils_sha256_starts (iot_sha256_context * ctx);
void utils_sha256_update(iot_sha256_context *ctx, const unsigned char *input, uint32_t ilen);
void utils_sha256_finish(iot_sha256_context *ctx, uint8_t output[32]);
void utils_hmac_sha256(const uint8_t *msg, uint32_t msg_len, const uint8_t *key, uint32_t key_len, uint8_t output[32]);


#endif
