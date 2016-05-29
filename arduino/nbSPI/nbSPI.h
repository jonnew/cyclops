/*
 * SPI helper library for arduino inside a Cyclops.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _CYCLOPS_SPI_H
#define _CYCLOPS_SPI_H

#include <Arduino.h>
#include <SPI.h>

/*
 * The ISR just toggles the volatile ``_busy_X`` flag.
 */
 
extern volatile bool _busy_0; /**< ``true`` if the SPI0 device is busy in a transfer. */
// volatile bool _pending_1;

/**
 * @brief      Sets up the SPI registers.
 */
void initSPI();

/**
 * @brief
 * Begins a SPI transfer. Uses ``_busy_X`` to determine if SPI is available.
 * 
 * @param[in]  device_id  The SPI device to use (incase of multiple SPI devices on Teensy)
 * @param[in]  data       1 Byte of data
 *
 * @return     0 if transfer was started (i.e., SPI device was available), 1 otherwise.
 */
uint8_t send(uint8_t device_id, uint8_t data);

#endif
