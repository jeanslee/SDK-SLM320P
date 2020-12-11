

#include <string.h>

typedef unsigned char uint8_t;

int base64_encode(unsigned char *src, size_t inlen, uint8_t *out);
int base64_encode_dm(unsigned char *src, size_t inlen, uint8_t **out);
//int base64_decode(char *in, size_t inlen, uint8_t *out);
int base64_decode_dm(const char *src, size_t inlen, uint8_t **out);



