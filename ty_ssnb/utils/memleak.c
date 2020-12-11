/**************************************************************************
   memleak.c: A simple module for debbuging memory leaks. ANSI C.
   Copyright (c) 2005, 2008 Stanislav Maslovski.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   *************************************************************************

   ===================
   Theory of operation
   ===================
   The program simply allocates a header together with a piece of data.
   The headers contain information about the calls and are organized in a
   bidirectional list. The history of malloc() - free() calls is collected
   in a separate place in a circular buffer.
   
   ============
   Limitations:
   ============
   I use size_t to store the block allocation sizes. The overall allocated
   memory count is stored as a long. In ANSI C there is no type modifier
   for printing size_t values, so they are printed by casting to unsigned long.
   Pointers are validated by simple linear search in the lists, do not expect it
   to be the same fast as the library routines.

   ==============
   How to use it:
   ==============
   include memleak.h into all modules of your program you want to debug.
   Place a call to dbg_init() somewhere in the beginning of execution.
   Add calls to dbg_mem_stat() or dbg_heap_dump() or others (see below)
   to any suspicious place of your code. You may use "keyword" argument
   to the dumping functions to reduce the amount of the output.

   ===============================
   void dbg_init(unsigned history_size)
   ===============================
   This initializes the debugger. history_size defines the maximum number of
   stored headers of the freed data.

   ========================
   void dbg_zero_stat(void)
   ========================
   clears statistics.

   =======================
   dbg_catch_sigsegv(void)
   =======================
   triggers on SIGSEGV signal handler.

   ==================================================
   int dbg_check_addr(char *msg, void *addr, int opt)
   ==================================================
   checks for addr in the tables given by opt. opt can be CHK_FREED or
   CHK_ALLOC or CHK_ALL; returns CHK_FREED, CHK_ALLOC, or 0 if not found.
   Also prints debugging message to stderr, prefixed with msg.

   =======================
   void dbg_mem_stat(void) 
   =======================
   This function prints call statistics to stderr. The location of the
   call is printed at the beginning of the status line. The status line
   looks like this:
   
   file:line m:<num> c: <num> r:<num> f:<num> mem:<num>
   
   where m, c, r, f count malloc(), calloc(), realloc(), and free() calls.
   mem equals the total amount of requested and not yet freed memory (not
   exactly the same as the amount of actually allocated physical memory). 
   
   =================================
   void dbg_heap_dump(char *keyword)
   =================================
   This function prints a dump of the current heap state to stderr. Every
   line of the dump corresponds to an allocated and not yet freed piece of
   data. Use "keyword" to narrower the dump: keyword = "" => entire heap
   is dumped, if not "", strstr() is used to select strings with keyword
   in them.

   ====================================
   void dbg_history_dump(char *keyword)
   ====================================
   dumps history of malloc() - free() calls. keyword is the same as above.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
//#include <unistd.h>

//#include "softap_api.h"
#if MEMLEAK_CHECK


extern void LOG(const char *format, ... );

#define LOGI(format, ...)   LOG(format, ##__VA_ARGS__)
#define LOGE(format, ...)	LOG(format, ##__VA_ARGS__)

//#define die(msg) (perror(msg), abort())
//#define die(msg) (perror(msg))

#define die(msg) LOG(msg)


#define length(type) ((sizeof(type)*5)/2 + 1)

static long memory_cnt = 0;
static int malloc_cnt = 0;
static int calloc_cnt = 0;
static int realloc_cnt = 0;
static int free_cnt = 0;

static unsigned history_length = 0;


void * (*ml_malloc)( uint32_t ) = NULL;
void (*ml_free)( void * )     = NULL;

int memleak_set_plat_malloc_free( void * (*malloc_func)( uint32_t ),
                              void (*free_func)( void * ) )
{
    ml_malloc = malloc_func;
    ml_free = free_func;
    return( 0 );
}


struct head
{
  void *addr;
  size_t size;
  char *file;
  unsigned long line;
  /* two addresses took the same space as an address and an integer on many archs => usable */
  union {
    struct { struct head *prev, *next; } list;
    struct { char *file; unsigned long line; } free;
  } in;
};

#define CHK_FREED 1
#define CHK_ALLOC 2
#define CHK_ALL (CHK_FREED | CHK_ALLOC)

int dbg_check_addr(char *msg, void *ptr, int opt,const char*file,int line);
void dbg_abort(char *msg);

static struct head *first = NULL, *last = NULL;
static struct head *hist_base = NULL, *histp = NULL;

#define HLEN sizeof(struct head)

static struct head *add(void *buf, size_t s,const char*file,int line)
{
  struct head *p;
  p = (struct head*)ml_malloc(HLEN);
  if(p)
    {
      p->addr = buf;
      p->size = s;
      p->file = ( char*)file;
      p->line = line;
      p->in.list.prev = last;
      p->in.list.next = NULL;
      if(last)
	last->in.list.next = p;
      else
	first = p;
      last = p;
      memory_cnt += s;
    }
  return p;
}

