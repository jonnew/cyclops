#include "Task.h"

uint8_t Task::_taskCount = 0;

Task::Task(){}

Task::Task(uint8_t *packet, uint8_t length){
	Task::set(packet, length);
}

void Task::set(uint8_t *packet, uint8_t length){
	taskID     = ++Task::_taskCount;
	argsLength = length;
	channelID  = EXTRACT_CHANNEL(packet[0]);
	commandID  = EXTRACT_COMMAND(packet[0]);
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


Queue::Queue() : size(0), head(0) {}

uint8_t Queue::pushTask(uint8_t *packet, uint8_t length){
	if (size < QUEUE_CAPACITY){
		uint8_t insert = head + size;
		if (insert >= QUEUE_CAPACITY)
			insert -= QUEUE_CAPACITY;
		container[insert].set(packet, length);
		size++;
		return 0;
	}
	return 1;
}

Task* Queue::peek(){
	return &container[head];
}

void Queue::pop(){
	head = (head == QUEUE_CAPACITY - 1)? 0 : head+1;
	size--;
}