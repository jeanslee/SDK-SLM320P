#include "osi_log.h"

#include <errno.h>
#ifndef _MG_USB_SERIAL_API_H_
#define _MG_USB_SERIAL_API_H_

void MG_usb_serial_ext_echo(char* fmt, ...);

bool MG_init_usb_serial(void);

int MG_send_usb_serial(unsigned char* buff, int size);

int MG_recev_usb_serial(unsigned char* buff, int size);

#endif