#define ADVANCE(p) {if(++(p) >= hist_base + history_length) (p) = hist_base;}

static void del(struct head *p,const char *fun,int line)
{
  struct head *prev, *next;
  prev = p->in.list.prev;
  next = p->in.list.next;
  if(prev)
    prev->in.list.next = next;
  else
    first = next;
  if(next)
    next->in.list.prev = prev;
  else
    last = prev;
  memory_cnt -= p->size;
  /* update history */
  if(history_length)
    {
      p->in.free.file = fun;
      p->in.free.line = line;
      memcpy(histp, p, HLEN);
      ADVANCE(histp);
    }
  ml_free(p);
}

static void replace(struct head *p, void *buf, size_t s,const char*file,int line)
{
  memory_cnt -= p->size;
  p->addr = buf;
  p->size = s;
  p->file = (char*)file;
  p->line = line;
  memory_cnt += s;
}

void *dbg_malloc(size_t s,const char*file,int line)
{
  void *buf;
  malloc_cnt++;
  buf = ml_malloc(s);
  if(buf)
    {
      if(add(buf, s,file,line))
	return buf;
      else
	ml_free(buf);
    }
  LOG( "%s:%d: dbg_malloc: not enough memory\r\n", file, line);
  return NULL;
}

void *dbg_calloc(size_t n, size_t s,const char*file,int line)
{
  void *buf;
  s *= n;
  calloc_cnt++;
  buf = ml_malloc(s);
  if(buf)
    {
      if(add(buf, s,file,line))
	{
	  /* standard calloc() sets memory to zero */
	  memset(buf, 0, s);
	  return buf;
	}
      else
	ml_free(buf);
    }
  LOG( "%s:%d: dbg_calloc: not enough memory\r\n", file, line);
  return NULL;
}

static struct head *find_in_heap(void *addr)
{
  struct head *p;
  /* start search from lately allocated blocks */ 
  for(p = last; p; p = p->in.list.prev)
    if(p->addr == addr) return p;
  return NULL;
}

static struct head *find_in_hist(void *addr)
{
  struct head *p;
  int cnt;
  if(history_length)
    {
      if(histp->addr)
	{
	  cnt = history_length;
	  p = histp;
	}
      else
	{
	  cnt = histp - hist_base;
	  p = hist_base;
	}
      while(cnt--)
	{
	  if(p->addr == addr) return p;
	  ADVANCE(p);
	}
    }
  return NULL;
}

void dbg_free(void *buf,const char*file,int line)
{
  struct head *p;
  free_cnt++;
  if(buf)
  {
    if((p = find_in_heap(buf)))
    {
        del(p,file,line);
        ml_free(buf);
    }
    else
    {
        LOGE("%s NOT FIND IN HEAP!!! %s %d \r\n",__FUNCTION__,file,line);
        dbg_check_addr("dbg_free", buf, CHK_FREED,file,line);
    }
	
  }
  else
  #ifdef ANDROID_JNI_PLAT
      LOGE("memleak", "***** %s:%d: dbg_free: NULL", file, line);
  #else 
      LOG( "%s:%d: dbg_free: NULL\r\n", file, line);
  #endif
}

void *dbg_realloc(void *buf, size_t s,const char*file,int line)
{
  struct head *p;
  realloc_cnt++;
  if(buf) /* when buf is NULL do malloc. counted twice as r and m */
    if(s)  /* when s = 0 realloc() acts like free(). counted twice: as r and f */
      if((p = find_in_heap(buf)))
	{
	  buf = realloc(buf, s);
	  if(buf)
	    {
	      replace(p, buf, s,file,line);
	      return buf;
	    }
	  else
	    LOG( "%s:%d: dbg_realloc: not enough memory\r\n",
		    file, line);
	}
      else
	dbg_check_addr("dbg_realloc", buf, CHK_FREED,file,line);
    else
      dbg_free(buf,file,line);
  else
    return dbg_malloc(s,file,line);
  return NULL;
}

int dbg_check_addr(char *msg, void *addr, int opt,const char*file,int line)
{
  struct head *p;
  if(opt & CHK_ALLOC)
    if((p = find_in_heap(addr)))
      {
	
 #ifdef ANDROID_JNI_PLAT
    LOGE("memleak", "***** %s:%d: %s: allocated (alloc: %s:%lu size: %lu)\n", file, line, msg, p->file, p->line, (unsigned long)p->size);
#else 
    LOG( "%s:%lu: %s: allocated (alloc: %s:%lu size: %lu)\r\n",
		file, line, msg, p->file, p->line, (unsigned long)p->size);
#endif
	return CHK_ALLOC;
      }
  if(opt & CHK_FREED)
    if((p = find_in_hist(addr)))
      {
  #ifdef ANDROID_JNI_PLAT
  LOGE("memleak", "***** %s:%d: %s: freed (alloc: %s:%lu size: %lu free: %s:%lu)", file, line, msg, p->file, p->line,
		(unsigned long)p->size, p->in.free.file, p->in.free.line);
  #else
	LOG( "%s:%lu: %s: freed (alloc: %s:%lu size: %lu free: %s:%lu)\r\n",
		file, line, msg, p->file, p->line,
		(unsigned long)p->size, p->in.free.file, p->in.free.line);
  #endif
	return CHK_FREED;
      }
  if(opt)
    #ifdef ANDROID_JNI_PLAT
     LOGE("memleak", "***** %s:%d: %s: unknown\n", file, line, msg);
    #else
    LOG( "%s:%lu: %s: unknown\r\n", file, line, msg);
    #endif
   
  return 0;
}

