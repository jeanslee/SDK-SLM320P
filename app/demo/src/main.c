

#define LOG_TAG OSI_MAKE_LOG_TAG('B', 'L', 'N', 'W')

#include "osi_api.h"
#include "osi_log.h"
#include "cfw.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "sockets.h"
#include "osi_log.h"
#include "osi_api.h"


extern  osiThread_t * meig_Net_IND_Handle;
extern  osiThread_t * meig_TCP_Handle, *meig_baolan_lcd_Handle;

#include "oem.h"

int appimg_enter(void *param)
{
    MG_osiTracePrintf(LOG_TAG, "demo example enter");
    meig_baolan_lcd_Handle=MG_osiThreadCreateE("demo_lcd", OEM_LcdThread, NULL, OSI_PRIORITY_LOW, 1024,10);
    meig_Net_IND_Handle= MG_osiThreadCreateE("demo_net", OEM_NetThread, NULL, OSI_PRIORITY_NORMAL, 2*1024,10);
    meig_TCP_Handle=MG_osiThreadCreateE("demo_tcp_connect", OEM_sendTcpThread, NULL, OSI_PRIORITY_NORMAL, 8*1024,10);
    MG_osiThreadCreateE("demo_tcp_send", OEM_sendTcpThread, NULL, OSI_PRIORITY_NORMAL, 4*1024,10);	
    MG_osiThreadCreateE("demo_keypad", OEM_KeypadThread, NULL, OSI_PRIORITY_NORMAL, 1024,10);
    MG_osiThreadCreateE("demo_gpio", OEM_GpioThread, NULL, OSI_PRIORITY_NORMAL, 1024,10);
    MG_osiThreadCreateE("demo_record", OEM_RrcordThread, NULL, OSI_PRIORITY_NORMAL, 1024, 10);
    MG_osiThreadCreateE("demo_usb_serial", OEM_usb_serial_Thread, NULL, OSI_PRIORITY_NORMAL, 1024, 10);
    return 0;
}

void appimg_exit(void)
{
    MG_osiTracePrintf(LOG_TAG, "demo example exit");
}



