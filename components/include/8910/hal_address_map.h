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

static const halAddressRange_t gHwMap[] = {
    {0x00001000, 0x00100000},
    {0x00800000, 0x00880000},
    {0x00880000, 0x00882000},
    {0x08200000, 0x08208000},
    {0x08300000, 0x08400000},
    {0x08800000, 0x0881c000},
    {0x09000000, 0x091c0000},
    {0x10000000, 0x10080000},
    {0x14000000, 0x14004000},
    {0x14008000, 0x14010000},
    {0x14020000, 0x14060000},
    {0x14100000, 0x14100400},
    {0x14110000, 0x14116000},
    {0x15000000, 0x1500d000},
    {0x15100000, 0x15101000},
    {0x15102000, 0x15103000},
    {0x20000000, 0x20080000},
    {0x21400000, 0x21700000},
    {0x22000000, 0x22001000},
    {0x23000000, 0x23004000},
    {0x25000000, 0x25c00000},
    {0x26000000, 0x28000000},
    {0x40000000, 0x4001b000},
    {0x40020000, 0x40024000},
    {0x40025000, 0x40027000},
    {0x4002e000, 0x40030000},
    {0x40080000, 0x400e0000},
    {0x50000000, 0x50010000},
    {0x50020000, 0x5003d000},
    {0x50080000, 0x500b0000},
    {0x50100000, 0x5010f000},
    {0x50110000, 0x50111000},
    {0x50112000, 0x50113000},
    {0x5017F000, 0x50180000},
    {0x50300000, 0x50506000},
    {0x50510000, 0x5051a000},
    {0x60000000, 0x60000000 + CONFIG_FLASH_SIZE},
    {0x70000000, 0x80000000},
    {0x80000000, 0x80000000 + CONFIG_RAM_SIZE},
};
