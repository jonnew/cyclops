/**
 * @file Task.h
 * 
 * @page howto-serial Using the Serial RPC Interface
 * 
 * @section serial-overview Overview
 * This library comes with a handy collection of *Remote Procedure Calls* which can
 * be invoked by simple Serial packets.
 * @note       You cannot send any data to the USB Serial port from the PC while
 * using the RPC, however you can program the Teensy to *write anything* to the
 * Serial port.
 * 
 * The interface is well designed,
 * 
 * * Serial  port communications (read and write) are non-blocking.
 * * Recieved packets are pushed onto a *first come, first serve* Task Queue,
 * without servicing them.
 * * The objects involved are available in your sketch, for better understanding of
 * the library's working. **You can even post (using Queue::pushTask) Tasks to the same Queue being used for
 * RPC, based on your needs!**
 * 
 * @section rpc-teensy Teensy 3.X
 * The steps to use inbuilt RPC are:
 * 
 * 1. Include ``Task.h``
 * 2. Create the *derivation*-of-Source instances, as per your requirement.
 * 3. Create an array of pointers -- to -- Source instances. Ofcourse, there are no
 * Source instances (because it is a virtual class), this array will hold pointers
 * to the instances created in (2).
 * @attention  You must fill the array with the pointers in the *exact order* in
 * which they were declared!
 * 4. Assign the array to CyclopsLib::globalSourceList_ptr. The program will 
 * not compile unless this assignment is made.
 * 5. Create a Queue instance, (say myQueue).
 * @note       Now, do the following in ```loop()``...
 * 6. Apart form the normal waveform stuff, add a snippet as following:
 * @code
 *  CyclopsLib::readSerialAndPush(&myQueue);
 *  if (myQueue.size > 0){
        Task* t = myQueue.peek();
        t->compute();
        myQueue.pop();
    }
 * @endcode
 * This snipet will parse the recieved bytes into Task objects and push them on the
 * Queue you created. The ``if { ... }`` section will service the top-of-queue Task.
 *  
 * See ``/lib/task/examples/rpc_loop`` for an example.
 * 
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
    uint8_t taskID,             /**< Not used */
            channelID,          /**< The channel _(or channels, as a bitmask)_ on which the Task applies */
            commandID,          /**< @ref rpc-desc "RPC Description" */
            argsLength;         /**< length of args array in "bytes" */
    char    args[RPC_MAX_ARGS]; /**< Space for arguments of any command */

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

namespace CyclopsLib{
    /**
     * @ingroup    ns-CyclopsLib
     * @brief      Reads the device serial buffer and parses the packets into
     *             Task objects.
     * @details
     * This function does not busy wait on partially recieved packets, it will
     * return immediately if a packet cannot be formed, _yet_. If many packets
     * are waiting in the buffer, all of them would be converted into Task
     * objects.
     * @param      q     The Queue object which will be populated.
     */
    void readSerialAndPush(Queue *q);
}
/*
class PriorityQueue{
};
*/

#endif