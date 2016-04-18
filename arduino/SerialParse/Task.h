/*! @file Task.h
    @brief Declares the Task struct/class.
           This file is part of CL.
    
    @author Ananya Bahadur
*/
#ifndef CL_TASKS_H
#define CL_TASKS_H

// global variable, can be accessed by anywhere in the program. Used to determine task_id.
// Is this critical?
extern uint8_t task_count;

typedef struct{
	uint8_t task_id,
	        channel_id,
	        command_id,
			args_len;   // length of args array in "bytes"
	void *args;
} Task;

#endif