void dbg_mem_stat(void)
{
  LOG( "m: %d, c: %d, r: %d, f: %d, mem: %ld\r\n",
	  malloc_cnt, calloc_cnt, realloc_cnt, free_cnt, memory_cnt);
}

void dbg_heap_dump(char *key)
{
  char *buf;
  struct head *p;
#ifdef ANDROID_JNI_PLAT
    LOGE("memleak", "***** heap dump start\n");
#else 
      LOG( "***** heap dump start\r\n");
#endif
  
  p = first;
  while(p)
    {
      buf = (char*)ml_malloc(strlen(p->file) + 2*length(long) + 20);
#ifdef ANDROID_JNI_PLAT
    LOGE("memleak", "(alloc: %s:%lu size: %lu)\n", p->file, p->line, (unsigned long)p->size);
#else 
     sprintf(buf, "(alloc: %s:%lu size: %lu)\n", p->file, p->line, (unsigned long)p->size);
#endif
      
      p = p->in.list.next;
      if(strstr(buf, key)) 
      {
        //fputs(buf, stderr);
        LOG("%s\r\n",buf);
      }
      ml_free(buf);
    }
#ifdef ANDROID_JNI_PLAT
    LOGE("memleak", "heap dump end\r\n");
#else 
     LOG( "*****  heap dump end\r\n");
#endif
  
}

void dbg_history_dump(char *key)
{
  int cnt;
  char *buf;
  struct head *p;
  if(history_length)
    {
#ifdef ANDROID_JNI_PLAT
    LOGE("memleak","history dump start\n");
#else 
      LOG( "***** history dump start\r\n");
#endif
      if(histp->addr)
	{
	  cnt = history_length;
	  p = histp;
	}
      else
	{
	  cnt = histp - hist_base;
	  p = hist_base;
	}
      while(cnt--)
	{
	  buf = (char*)ml_malloc(strlen(p->file) + strlen(p->in.free.file) + 3*length(long) + 30);
#ifdef ANDROID_JNI_PLAT
    LOGE("memleak","(alloc: %s:%lu size: %lu free: %s:%lu)\n",
		  p->file, p->line, (unsigned long)p->size, p->in.free.file, p->in.free.line);
#else 
      sprintf(buf, "(alloc: %s:%lu size: %lu free: %s:%lu)\n",
		  p->file, p->line, (unsigned long)p->size, p->in.free.file, p->in.free.line);
#endif
	  
	  if(strstr(buf, key)) 
      {
        //fputs(buf, stderr);
        LOG("%s\r\n",buf);
      }
	  ml_free(buf);
	  ADVANCE(p);
	}
  #ifdef ANDROID_JNI_PLAT
    LOGE("memleak","history dump end\n");
#else 
       LOG( "***** history dump end\r\n");
#endif
     
    }
}     

void dbg_abort(char *msg)
{
  LOG( "+++++ %s: aborting.\n+++++ last call at\r\n", msg);
  dbg_mem_stat();
  dbg_heap_dump("");
  if(history_length) dbg_history_dump("");
  //abort();
}

void dbg_zero_stat(void)
{
  memory_cnt = 0;
  malloc_cnt = 0;
  calloc_cnt = 0;
  realloc_cnt = 0;
  free_cnt = 0;
}

static void sigsegv_handler(int signal)
{
  //if(signal == SIGSEGV)
  //    dbg_abort("catched SIGSEGV");
}

void dbg_catch_sigsegv(void)
{
  //signal(SIGSEGV, sigsegv_handler);
}

void dbg_init(int hist_len)
{
  history_length = hist_len;
  if(history_length)
    {
      histp = hist_base = (struct head*)ml_malloc(history_length*HLEN);
      if(hist_base == NULL) die("cannot allocate history buffer");
      else
        LOG("allocate history buffer OK!!\r\n");
    }
}

#ifdef WITH_DBG_STRDUP
/* Quick fix to support strdup() and strndup() calls.
   No checking of the pointers passed to these functions is done.
   Counted as m in the statistic.
*/

char *dbg_strndup(const char *str, size_t n)
{
  size_t len;
  char *buf;
  len = strlen(str);
  if(len > n) len = n;
  buf = dbg_malloc(len + 1);
  if(buf)
    {
      memcpy(buf, str, len);
      buf[len] = 0;
    }
  return buf;
}

char *dbg_strdup(const char *str)
{
  /* imposes a limit on the string length */
  return dbg_strndup(str, ULONG_MAX);
}
#endif

/*
  end
*/
#endif

