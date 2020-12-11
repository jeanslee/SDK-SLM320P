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

#ifndef _HWREG_ACCESS_H_
#define _HWREG_ACCESS_H_

#include <stdint.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXP2(n) (1 << (n))
typedef uint32_t REG32;
typedef uint16_t REG16;
typedef uint8_t REG8;
typedef uint8_t UINT8;

#define REG_BIT(n) (1U << (n))
#define REG_BIT0 (1U << 0)
#define REG_BIT1 (1U << 1)
#define REG_BIT2 (1U << 2)
#define REG_BIT3 (1U << 3)
#define REG_BIT4 (1U << 4)
#define REG_BIT5 (1U << 5)
#define REG_BIT6 (1U << 6)
#define REG_BIT7 (1U << 7)
#define REG_BIT8 (1U << 8)
#define REG_BIT9 (1U << 9)
#define REG_BIT10 (1U << 10)
#define REG_BIT11 (1U << 11)
#define REG_BIT12 (1U << 12)
#define REG_BIT13 (1U << 13)
#define REG_BIT14 (1U << 14)
#define REG_BIT15 (1U << 15)
#define REG_BIT16 (1U << 16)
#define REG_BIT17 (1U << 17)
#define REG_BIT18 (1U << 18)
#define REG_BIT19 (1U << 19)
#define REG_BIT20 (1U << 20)
#define REG_BIT21 (1U << 21)
#define REG_BIT22 (1U << 22)
#define REG_BIT23 (1U << 23)
#define REG_BIT24 (1U << 24)
#define REG_BIT25 (1U << 25)
#define REG_BIT26 (1U << 26)
#define REG_BIT27 (1U << 27)
#define REG_BIT28 (1U << 28)
#define REG_BIT29 (1U << 29)
#define REG_BIT30 (1U << 30)
#define REG_BIT31 (1U << 31)

#ifdef CONFIG_SOC_8910
#define HAL_SYSIRQ_NUM(irq) ((irq) + 32)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#ifdef CONFIG_SOC_6760
#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#ifdef CONFIG_SOC_8811
#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#if defined(CONFIG_SOC_8955) || defined(CONFIG_SOC_8909)
#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ACCESS_ADDRESS(addr) ((addr) | 0xa0000000)
#endif

#ifdef __mips__
#define OSI_KSEG0(addr) (((unsigned long)(addr)&0x1fffffff) | 0x80000000)
#define OSI_KSEG1(addr) (((unsigned long)(addr)&0x1fffffff) | 0xa0000000)
#define OSI_IS_KSEG0(addr) (((unsigned long)(addr)&0xe0000000) == 0x80000000)
#define OSI_IS_KSEG1(addr) (((unsigned long)(addr)&0xe0000000) == 0xa0000000)
#define MEM_ACCESS_CACHED(addr) ((UINT32 *)OSI_KSEG0(addr))
#define MEM_ACCESS_UNCACHED(addr) ((UINT32 *)OSI_KSEG1(addr))
#endif

// Range in [start, end]
#define HWP_ADDRESS_RANGE(hwp) (uintptr_t)(hwp), (uintptr_t)(hwp) + sizeof(*(hwp)) - 1

// hardware registers are defined as union. The template of union is
//      union {
//          uint32_t v;
//          struct {
//              uint32_t f1 : 1;
//              uint32_t f2 : 2;
//              uint32_t f3 : 3;
//              ......
//          } b;
//      };
//
// The following macros are helpers for bit field operations:
// * REG_FIELD_GET: read the register, and return the value of specified field.
// * REG_WAIT_FIELD_EQ/NE/GT/GE/LT/LE/EQZ/NEZ: wait until the register fields
//   meet the condition.
// * REG_FIELD_MASK: return a mask (1 for the specified fields) of one or more
//   fields. In the above example, mask for f1 and f3 is 0x00000039
// * REG_FIELD_VALUE: return a value with specified fields, other fields are 0.
// * REG_FIELD_MASKVAL: return "mask, value"
// * REG_FIELD_CHANGE: change the specified fields, and other fields are
//   untouched. The changed value is returned
// * REG_FIELD_WRITE: write register with specified fields, and other fields
//   are 0, return the value to be written to register

#define REGTYPE_FIELD_GET(type, var, f1) ({ type _var = {(var)}; _var.b.f1; })
#define REGTYPE_FIELD_VAL(type, f1, v1) ({ type _var = {.b={.f1=(v1)}}; _var.v; })
#define REGTYPE_FIELD_MASK(type, f1) ({ type _var = {0xffffffff}; _var.b.f1 = 0; ~_var.v; })

#define REG_FIELD_GET(r, var, f) ({ var.v = r; var.b.f; })

