/*
 Cyclops.cpp - Cyclops Driver
 Copyright (c) 2014 Jonathan Newman  All right reserved.

*/

#include <stdlib.h>
#include <stdio.h>

#include "Arduino.h"
#include "Cyclops.h"
#include <SPI.h>

// Public Methods
Cyclops::Cyclops()
{
    // Set pin modes
    pinMode (CS1, OUTPUT);
    pinMode (CS2, OUTPUT);
    pinMode (CS3, OUTPUT);
    pinMode (CS4, OUTPUT);
    pinMode (LDAC, OUTPUT);

    // Get the load-dac line ready
    digitalWrite(CS1, HIGH);
    digitalWrite(CS2, HIGH);
    digitalWrite(CS3, HIGH);
    digitalWrite(CS4, HIGH);
    digitalWrite(LDAC, HIGH);

    // Initialize SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();

    // Intitialize private variables
    _v_out = 0;
}

/* default implementation: may be overridden */
void Cyclops::send_test_waveform(int channel)

    while (_v_out < 4085)
    {
      _v_out += 10;
      update_DAC(channel, 0x1000);
    }
    while (_v_out > 10)
    {
      _v_out -= 10;
      update_DAC(channel, 0x1000);
    }
}

// Private Methods 

void Cyclops::update_dac(int channel, short address) {

    // Create data packet
    int data = _v_out & 0x0fff;
    int spi_out = address | data;

    // take the SS pin low to select the chip
    digitalWrite(channel, LOW);

    //  send in the address and value via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // take the SS pin high to de-select the chip:
    digitalWrite(mcp_cs, HIGH);

    // Update both DAC outputs by loading their
    // output registers w/ >100 ns delay
    digitalWrite(LDAC, LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(LDAC, HIGH);
}
