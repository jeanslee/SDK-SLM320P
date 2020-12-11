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

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"
#include "osi_pipe.h"
#include <string.h>
#include "drv_names.h"
#include "hal_chip.h"
#include "at_engine.h"

static void prvVirtAtEntry(void *param)
{
	uint8_t teset_data[] = "1234";
	uint8_t rev_data[64] = "";
	uint32_t rev_size;
	atDevice_t *meig_uart = MG_atDeviceUsbCreate(DRV_NAME_USRL_COM1);

	 if (meig_uart == NULL || !MG_atDeviceOpen(meig_uart))
    {
        MG_atDeviceDelete(meig_uart);
        meig_uart = NULL;
		MG_osiTracePrintf(LOG_TAG, "create the at uart failed, exit");
		MG_osiThreadExit();
		return;
    }

	 MG_atDeviceWrite(&meig_uart,&teset_data,sizeof(teset_data));
	 
	 MG_osiTracePrintf(LOG_TAG, "at uart write data ");

	 rev_size=MG_atDeviceRead(&meig_uart, &rev_data,sizeof(rev_data));

	 MG_osiTracePrintf(LOG_TAG, "at uart read data rev_data=%d",rev_data);
	 
	 MG_osiThreadExit();

}

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "application image enter, param 0x%x", param);
    osiThreadCreate("uart", prvVirtAtEntry, NULL, OSI_PRIORITY_NORMAL, 1024, 4);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "application image exit");
}