#define REG_WAIT_FIELD_EQ(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_WAIT_FIELD_NE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_WAIT_FIELD_GT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_WAIT_FIELD_GE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_WAIT_FIELD_LT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_WAIT_FIELD_LE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_WAIT_FIELD_EQZ(var, reg, field) REG_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_WAIT_FIELD_NEZ(var, reg, field) REG_WAIT_FIELD_NE(var, reg, field, 0)

#define REG_WAIT_COND(cond) \
    do                      \
    {                       \
        while (!(cond))     \
            ;               \
    } while (0)

#define REG_FIELD_MASK1(var, f1) ({ var.v = 0xffffffff; var.b.f1 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK2(var, f1, f2) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK3(var, f1, f2, f3) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK4(var, f1, f2, f3, f4) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK5(var, f1, f2, f3, f4, f5) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.b.f6 = 0; var.v = ~var.v; var.v; })

#define REG_FIELD_VALUE1(var, f1, v1) ({ var.v = 0; var.b.f1 = v1; var.v; })
#define REG_FIELD_VALUE2(var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.v; })
#define REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.v; })
#define REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.v; })
#define REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.v; })
#define REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; var.v; })

#define REG_FIELD_MASKVAL1(var, f1, v1) REG_FIELD_MASK1(var, f1), REG_FIELD_VALUE1(var, f1, v1)
#define REG_FIELD_MASKVAL2(var, f1, v1, f2, v2) REG_FIELD_MASK2(var, f1, f2), REG_FIELD_VALUE2(var, f1, v1, f2, v2)
#define REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3) REG_FIELD_MASK3(var, f1, f2, f3), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3)
#define REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4) REG_FIELD_MASK4(var, f1, f2, f3, f4), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4)
#define REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) REG_FIELD_MASK5(var, f1, f2, f3, f4, f5), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5)
#define REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6)

#define REG_FIELD_CHANGE1(r, var, f1, v1) ({ var.v = r; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_CHANGE2(r, var, f1, v1, f2, v2) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_FIELD_WRITE1(r, var, f1, v1) ({ var.v = 0; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_WRITE2(r, var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_WRITE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_ADI_CHANGE1(r, var, f1, v1) halAdiBusChange(&(r), REG_FIELD_MASKVAL1(var, f1, v1))
#define REG_ADI_CHANGE2(r, var, f1, v1, f2, v2) halAdiBusChange(&(r), REG_FIELD_MASKVAL2(var, f1, v1, f2, v2))
#define REG_ADI_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) halAdiBusChange(&(r), REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) halAdiBusChange(&(r), REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) halAdiBusChange(&(r), REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) halAdiBusChange(&(r), REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WRITE1(r, var, f1, v1) halAdiBusWrite(&(r), REG_FIELD_VALUE1(var, f1, v1))
#define REG_ADI_WRITE2(r, var, f1, v1, f2, v2) halAdiBusWrite(&(r), REG_FIELD_VALUE2(var, f1, v1, f2, v2))
#define REG_ADI_WRITE3(r, var, f1, v1, f2, v2, f3, v3) halAdiBusWrite(&(r), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) halAdiBusWrite(&(r), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) halAdiBusWrite(&(r), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) halAdiBusWrite(&(r), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WAIT_FIELD_EQ(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_ADI_WAIT_FIELD_NE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_ADI_WAIT_FIELD_GT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_ADI_WAIT_FIELD_GE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_ADI_WAIT_FIELD_LT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_ADI_WAIT_FIELD_LE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_ADI_WAIT_FIELD_EQZ(var, reg, field) REG_ADI_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_ADI_WAIT_FIELD_NEZ(var, reg, field) REG_ADI_WAIT_FIELD_NE(var, reg, field, 0)

#define _REG_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                 \
    {                                                  \
        for (;;)                                       \
        {                                              \
            (var).v = (reg);                           \
            if (op((var).b.field, (expected)))         \
                break;                                 \
        }                                              \
    } while (0)
#define _REG_ADI_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                     \
    {                                                      \
        for (;;)                                           \
        {                                                  \
            (var).v = halAdiBusRead(&(reg));               \
            if (op((var).b.field, (expected)))             \
                break;                                     \
        }                                                  \
    } while (0)

#define _REG_WAIT_OP_EQ(a, b) ((a) == (b))
#define _REG_WAIT_OP_NE(a, b) ((a) != (b))
#define _REG_WAIT_OP_GT(a, b) ((a) > (b))
#define _REG_WAIT_OP_GE(a, b) ((a) >= (b))
#define _REG_WAIT_OP_LT(a, b) ((a) < (b))
#define _REG_WAIT_OP_LE(a, b) ((a) <= (b))

#ifdef __cplusplus
}
#endif
#endif
