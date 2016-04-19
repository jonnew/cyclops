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
    static uint8_t task_count; /**< To create a different* `task_id` for all instances.
                                    @warning Will roll-over, and there is no guarantee of uniqueness! */
    uint8_t task_id,
            channel_id,
            command_id,
            args_len;   /**< length of args array in "bytes" */
    void *args;         /**< pointer to arguments of this command */

    /**
     * @brief      Parses the recieved "packet" which conforms to the Cyclops-RPC format into a Task instance.
     *
     * @param[in]  packet  Pointer to `uint8_t` array, this array lies in the Serial buffer. It's contents need to be copied into the `Task.args`
     * @param[in]  length  length in bytes of the packet
     */
    Task (uint8_t *packet, uint8_t length);

    /**
     * @brief      `free`s the `args` member.
     */
    ~Task();

 private:
    uint8_t priority;  /**< Not needed now */
    
    /**
     * @brief      determines priority of this task using information in RPC_defs.h?
     */
    void computePriority();
};

#endif