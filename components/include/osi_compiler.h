/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _OSI_COMPILER_H_
#define _OSI_COMPILER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// macros for alias, either strong or weak. It can't work with LTO.
#define OSI_STRONG_ALIAS(alias, sym) __asm(".global " #alias "\n" #alias " = " #sym)
#define OSI_WEAK_ALIAS(alias, sym) __asm(".weak " #alias "\n" #alias " = " #sym)

// macros for alias, either strong or weak,
#define OSI_DECL_STRONG_ALIAS(oldname, newfunc) newfunc __attribute__((alias(#oldname)))
#define OSI_DECL_WEAK_ALIAS(oldname, newfunc) newfunc __attribute__((weak, alias(#oldname)))

#define OSI_STRINGFY(s) _OSI_STRINGFY(s)
#define _OSI_STRINGFY(s) #s

// macro for nop instruction
#define OSI_NOP asm volatile("nop")

// macro for compiler memory access barrier
#define OSI_BARRIER() asm volatile("" :: \
                                       : "memory")

// macro for alignment attribute
#define OSI_CACHE_LINE_ALIGNED __attribute__((aligned(CONFIG_CACHE_LINE_SIZE)))
#define OSI_ALIGNED(n) __attribute__((aligned(n)))

#define OSI_ATTRIBUTE_ISR __attribute__((interrupt))
#define OSI_ATTRIBUTE_USED __attribute__((used))
#define OSI_ATTRIBUTE_OPTIMIZE(n) __attribute__((optimize(n)))

// macros for "known" sections
#define OSI_SECTION(sect) __attribute__((section(#sect)))
#define OSI_SECTION_SRAM_BOOT_TEXT OSI_SECTION(.sramboottext)
#define OSI_SECTION_SRAM_TEXT OSI_SECTION(.sramtext)
#define OSI_SECTION_SRAM_DATA OSI_SECTION(.sramdata)
#define OSI_SECTION_SRAM_BSS OSI_SECTION(.srambss)
#define OSI_SECTION_SRAM_UNINIT OSI_SECTION(.sramuninit)
#define OSI_SECTION_SRAM_UC_DATA OSI_SECTION(.sramucdata)
#define OSI_SECTION_SRAM_UC_BSS OSI_SECTION(.sramucbss)
#define OSI_SECTION_SRAM_UC_UNINIT OSI_SECTION(.sramucuninit)
#define OSI_SECTION_RAM_TEXT OSI_SECTION(.ramtext)
#define OSI_SECTION_RAM_DATA OSI_SECTION(.ramdata)
#define OSI_SECTION_RAM_BSS OSI_SECTION(.rambss)
#define OSI_SECTION_RAM_UNINIT OSI_SECTION(.ramuninit)
#define OSI_SECTION_RAM_UC_DATA OSI_SECTION(.ramucdata)
#define OSI_SECTION_RAM_UC_BSS OSI_SECTION(.ramucbss)
#define OSI_SECTION_RAM_UC_UNINIT OSI_SECTION(.ramucuninit)
#define OSI_SECTION_BOOT_TEXT OSI_SECTION(.boottext)
#define OSI_SECTION_RO_KEEP __attribute__((used, section(".rokeep")))
#define OSI_SECTION_RW_KEEP __attribute__((used, section(".rwkeep")))

// macros for attributes
#define OSI_WEAK __attribute__((weak))
#define OSI_USED __attribute__((used))
#define OSI_UNUSED __attribute__((unused))
#define OSI_NO_RETURN __attribute__((__noreturn__))
#define OSI_NO_INLINE __attribute__((noinline))
#define OSI_FORCE_INLINE __attribute__((always_inline)) inline

#if __mips__
#define OSI_NO_MIPS16 __attribute__((nomips16))
#define OSI_NAKED
#endif

#if __arm__
#define OSI_NO_MIPS16
#define OSI_NAKED __attribute__((naked))
#define OSI_DMB() asm volatile("dmb 0xf" :: \
                                   : "memory")
#define OSI_DSB() asm volatile("dsb 0xf" :: \
                                   : "memory")
#define OSI_ISB() asm volatile("isb 0xf" :: \
                                   : "memory")
#endif

// macro maybe helpful for compiler optimization
#define OSI_LIKELY(x) __builtin_expect(!!(x), 1)
#define OSI_UNLIKELY(x) __builtin_expect(!!(x), 0)

