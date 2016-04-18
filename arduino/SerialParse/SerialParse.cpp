#include "SerialParse.h"

void parseSerial(uint8_t *packet, uint8_t length, Task *task){
	task->channel_id = packet[0] & 0x60;
	task->command_id = packet[0] & 0x1f;
	task->task_id = ++task_count; // use the global `task_count`
	// copy the remaining packet into args. No need to parse the args now.
	memcpy(, packet+1, length-1)
}