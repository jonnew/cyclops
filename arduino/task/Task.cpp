#include "Task.h"

uint8_t Task::_taskCount = 0;

Task::Task(uint8_t *packet, uint8_t length) : taskID(++Task::_taskCount), argsLength(length){
	channelID = EXTRACT_CHANNEL(packet[0]);
	commandID = EXTRACT_COMMAND(packet[0]);
	// copy the remaining packet into args. No need to parse the args now.
	// 
	// TODO: 
	// There should be a way to evade this usage of heap.
	args = malloc(length - RPC_HEADER_SZ);
	#ifdef MEM_DEBUG
		if (args == NULL) ; // set some error flag here and abort
	#endif
	memcpy(args, packet + RPC_HEADER_SZ, length - RPC_HEADER_SZ);
}

Task::~Task(){
	free(args);
}