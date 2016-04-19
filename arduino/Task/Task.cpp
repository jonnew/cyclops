#include "Task.h"

uint8_t Task::task_count = 0;

Task::Task(uint8_t *packet, uint8_t length) : task_id(++Task::task_count), args_len(length){
	channel_id = EXTRACT_CHANNEL(packet[0]);
	command_id = EXTRACT_COMMAND(packet[0]);
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