// macros for MIPS KSEG0/1
#if __mips__
#define OSI_KSEG0(addr) (((unsigned long)(addr)&0x1fffffff) | 0x80000000)
#define OSI_KSEG1(addr) (((unsigned long)(addr)&0x1fffffff) | 0xa0000000)
#define OSI_IS_KSEG0(addr) (((unsigned long)(addr)&0xe0000000) == 0x80000000)
#define OSI_IS_KSEG1(addr) (((unsigned long)(addr)&0xe0000000) == 0xa0000000)
#define OSI_KSEG01_PHY_ADDR(addr) ((unsigned long)(addr)&0x0FFFFFFF)
#endif

typedef struct
{
    uint32_t data[16 / 4];
} osiBits128_t, osiBytes16_t;

typedef struct
{
    uint32_t data[32 / 4];
} osiBits256_t, osiBytes32_t;

typedef struct
{
    uint32_t data[64 / 4];
} osiBits512_t, osiBytes64_t;

typedef struct
{
    uint32_t data[80 / 4];
} osiBits640_t, osiBytes80_t;

typedef struct
{
    uint32_t data[512 / 4];
} osiBits4096_t, osiBytes512_t;

/**
 * \brief tiny helper for buffer pointer and size
 */
typedef struct
{
    uintptr_t ptr; ///< buffer pointer
    unsigned size; ///< buffer size
} osiBuffer_t;

/**
 * \brief contiguous buffer, with size as first word
 */
typedef struct
{
    uint32_t size;    ///< size the following data
    uint32_t data[0]; ///< data
} osiBufSize32_t;

/**
 * \brief contiguous buffer, with size as first short
 */
typedef struct
{
    uint16_t size;    ///< size the following data
    uint16_t data[0]; ///< data
} osiBufSize16_t;

/**
 * \brief contiguous buffer, with size as first byte
 */
typedef struct
{
    uint8_t size;    ///< size the following data
    uint8_t data[0]; ///< data
} osiBufSize8_t;

/**
 * data structure to define an unsigned integer range
 */
typedef struct
{
    uint32_t minval; ///< minimal value
    uint32_t maxval; ///< maximum value
} osiUintRange_t;

/**
 * data structure to define a signed integer range
 */
typedef struct
{
    int minval; ///< minimal value
    int maxval; ///< maximum value
} osiIntRange_t;

/**
 * data structure to define a unsigned 64bits integer range
 */
typedef struct
{
    uint64_t minval; ///< minimal value
    uint64_t maxval; ///< maximum value
} osiUint64Range_t;

/**
 * data structure to define a signed 64bits integer range
 */
typedef struct
{
    int64_t minval; ///< minimal value
    int64_t maxval; ///< maximum value
} osiInt64Range_t;

// do { ... } while (0) is common trick to avoid if/else error
#define OSI_DO_WHILE0(expr) \
    do                      \
    {                       \
        expr                \
    } while (0)

// just a dead loop
#define OSI_DEAD_LOOP OSI_DO_WHILE0(for (;;);)

// Busy loop wait until condition is true
#define OSI_LOOP_WAIT(cond) OSI_DO_WHILE0(while (!(cond));)

// Busy loop wait util condition is true. When polling peripherals, it is
// needed to avoid read peripheral registers without delay, especially
// when the peripheral is connected to a slow bus. This may cause the bus
// is busy to react CPU register read, and other operations are affected.
#define OSI_POLL_WAIT(cond) OSI_DO_WHILE0(while (!(cond)) { OSI_NOP; OSI_NOP; OSI_NOP; OSI_NOP; })

// Loop wait. Return true if cont_true is meet, return false if cond_false is meet.
#define OSI_LOOP_WAIT_IF(cond_true, cond_false) \
    ({                                          \
        bool _waited;                           \
        for (;;)                                \
        {                                       \
            if (cond_true)                      \
            {                                   \
                _waited = true;                 \
                break;                          \
            }                                   \
            if (cond_false)                     \
            {                                   \
                _waited = false;                \
                break;                          \
            }                                   \
        }                                       \
        _waited;                                \
    })

// Loop wait with timeout
#define OSI_LOOP_WAIT_TIMEOUT_US(cond, us)            \
    ({                                                \
        bool _waited = false;                         \
        unsigned _us = (us);                          \
        osiElapsedTimer_t _timeout;                   \
        osiElapsedTimerStart(&_timeout);              \
        do                                            \
        {                                             \
            if (cond)                                 \
            {                                         \
                _waited = true;                       \
                break;                                \
            }                                         \
        } while (osiElapsedTimeUS(&_timeout) <= _us); \
        _waited;                                      \
    })

