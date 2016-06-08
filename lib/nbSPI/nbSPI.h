/*
 * SPI helper library for arduino inside a Cyclops.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

/** @file nbSPI.h
 * @section spi_speed_ups Practical Findings
 * See the example in ``nbSPI/examples/basic_test.ino``.
 * @subsection nbspi_arduino_uno Arduino Uno
 * Params      | Value | Unit
 * ------      | ----- | ----
 * ``FCPU``    | 16    | MHz
 * ``SPI_CLK`` | 8     | MHz
 * 
 * We can see clearly that the CPU can perform atmost 16 instructions
 * between initiation and completion of 1B SPI transfer. In the above
 * mentioned example, a counter is incremented _during_ the SPI transfer.
 * For back-to-back \\(n\\) transmissions, counter was incremented just \\(n-1\\)
 * times!
 * 
 * @remark
 * There was significant overhead in the busy-check-condition, which cannot
 * be neglected! This overhead will exist in real implementations also but
 * it's effect is decreased if there _really exist other tasks to perform_.
 *
 * **Clearly, non-blocking SPI provides little performance gain for these devices.
 * But, it can be no slower than blocking SPI transfers.**
 * @author Ananya Bahadur
 */

#ifndef CL_NB_SPI_H
#define CL_NB_SPI_H

#include <Arduino.h>
#include <SPI.h>

//MCP4921 stuff
#define DAC_CONF_ACTIVE 	  (0x1000) // 0101
#define DAC_CONF_SHDN 		  (0x1000)
#define DAC_UPDATE_DELAY_USEC 54 // LEONARDO SPECIFIC
#define DAC_BLOCK_SIZE        2  /**< The no. of bytes for each SPI transfer. HI:LO <=> {Command, 12b Data} */

/*
 * The ISR just toggles the volatile ``_busy_X`` flag.
 */

/**
 * @brief      Sets up the SPI registers.
 * @details
 * Strives to use the fastest CLK freq. as possible.
 * 
 * For Teensy, MAX = 20MHz or ``CPU_freq/2``, _whichever is smaller!_
 * For Arduino, MAX = 8MHz
 */
void initSPI();

/**
 * @brief
 * Begins a SPI transfer.
 * @details
 * Uses ``_busy_X`` to determine if SPI is available. ``_busy_X`` is not accessible to user-code.
 * 
 * Recommended usage:
 * \code{.cpp}
 * // This will busy wait till a write is done. Defeats the purpose of
 * // interrupts, and also of this library.
 * while (sendSPI(X, <data>) == 1){
 *     //device busy... abort OR
 *     //do something about it OR
 *     //do nothing, just wait!
 * }
 * 
 * OR
 * 
 * if (sendSPI(X, <data>) == 0){
 *     //device was available, write is complete
 *     .
 *     do something about that...
 *     .
 * }
 * else{
 *     //device busy... abort (or do something about it)
 *     .
 *     .
 * }
 * \endcode
 * @param[in]  device_id  The SPI device to use (incase of multiple SPI devices on Teensy)
 * @param[in]  data       1 Byte of data
 *
 * @return     0 if transfer was started (i.e., SPI device was available), 1 otherwise.
 */
uint8_t sendSPI(uint8_t device_id, uint8_t data);

#endif
