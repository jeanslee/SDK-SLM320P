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
#include "drv_uart.h"

drvUart_t *meig_uart;
osiMessageQueue_t *uart_Msg=NULL;
#define UART_RX_SIZE (512)
#define UART_TX_SIZE (512)
static void _testUartCB(void *param, uint32_t event)
{
	char rev_data[UART_RX_SIZE] = "";
	uint32_t rev_size,rev_len;
	bool msg_status=false;


    if (DRV_UART_EVENT_RX_ARRIVED == event)
    {
    	
		rev_size = drvUartReadAvail(meig_uart);
        rev_len = drvUartReceive(meig_uart, rev_data, rev_size);
		rev_data[rev_len]='\0';
		msg_status=MG_osiMessageQueueTryPut(uart_Msg,rev_data,0);		
		OSI_PRINTFI("********Meig uart read rx data size,%d,%s,%d",rev_size,rev_data,msg_status);
    }
    else if(DRV_UART_EVENT_RX_OVERFLOW == event)
    {
        OSI_LOGI(0,"uart overflow\n");
    }
	 else if(DRV_UART_EVENT_TX_COMPLETE == event)
    {
        OSI_LOGI(0,"uart send compele\n");
    }
    return;
	
}

static void prvVirtAtEntry(void *param)
{
	//uint8_t teset_data[] = "GNGSA,A,1,,,,,,,,,,,,,99.9,99.9,99.9,1*0A";
	char data[UART_RX_SIZE] = "";
	uint32_t rev_size,rev_len;
	bool msg_sts=0;	int i =0;
	meig_uart = MG_drvUartCreate(DRV_NAME_UART1,115200,DRV_UART_NO_PARITY,_testUartCB);
	uart_Msg=MG_osiMessageQueueCreate(3, 512);

	 if (uart_Msg == NULL || meig_uart == NULL || !MG_drvUartOpen(meig_uart))
    {
        MG_drvUartDestroy(meig_uart);
        meig_uart = NULL;
		MG_osiTracePrintf(LOG_TAG, "create the uart failed(%x,%x), exit",uart_Msg,meig_uart);
		MG_osiThreadExit();
		return;
    }
	 while(1)
	 	{
	 		msg_sts=MG_osiMessageQueueTryGet(uart_Msg,data,1024);
			rev_len = sizeof(data);
			if(msg_sts)
			{
				//for( i=0;i<128;i++){
				MG_osiTracePrintf(LOG_TAG,"Meig uart  customer get data --- %s ",data);
				//}
			}
			osiThreadSleep(200);
	 	}
	 
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
