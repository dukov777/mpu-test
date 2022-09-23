#include "restricted_task_helper.h"

ProtectedTasks_t unprivileged_tasks_descriptors[10];

static ProtectedTasks_t* find_descriptor_by_memory_address(const void* address){
	for(uint8_t i = 0; i < 10; i++){
		if(address == NULL && unprivileged_tasks_descriptors[i].memory_start == NULL){
			return &unprivileged_tasks_descriptors[i];
		}

		if(unprivileged_tasks_descriptors[i].memory_start <= address
				&& address < unprivileged_tasks_descriptors[i].memory_start + (unprivileged_tasks_descriptors[i].stack_len*4)){
			return &unprivileged_tasks_descriptors[i];
		}
	}
	return NULL;
}

static ProtectedTasks_t* find_descriptor(ProtectedTasks_t* descriport){
	return find_descriptor_by_memory_address(descriport->memory_start);
}


static ProtectedTasks_t* get_free_unprivileged_tasks_descriptors(){
	ProtectedTasks_t descr = {.memory_start = NULL, .stack_len = 0};

	return find_descriptor(&descr);
}

static int release_unprivileged_tasks_descriptors(ProtectedTasks_t* descriport){
	ProtectedTasks_t* descr = find_descriptor(descriport);

	// invalidate the bank
	descr->memory_start = NULL;
	descr->stack_len = 0;

	return 0;
}

void init_restricted_tasks(void){
	for(uint8_t i = 0; i < 10; i++){
		unprivileged_tasks_descriptors[i].memory_start = 0;
	}
}

BaseType_t create_restricted_task(const TaskParameters_t * const pxTaskDefinition)
{
	ProtectedTasks_t* task_descriptor = get_free_unprivileged_tasks_descriptors();

	task_descriptor->memory_start = pxTaskDefinition->puxStackBuffer;
	task_descriptor->stack_len = pxTaskDefinition->usStackDepth;


	return xTaskCreateRestricted( pxTaskDefinition, &task_descriptor->task_handle);
}

BaseType_t kill_restricted_task(const void* task_stack)
{
	ProtectedTasks_t* task_descriptor = find_descriptor_by_memory_address(task_stack);
	if(task_descriptor) {
		vTaskDelete(task_descriptor->task_handle);
		release_unprivileged_tasks_descriptors(task_descriptor);
		return 0;
	}

	return -1;
}
