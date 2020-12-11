/* //device/system/reference-ril/at_tok.c
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "at_tok.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

//static nwy_at_info _g_at_cmd;

/**
 * Starts tokenizing an AT response string
 * returns -1 if this is not a valid response string, 0 on success.
 * updates *p_cur with current position
 */

int at_tok_start(char **p_cur)
{
    if (*p_cur == NULL) {
        return -1;
    }

    // skip prefix
    // consume "^[^:]:"

    char* tmp = strchr(*p_cur, ':');

    if (*tmp == NULL) {
        return -1;
    }
	int skip = tmp - *p_cur;
    tmp++;
	*p_cur = tmp;

    return skip;
}

static void skipWhiteSpace(char **p_cur)
{
    if (*p_cur == NULL) return;

    while (**p_cur != '\0' && isspace(**p_cur)) {
        (*p_cur)++;
    }
}

static void skipNextComma(char **p_cur)
{
    if (*p_cur == NULL) return;

    while (**p_cur != '\0' && **p_cur != ',') {
        (*p_cur)++;
    }

    if (**p_cur == ',') {
        (*p_cur)++;
    }
}

char *strsep (char **stringp, const char *delim)
{
  char *begin, *end;
  begin = *stringp;
  if (begin == NULL)
    return NULL;
  /* Find the end of the token.  */
  end = begin + strcspn (begin, delim);
  if (*end)
    {
      /* Terminate the token and set *STRINGP past NUL character.  */
      *end++ = '\0';
      *stringp = end;
    }
  else
    /* No more delimiters; this is the last token.  */
    *stringp = NULL;
  return begin;
}


static char * nextTok(char **p_cur)
{
    char *ret = NULL;

    skipWhiteSpace(p_cur);

    if (*p_cur == NULL) {
        ret = NULL;
    } else if (**p_cur == '"') {
        (*p_cur)++;
        ret = strsep(p_cur, "\"");
        skipNextComma(p_cur);
    } else {
        ret = strsep(p_cur, ",");
    }

    return ret;
}

static int nextTokCopy(char **p_cur,char *p_out,int ilen)
{
    int ret = 0;
    char *index = NULL;
    skipWhiteSpace(p_cur);

    if (*p_cur == NULL) {
        ret = -1;
    } else if (**p_cur == '"') {
        (*p_cur)++;
        index = strstr(*p_cur, "\"");
        if(ilen >= index-*p_cur)
            memcpy(p_out,*p_cur,index-*p_cur);
        else ret = -2;
        skipNextComma(p_cur);
    } else {
        index = strstr(*p_cur, ",");
        if(ilen >= index-(*p_cur))
         memcpy(p_out,*p_cur,index-(*p_cur));
         else ret = -3;

    }

    return ret;
}


/**
 * Parses the next integer in the AT response line and places it in *p_out
 * returns 0 on success and -1 on fail
 * updates *p_cur
 * "base" is the same as the base param in strtol
 */

static int at_tok_nextint_base(char **p_cur, int *p_out, int base, int  uns)
{
    char *ret;

    if (*p_cur == NULL) {
        return -1;
    }

    ret = nextTok(p_cur);

    if (ret == NULL) {
        return -2;
    } else {
        long l;
        char *end;

        if (uns)
            l = strtoul(ret, &end, base);
        else
            l = strtol(ret, &end, base);

        *p_out = (int)l;

        if (end == ret) {
            return -3;
        }
    }

    return 0;
}

/**
 * Parses the next base 10 integer in the AT response line
 * and places it in *p_out
 * returns 0 on success and -1 on fail
 * updates *p_cur
 */
int at_tok_nextint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 10, 0);
}

/**
 * Parses the next base 16 integer in the AT response line
 * and places it in *p_out
 * returns 0 on success and -1 on fail
 * updates *p_cur
 */
int at_tok_nexthexint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 16, 1);
}

int at_tok_nextbool(char **p_cur, char *p_out)
{
    int ret;
    int result;

    ret = at_tok_nextint(p_cur, &result);

    if (ret < 0) {
        return -1;
    }

    // booleans should be 0 or 1
    if (!(result == 0 || result == 1)) {
        return -1;
    }

    if (p_out != NULL) {
        *p_out = (char)result;
    }

    return ret;
}

int at_tok_nextstr(char **p_cur, char **p_out)
{
    if (*p_cur == NULL) {
        return -1;
    }

    *p_out = nextTok(p_cur);

    return 0;
}

int at_tok_nextstr_copy(char **p_cur, char *p_out,int ilen)
{
    if (*p_cur == NULL) {
        return -1;
    }

    return nextTokCopy(p_cur,p_out,ilen);
}


/** returns 1 on "has more tokens" and 0 if no */
int at_tok_hasmore(char **p_cur)
{
    return ! (*p_cur == NULL || **p_cur == '\0');
}


/*
 *  Add to skip comma
 *  by Zee
 */
int skipComma(char **p_cur)
{
    if(*p_cur == NULL) return -1;

    while(**p_cur != '\0' && **p_cur != ',')
    {
        (*p_cur)++;
    }

    if(**p_cur == ',')
    {
        (*p_cur)++;
    }

    if(*p_cur == NULL) return -1;


    return 0;
}


/** return the num of $(*target) char in $(*p_cur) string */
int at_tok_charcounter(char *p_cur, char *target, int *p_outcount)
{
    int targetcounter = 0;

    if(p_cur == NULL)
        return -1;

    while(*p_cur != '\0')
    {
        if( *p_cur == *target)
            targetcounter++;
        p_cur++;
    }
        *p_outcount=targetcounter;
    return 0;
}


/** return the element value between beginTag and endTag,
 *  also return the new string begin from endTag.(endTag is
 *  not in the new string)
 */
char* at_tok_getElementValue(const char *p_in, const char *beginTag, const char *endTag, char **remaining)
{
    char *ret = NULL;
    char *start = NULL;
    char *end = NULL;
    int n = 0;
    int m = 0;

    if(p_in == NULL || beginTag == NULL || endTag == NULL)
        return NULL;

    start = strstr(p_in, beginTag);
    if(start != NULL)
        end = strstr(p_in,endTag);
    if(end != NULL)
    {
        n = strlen(beginTag);
        m = end - (start+n);

        ret = (char *)malloc(m * sizeof(char *));
        if(ret != NULL)
        {
            strncpy(ret, start+n, m);
            ret[m] = (char)0;
        }

        if(remaining != NULL)
            *remaining = end + strlen(endTag);
    }

    return ret;
}

char *strrpl(char *in, char *out, int outlen, const char *src, char *dst)
{
    char *p = in;
    unsigned int  len = outlen - 1;

    if((NULL == src) || (NULL == dst) || (NULL == in) || (NULL == out))
    {
        return NULL;
    }
    if((strcmp(in, "") == 0) || (strcmp(src, "") == 0))
    {
        return NULL;
    }
    if(outlen <= 0)
    {
        return NULL;
    }

    while((*p != '\0') && (len > 0))
    {
        if(strncmp(p, src, strlen(src)) != 0)
        {
            int n = strlen(out);

            out[n] = *p;
            out[n + 1] = '\0';

            p++;
            len--;
        }
        else
        {
            strcat(out, dst);
            p += strlen(src);
            len -= strlen(dst);
        }
    }

    return out;
}


int strStartsWith(const char *line, const char *prefix)
{
    for ( ; *line != '\0' && *prefix != '\0' ; line++, prefix++) {
        if (*line != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}


