#include "Task.h"

uint8_t Task::task_count = 0;

Task::Task(uint8_t *packet, uint8_t length) : task_id(++Task::task_count), args_len(length){
	channel_id = packet[0] & 0x60;
	command_id = packet[0] & 0x1f;
	task_id = ++task_count; // use the global `task_count`
	// copy the remaining packet into args. No need to parse the args now.
	args = malloc(length - RPC_HEADER_SZ);
	#ifdef MEM_DEBUG
		if (args == NULL) ; // set some error flag here and abort
	#endif
	memcpy(args, packet + RPC_HEADER_SZ, length - RPC_HEADER_SZ);
}

Task::~Task(){
	free(args);
}