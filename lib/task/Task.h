/**
 * @file Task.h
 * 
 * @author Ananya Bahadur
 */

#ifndef CL_TASKS_H
#define CL_TASKS_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"
#include "Waveform_t.h"

/**
 * @brief      Task objects reside in the Process Queue, waiting to be popped and executed by the CPU.
 *             They are created *exclusively* from  RPC packets recieved from the Open Ephys GUI.
 *             
 * @attention
 * Not all RPC packets are transformed into a Task and placed on a Queue.
 * Some RPC commands have an *immediate effect*, so to say.
 */
class Task{
 public:
    uint8_t taskID,
            channelID,
            commandID,
            argsLength;          /**< length of args array in "bytes" */
    char    args[RPC_MAX_ARGS];  /**< Space for arguments of any command */

    Task();

    /**
     * @brief      Sets the args
     *
     * @param[in]  arg_len  The argument length
     */
    void setArgs(uint8_t arg_len);

    /**
     * @brief      Executes the Task.
     *
     * @return     ``0`` if successful, else ``1``
     */
    uint8_t compute();

 private:
    uint8_t _priority;         /**< Not needed now */
    static uint8_t _taskCount; /**< To create a different* `task_id` for all instances.
                                    @warning Will roll-over, and there is no guarantee of uniqueness! */
    /**
     * @brief      determines priority of this task using information in RPC_defs.h?
     */
    void computePriority();
};

#define QUEUE_CAPACITY 8 /**< Hard coded Queue container Size */

/**
 * @brief      This is the (_circular_) RPC Task Queue.
 * @details
 * The RPC lib will pushTask() on the instance and Cyclops main-loop would pop-
 * and-service the incoming Tasks.
 */
class Queue{
private:
    Task container[QUEUE_CAPACITY];
public:
    uint8_t head;

    /**
     * @brief      Creates an empty Queue of capacity QUEUE_CAPACITY
     */
    Queue();

    uint8_t size; /**< No. of elements in Queue. */
    /**
     * @brief
     * Creates a new task using the RPC packet and pushes it at tail of Queue.
     * 
     * @param[in]  packet  Pointer to `uint8_t` array, this array lies in the Serial buffer. Passed to Task::set().
     * @param[in]  length  length in bytes of the packet
     *
     * @return     0 if success else 1.
     */
    uint8_t pushTask(uint8_t packet, uint8_t length);

    /**
     * @return     Pointer to top-of-queue Task.
     */
    Task* peek();

    /**
     * @brief      Pop out top-of-queue.
     */
    void pop();
};

void readSerial(Queue *q);

/*
class PriorityQueue{
};
*/

#endif