// Loop wait with timeout, also post will be executed for each loop
#define OSI_LOOP_WAIT_POST_TIMEOUT_US(cond, us, post) \
    ({                                                \
        bool _waited = false;                         \
        unsigned _us = (us);                          \
        osiElapsedTimer_t _timeout;                   \
        osiElapsedTimerStart(&_timeout);              \
        for (;;)                                      \
        {                                             \
            if (cond)                                 \
            {                                         \
                _waited = true;                       \
                break;                                \
            }                                         \
            if (osiElapsedTimeUS(&_timeout) > _us)    \
            {                                         \
                _waited = false;                      \
                break;                                \
            }                                         \
            (void)(post);                             \
        }                                             \
        _waited;                                      \
    })

// macro for fourcc tag
#define OSI_MAKE_TAG(a, b, c, d) ((unsigned)(a) | ((unsigned)(b) << 8) | ((unsigned)(c) << 16) | ((unsigned)(d) << 24))

// macro for array dimension
#define OSI_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// macros for 2^n, and 2^n alignment
#define OSI_IS_POW2(v) (((v) & ((v)-1)) == 0)
#define OSI_IS_ALIGNED(v, n) (((unsigned long)(v) & ((n)-1)) == 0)
#define OSI_ALIGN_UP(v, n) (((unsigned long)(v) + (n)-1) & ~((n)-1))
#define OSI_ALIGN_DOWN(v, n) ((unsigned long)(v) & ~((n)-1))
#define OSI_DIV_ROUND(m, n) (((m) + ((n) >> 1)) / (n))
#define OSI_DIV_ROUND_UP(n, m) (((n) + (m)-1) / (m))

// macro for compare two chars ignoring case
#define OSI_CHAR_CASE_EQU(a, b) (((a) | 0x20) == ((b) | 0x20))

// macro to increase the pointer, and return the original pointer
#define OSI_PTR_INCR_POST(p, n) ({uintptr_t _orig = (p); (p) += (n); _orig; })

// pointer (signed) diff, either can be any pointer type
#define OSI_PTR_DIFF(a, b) ((intptr_t)(a) - (intptr_t)(b))

// Macro for variadic argument count
#define OSI_VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...) N
#define OSI_VA_NARGS(...) OSI_VA_NARGS_IMPL(0, ##__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// macros for declaration and definition
#define OSI_DEF_CONST_VAR(decl, ...) static const decl = __VA_ARGS__
#define OSI_DEF_GLOBAL_VAR(decl, ...) decl = __VA_ARGS__
#define OSI_DECL_GLOBAL_VAR(decl, ...) extern decl

// macros to convert CPU endian to little/big endian
#define OSI_TO_LE16(v) (v)
#define OSI_TO_LE32(v) (v)
#define OSI_TO_BE16(v) __builtin_bswap16(v)
#define OSI_TO_BE32(v) __builtin_bswap32(v)

// macros to convert CPU endian from little/big endian
#define OSI_FROM_LE16(v) (v)
#define OSI_FROM_LE32(v) (v)
#define OSI_FROM_BE16(v) __builtin_bswap16(v)
#define OSI_FROM_BE32(v) __builtin_bswap32(v)

// macro for 32bits/16bits/8bits register read and write
#define OSI_REG32_WRITE(address, value) *(volatile uint32_t *)(address) = (value)
#define OSI_REG32_READ(address) (*(volatile uint32_t *)(address))
#define OSI_REG16_WRITE(address, value) *(volatile uint16_t *)(address) = (value)
#define OSI_REG16_READ(address) (*(volatile uint16_t *)(address))
#define OSI_REG8_WRITE(address, value) *(volatile uint8_t *)(address) = (value)
#define OSI_REG8_READ(address) (*(volatile uint8_t *)(address))

// macros for easier writing
#define OSI_KB(n) ((unsigned)(n) * (unsigned)(1024))
#define OSI_MB(n) ((unsigned)(n) * (unsigned)(1024 * 1024))
#define OSI_GB(n) ((unsigned)(n) * (unsigned)(1024 * 1024 * 1024))
#define OSI_MHZ(n) ((unsigned)(n) * (unsigned)(1000 * 1000))

// macros for min, max. the variable will be accessed only once
#define OSI_MIN(type, a, b) ({ type _a = (type)(a); type _b = (type)(b); _a < _b? _a : _b; })
#define OSI_MAX(type, a, b) ({ type _a = (type)(a); type _b = (type)(b); _a > _b? _a : _b; })

