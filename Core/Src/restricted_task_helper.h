#ifndef RESTRICTED_TASK_HELPER_H
#define RESTRICTED_TASK_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
	TaskHandle_t task_handle;
	void *memory_start;
	size_t stack_len;
} ProtectedTasks_t;


void init_restricted_tasks(void);

BaseType_t create_restricted_task(const TaskParameters_t * const pxTaskDefinition);

BaseType_t kill_restricted_task(const void* task_stack);


#ifdef __cplusplus
}
#endif

#endif /* RESTRICTED_TASK_HELPER_H */


