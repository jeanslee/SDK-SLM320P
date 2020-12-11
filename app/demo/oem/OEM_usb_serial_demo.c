
//#include "mg_usb_serial.h"

#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'G', 'E', 'X')

#include "osi_api.h"
#include "osi_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oem.h"






void OEM_usb_serial_Thread(void *param)
{
	bool ret_b= MG_init_usb_serial();
	OSI_LOGI(0, "init_serial ret_b=%d",ret_b);
	unsigned char buff[11]={1,2,3,4,5,6,7,8,9,0,0};
	int num= MG_send_usb_serial(&buff[0],9);
	int n=0;
	for (;;)
	{
		MG_usb_serial_ext_echo("serial send=%d\r\n",n++); 	
		osiThreadSleep(500);	
		num=MG_recev_usb_serial(&buff[0],10);	
		OSI_LOGI(0, "serial recev num=%d,buff[0]=%d,buff[1]=%d,buff[2]=%d,buff[3]=%d",num,buff[0],buff[1],buff[2],buff[3]);
	}	
	MG_osiThreadExit();
}



























