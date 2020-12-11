#ifndef _OEM_API_H_
#define _OEM_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void OEM_NetThread(void *param);
void OEM_TcpThread(void *param);
void OEM_RrcordThread(void *param);
void OEM_KeypadThread(void *param);
void OEM_LcdThread(void *param);
void OEM_GpioThread(void *param);
void OEM_sendTcpThread(void *param);
void OEM_usb_serial_Thread(void *param);

#ifdef __cplusplus
}
#endif

#endif

