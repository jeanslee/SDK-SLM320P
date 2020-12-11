#ifndef __QUEUE_TASK_H__
#define __QUEUE_TASK_H__

#include "ty_mqtt_basic_info.h"

#define TY_QUEUE_TASK_MSGQ_SIZE 10

void ty_queue_task_loop(void *args);

int ty_queue_task_put_msg(ty_queue_msg *msg);

#endif