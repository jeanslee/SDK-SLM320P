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

static const uint32_t ramPhyPadCfg[] = {
    // PSRAM PHY
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_phy, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads0, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads1, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads0, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads0, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads0, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads1, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads1, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads1, 0x0000003b),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_reg_sel, 0x00000000),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_dqs_ie_sel_f0, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_dqs_oe_sel_f0, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_dqs_out_sel_f0, 0x00000004),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_dqs_gate_sel_f0, 0x0000000f),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_data_ie_sel_f0, 0x00000002),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg_data_oe_sel_f0, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_data_oe_ext, 0x00000011),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_dqs_oe_ext, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_format_control, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_rcd, 0x00000006),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_rddata_en, 0x00000006),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_rddata_late, 0x00000002),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_cph_wr, 0x00000006),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_cph_rd, 0x00000012),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_t_xphs, 0x0000000c),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_drf_cfg, 0x00000001),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_clock_gate, 0x0000001f),
    REG_APPLY_WRITE(&hwp_psramPhy->psram_rf_cfg_phy, 0x00000003),

    // PSRAM PAD
    REG_APPLY_WRITE(&hwp_analogReg->ddr_pad_cfg, 0),
    REG_APPLY_WRITE(&hwp_lpddrPhy->io_rf_ddr_pad_cfg, 0),
    REG_APPLY_WRITE(&hwp_lpddrPhy->io_rf_psram_drv_cfg, 0x420),
    REG_APPLY_WRITE(&hwp_lpddrPhy->io_rf_psram_pull_cfg, 0x551), // dqs/1, dq/0, dm/1, clkn/1, clk/1, cen/1
    REG_APPLY_WRITE(&hwp_lpddrPhy->io_rf_psram_pad_en_cfg, 1),   // pad_clk_en/1

    REG_APPLY_END,
};

static const uint32_t ramDmcCfg[] = {
    REG_APPLY_WRITE(&hwp_dmcCtrl->format_control, 0x11000101), // 32x16, BURST_2_DDR_BL4, ACC_GRANU_2_DDR_4N, ALIGN_BOUNDARY_2_COL_2BIT
    REG_APPLY_WRITE(&hwp_dmcCtrl->address_control, 0x20201),   // 9_COL_BITS, 13_ROW_BITS, 2_BANK_BITS_4BK, 0_CHIP_BITS_1CS, 0_CHANNEL_BITS_1MEMIF
    REG_APPLY_WRITE(&hwp_dmcCtrl->decode_control, 0x20),       // CHANNEL_CHIP_ROW_BANK_COL, PAGE_ADDR_11_10
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_refi, 0x40),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rfc, 0x40004),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_mrr, 0x00000002),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_mrw, 0x00000005),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rcd, 0x00000006),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_ras, 0x00000011),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rp, 0x4),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rpall, 0x4),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rrd, 0x00000004),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_faw, 0x00000014),
    REG_APPLY_WRITE(&hwp_dmcCtrl->read_latency, 0x0000000e),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rtr, 0x00000006),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rtw, 0x0000000c),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rtp, 0x00000008),
    REG_APPLY_WRITE(&hwp_dmcCtrl->write_latency, 0x0000000a),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_wr, 0x0000000c),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_wtr, 0x000a000a),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_wtw, 0x000c000c),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_eckd, 0x0000000b),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_xckd, 0x0000000b),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_ep, 0x00000002),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_xp, 0x00030003),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_esr, 0x00000002),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_xsr, 0x40004),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_srckd, 0x0000000b),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_cksrd, 0x0000000b),
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_rddata_en, 0x00000006), // RL=6
    REG_APPLY_WRITE(&hwp_dmcCtrl->t_phywrlat, 0x00000103),

    // REG_APPLY_WRITE(&hwp_sysCtrl->cfg_qos_wcn_a5_gge, 0x6331800),     // a5/3, gge/6
    // REG_APPLY_WRITE(&hwp_sysCtrl->cfg_qos_axidma_cpa5_f8, 0xa521021), // axidma/1, cpa5/4, f8/5
    // REG_APPLY_WRITE(&hwp_sysCtrl->cfg_qos_lcdc_lzma_gouda, 0),
    // REG_APPLY_WRITE(&hwp_sysCtrl->cfg_qos_lte_usb, 0x108e7), // lte/7, usb/2

    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos0_control, 0xf00),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos1_control, 0xf01),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos2_control, 0xf02),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos3_control, 0xf03),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos4_control, 0xf04),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos5_control, 0xf05),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos6_control, 0xf06),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos7_control, 0xf07),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos8_control, 0xf08),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos9_control, 0xf09),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos10_control, 0xf0a),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos11_control, 0xf0b),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos12_control, 0xf0c),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos13_control, 0xf0d),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos14_control, 0xf0e),
    // REG_APPLY_WRITE(&hwp_dmcCtrl->qos15_control, 0xf0f),
    // hwp_dmcCtrl->turnaround_priority, 0xf3,
    // hwp_dmcCtrl->hit_priority, 0xf3,

    REG_APPLY_END,
};

static const uint32_t ramDmcBootCfg[] = {
    REG_APPLY_UDELAY(200),                                 // t_PU >= 150 us
    REG_APPLY_WRITE(&hwp_dmcCtrl->direct_cmd, 0x0),        // NOPE
    REG_APPLY_UDELAY(10),                                  // wait dmc toggle dfi clk
    REG_APPLY_WRITE(&hwp_dmcCtrl->direct_cmd, 0x1000003f), // MRS, MR63, reset
    REG_APPLY_UDELAY(10),                                  // t_RST >= 2 us
    REG_APPLY_UDELAY(1000),                                // t_HSPU - tPU - tRST >= 1 ms
    REG_APPLY_WRITE(&hwp_dmcCtrl->direct_cmd, 0x10001b00),
    REG_APPLY_UDELAY(10),
    REG_APPLY_WRITE(&hwp_dmcCtrl->direct_cmd, 0x10008004), // MRS, MR4, WL=2, PSRAM-200M
    REG_APPLY_UDELAY(10),

    REG_APPLY_WRITE(&hwp_dmcCtrl->memc_cmd, 3), // GO
    REG_APPLY_UDELAY(10),

    REG_APPLY_END,
};

static const uint32_t ramDmcWakeCfg[] = {
    REG_APPLY_WRITE(&hwp_pwrctrl->ddr_hold_ctrl, 0),
    REG_APPLY_WRITE(&hwp_idle->ddr_latch, 0),

    REG_APPLY_WRITE(&hwp_dmcCtrl->memc_cmd, 3), // GO
    REG_APPLY_UDELAY(10),

    REG_APPLY_END,
};
