#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "base64.h"
#include "hex_util.h"

#define LOG_TAG "base64"

#define LOG(fmt, ...)           LOGI(LOG_TAG,fmt, ##__VA_ARGS__)


static const char b64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

//#define BASE64_DECODE_DEBUG


//int base64_decode_dm(char *in, int inlen, uint8_t **out);
int base64_decode_dm (const char *src, size_t len, uint8_t **out) 
{
  int i = 0;
  int j = 0;
  int l = 0;
  size_t size = 0;
  unsigned char *dec = NULL;
  unsigned char buf[3];
  unsigned char tmp[4];
    //size_t *decsize
  // alloc
  dec = (unsigned char *) lpa_malloc(len);
  if (NULL == dec) { return -1; }

  // parse until end of source
  while (len--) {
    // break if char is `=' or not base64 char
    if ('=' == src[j]) { break; }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) { break; }

    // read up to 4 bytes at a time into `tmp'
    tmp[i++] = src[j++];

    // if 4 bytes read then decode into `buf'
    if (4 == i) {
      // translate values in `tmp' from table
      for (i = 0; i < 4; ++i) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[i] == b64_table[l]) {
            tmp[i] = l;
            break;
          }
        }
      }

      // decode
      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      // write decoded buffer to `dec'
      //dec = (unsigned char *) b64_buf_realloc(dec, size + 3);
      if (dec != NULL){
        for (i = 0; i < 3; ++i) {
          dec[size++] = buf[i];
        }
      } else {
        lpa_free(dec);
        dec = NULL;
        return -2;
      }

      // reset
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 4 times
    for (j = i; j < 4; ++j) {
      tmp[j] = '\0';
    }

    // translate remainder
    for (j = 0; j < 4; ++j) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[j] == b64_table[l]) {
            tmp[j] = l;
            break;
          }
        }
    }

    // decode remainder
    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

    // write remainer decoded buffer to `dec'
    //dec = (unsigned char *)b64_buf_realloc(dec, size + (i - 1));
    if (dec != NULL){
      for (j = 0; (j < i - 1); ++j) {
        dec[size++] = buf[j];
      }
    } else {
        lpa_free(dec);
        dec = NULL;
        return -3;
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  //dec = (unsigned char *)b64_buf_realloc(dec, size + 1);
  if (dec != NULL){
    dec[size] = '\0';
  } else {
    lpa_free(dec);
    dec = NULL;  
    return -4;
  }

  // Return back the size of decoded string if demanded.
    *out = dec;
    #ifdef BASE64_DECODE_DEBUG
    char* hex = byteToStr (*out,(int)size);
    if(hex)
    {
        LOG("%s hex=%s \r\n",__FUNCTION__,hex);
        lpa_free(hex);
        hex = NULL;
    }
    #endif
    return size;
}

static inline int calc_enc_len_base64(int srclen)
{
    return (srclen+2)/3*4+1;
}

int base64_encode_dm(unsigned char *src, size_t len, uint8_t **enc)
{
  int i = 0;
  int j = 0;
  size_t size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  *enc = lpa_malloc(calc_enc_len_base64(len));//
  // alloc
  //enc = (char *) b64_buf_malloc();
  if (NULL == *enc) { return -1; }

  // parse until end of source
  while (len--) {
    // read up to 3 bytes at a time into `tmp'
    tmp[i++] = *(src++);

    // if 3 bytes read then encode into `buf'
    if (3 == i) {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      // allocate 4 new byts for `enc` and
      // then translate each encoded buffer
      // part by index from the base 64 index table
      // into `enc' unsigned char array
      //enc = (char *) b64_buf_realloc(enc, size + 4);
      for (i = 0; i < 4; ++i) {
        *enc[size++] = b64_table[buf[i]];
      }

      // reset index
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) {
      tmp[j] = '\0';
    }

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) {
      //enc = (char *) b64_buf_realloc(enc, size + 1);
      *enc[size++] = b64_table[buf[j]];
    }

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) {
      //enc = (char *) b64_buf_realloc(enc, size + 1);
      *enc[size++] = '=';
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  //enc = (char *) b64_buf_realloc(enc, size + 1);
  *enc[size] = '\0';
  return size;
}


int base64_encode(unsigned char *src, size_t len, uint8_t *enc)
{
	int i = 0;
	int j = 0;
	size_t size = 0;
	unsigned char buf[4];
	unsigned char tmp[3];

	// alloc
	//enc = (char *) b64_buf_malloc();
	if (NULL == enc) { return -1; }

	// parse until end of source
	while (len--) {
		// read up to 3 bytes at a time into `tmp'
		tmp[i++] = *(src++);

		// if 3 bytes read then encode into `buf'
		if (3 == i) {
			buf[0] = (tmp[0] & 0xfc) >> 2;
			buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
			buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
			buf[3] = tmp[2] & 0x3f;

			// allocate 4 new byts for `enc` and
			// then translate each encoded buffer
			// part by index from the base 64 index table
			// into `enc' unsigned char array
			//enc = (char *) b64_buf_realloc(enc, size + 4);
			for (i = 0; i < 4; ++i) {
				enc[size++] = b64_table[buf[i]];
			}

			// reset index
			i = 0;
		}
	}

	// remainder
	if (i > 0) {
		// fill `tmp' with `\0' at most 3 times
		for (j = i; j < 3; ++j) {
			tmp[j] = '\0';
		}

		// perform same codec as above
		buf[0] = (tmp[0] & 0xfc) >> 2;
		buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
		buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
		buf[3] = tmp[2] & 0x3f;

		// perform same write to `enc` with new allocation
		for (j = 0; (j < i + 1); ++j) {
			//enc = (char *) b64_buf_realloc(enc, size + 1);
			enc[size++] = b64_table[buf[j]];
		}

		// while there is still a remainder
		// append `=' to `enc'
		while ((i++ < 3)) {
			//enc = (char *) b64_buf_realloc(enc, size + 1);
			enc[size++] = '=';
		}
	}

	// Make sure we have enough space to add '\0' character at end.
	//enc = (char *) b64_buf_realloc(enc, size + 1);
	enc[size] = '\0';
	return size;
}

