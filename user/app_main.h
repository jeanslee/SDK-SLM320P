#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')
#define LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')
#include "osi_log.h"
#include "osi_api.h"
#include "osi_pipe.h"
#include "mg_os_api.h"
#include "../simid/inc/simid.h"

#include <string.h>
#include "lwip/inet.h"
#include "cfw.h"
#include "sockets.h"
#include "mg_sslsock_api.h"
#include "ty_mqtt_adpt.h"
#include "ty_http_adpt.h"
#include "ty_os_adpt.h"
#include "ty_sc_adpt.h"
#include "ty_socket_adpt.h"
#include "ty_ssl_adpt.h"
#include "ty_sock_test.h"
typedef enum
{
	TY_TASK_TEST = 1,
    TY_HTTPS_TEST = 2,
	TY_MQTT_TEST = 3,
	TY_SOCKET_TEST = 4,	
	TY_SSLSOCK_TEST = 5,	
	TY_MSGQUEUE_TEST = 6,
	TY_MUTEX_TEST = 7,
	TY_FILESYSTEM_TEST = 8,
	TY_SYSTEM_TIMER_TEST = 9,
	TY_SC_CHANNEL_TEST = 10,
	TY_LOCALTIME_TEST = 11,
	TY_MAIN_TASK_ENTRY = 12,
	TY_ESIM_PROFILE_ENABLE_TEST = 13,
	TY_LOCAL_TIME_TEST = 14

}OPTION;

int app_main_test(OPTION option);



#endif
