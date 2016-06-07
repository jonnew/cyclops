#ifndef CL_TASKS_H
#define CL_TASKS_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"

/**
 * @brief      Task objects reside in the Process Queue, waiting to be popped and executed by the CPU.
 *             They are created *exclusively* from  RPC packets recieved from the Open Ephys GUI.
 *             
 * @attention  Not all RPC packets are transformed into a Task and placed on a Queue. Some RPC commands have an *immediate effect*, so to say.
 */
class Task{
 public:
    uint8_t taskID,
            channelID,
            commandID,
            argsLength;   /**< length of args array in "bytes" */
    void *args;           /**< pointer to arguments of this command */

    Task();

    /**
     * @brief      Creates a Task instance using an RPC packet.
     * @warning    There is no need to construct new Task objects. The Process_Queue Task elements should be modified using Task::set() instead.
     * 
     * @param[in]  packet  Passed to Task::set()
     * @param[in]  length  Passed to Task::set()
     */
    Task (uint8_t *packet, uint8_t length);

    /**
     * @brief
     * Parses the recieved "packet" which conforms to the Cyclops-RPC format into a Task instance.
     * @param      packet  Pointer to `uint8_t` array, this array lies in the Serial buffer. It's contents need to be copied into the `Task.args`
     * @param[in]  length  length in bytes of the packet
     */
    void set(uint8_t *packet, uint8_t length);

    /**
     * @brief      `free`s the `args` member.
     */
    ~Task();

 private:
    uint8_t _priority;         /**< Not needed now */
    static uint8_t _taskCount; /**< To create a different* `task_id` for all instances.
                                    @warning Will roll-over, and there is no guarantee of uniqueness! */
    /**
     * @brief      determines priority of this task using information in RPC_defs.h?
     */
    void computePriority();
};

#define QUEUE_CAPACITY 8

class Queue{
private:
    
    Task container[QUEUE_CAPACITY];
public:
    uint8_t head;
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
    uint8_t pushTask(uint8_t *packet, uint8_t length);

    /**
     * @return     Pointer to top-of-queue Task.
     */
    Task* peek();

    /**
     * @brief      Pop out top-of-queue.
     */
    void pop();
};

/*
class PriorityQueue{
};
*/

#endif