/** @file SerialParse.h
    @brief Provides methods to parse the RPC packets and make Task instances.
           This file is part of CL.
    
    @author Ananya Bahadur
*/

#ifndef SerialParse_h
#define SerialParse_h

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Task.h"

/**
 * @brief      Parses the recieved "packet" which conforms to the Cyclops-RPC format into a Task instance.  
 *             @see RPC 
 *
 * @param[in]  packet  Pointer to array, this array lies in the Serial buffer. It's contents need to be copied into the `Task.args`
 * @param[in]  length  length in bytes of the packet
 * @param[out] task    Pointer to Task instance created by caller. This will be populated.
 */
void parseSerial(uint8_t *packet, uint8_t length, Task *task);