// Range [start, end) and region [start, start+size) macros
#define OSI_IS_IN_RANGE(type, a, start, end) ({type _a = (type)(a); type _start = (type)(start); type _end = (type)(end); _a >= _start && _a < _end; })
#define OSI_IS_IN_REGION(type, a, start, size) ({type _a = (type)(a); type _start = (type)(start); type _end = _start + (type)(size); _a >= _start && _a < _end; })
#define OSI_RANGE_INSIDE(type, start1, end1, start2, end2) ({type _s1 = (type)(start1), _e1 = (type)(end1), _s2 = (type)(start2), _e2 = (type)(end2);  _s1 >= _s2 && _e1 <= _e2; })
#define OSI_REGION_INSIDE(type, start1, size1, start2, size2) ({type _s1 = (type)(start1), _e1 = _s1 + (type)(size1), _s2 = (type)(start2), _e2 = _s2 + (type)(size2);  _s1 >= _s2 && _e1 <= _e2; })

// macro to swap 2 variables
#define OSI_SWAP(type, a, b) ({ type _t = (a); (a) = (b); (b) = _t; })

// macro for offsetof and container_of
#define OSI_OFFSETOF(type, member) __builtin_offsetof(type, member)
#define OSI_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr)-OSI_OFFSETOF(type, member)))

// assert not enabled by default, and shall not configured globally
#if defined(OSI_LOCAL_DEBUG_ASSERT_ENABLED) && !defined(OSI_DEBUG_ASSERT_DISABLED)
#define OSI_DEBUG_ASSERT(expr) OSI_DO_WHILE0(if (!(expr)) osiPanic();)
#else
#define OSI_DEBUG_ASSERT(expr)
#endif

#ifdef __cplusplus
#define OSI_EXTERN_C extern "C"
#define OSI_EXTERN_C_BEGIN extern "C" {
#define OSI_EXTERN_C_END }
#else
#define OSI_EXTERN_C
#define OSI_EXTERN_C_BEGIN
#define OSI_EXTERN_C_END
#endif

// macro for load section, symbol naming style matches linker script
#define OSI_LOAD_SECTION(name)                 \
    do                                         \
    {                                          \
        extern uint32_t __##name##_start;      \
        extern uint32_t __##name##_end;        \
        extern uint32_t __##name##_load_start; \
        uint32_t *p;                           \
        uint32_t *l;                           \
        for (p = &__##name##_start,            \
            l = &__##name##_load_start;        \
             p < &__##name##_end;)             \
            *p++ = *l++;                       \
    } while (0)

// macro for clear section, symbol naming style matches linker script
#define OSI_CLEAR_SECTION(name)           \
    do                                    \
    {                                     \
        extern uint32_t __##name##_start; \
        extern uint32_t __##name##_end;   \
        uint32_t *p;                      \
        for (p = &__##name##_start;       \
             p < &__##name##_end;)        \
            *p++ = 0;                     \
    } while (0)

// macros for "known" sections
#define OSI_SECTION_SRAM_BOOT_TEXT OSI_SECTION(.sramboottext)
#define OSI_SECTION_SRAM_TEXT OSI_SECTION(.sramtext)
#define OSI_SECTION_SRAM_DATA OSI_SECTION(.sramdata)
#define OSI_SECTION_SRAM_BSS OSI_SECTION(.srambss)
#define OSI_SECTION_SRAM_UNINIT OSI_SECTION(.sramuninit)
#define OSI_SECTION_SRAM_UC_DATA OSI_SECTION(.sramucdata)
#define OSI_SECTION_SRAM_UC_BSS OSI_SECTION(.sramucbss)
#define OSI_SECTION_SRAM_UC_UNINIT OSI_SECTION(.sramucuninit)
#define OSI_SECTION_RAM_TEXT OSI_SECTION(.ramtext)
#define OSI_SECTION_RAM_DATA OSI_SECTION(.ramdata)
#define OSI_SECTION_RAM_BSS OSI_SECTION(.rambss)
#define OSI_SECTION_RAM_UNINIT OSI_SECTION(.ramuninit)
#define OSI_SECTION_RAM_UC_DATA OSI_SECTION(.ramucdata)
#define OSI_SECTION_RAM_UC_BSS OSI_SECTION(.ramucbss)
#define OSI_SECTION_RAM_UC_UNINIT OSI_SECTION(.ramucuninit)
#define OSI_SECTION_BOOT_TEXT OSI_SECTION(.boottext)
#define OSI_SECTION_RO_KEEP __attribute__((used, section(".rokeep")))
#define OSI_SECTION_RW_KEEP __attribute__((used, section(".rwkeep")))

#define OSI_SECTION_LINE(s) __attribute__((section(OSI_STRINGFY(s) "." OSI_STRINGFY(__LINE__))))

#endif
