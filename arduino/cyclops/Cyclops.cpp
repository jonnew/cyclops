/*
 Cyclops.cpp - Cyclops Driver
 Copyright (c) 2014 Jonathan Newman  All right reserved.

*/

//#include <Arduino.h>
//#include <SPI.h>
//#include <avr/pgmspace.h>
#include "Cyclops.h"

// Public Methods
Cyclops::Cyclops(void) {

    // Set pin modes
    pinMode(CH1, OUTPUT);
    pinMode(CH2, OUTPUT);
    pinMode(CH3, OUTPUT);
    pinMode(CH4, OUTPUT);
    pinMode(LDAC, OUTPUT);

    // Get the load-dac line ready
    digitalWrite(CH1, HIGH);
    digitalWrite(CH2, HIGH);
    digitalWrite(CH3, HIGH);
    digitalWrite(CH4, HIGH);
    digitalWrite(LDAC, HIGH);
	
	// Initialize SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();
	
    // Initialize private variables
    _v_out = 0;
	_initialized = true;
}

void Cyclops::send_test_waveform(uint16_t chan) {

    while (_v_out < 4085) {
      _v_out += 10;
      update_dac(chan);
    }
    while (_v_out > 10) {
      _v_out -= 10;
      update_dac(chan);
    }
}

// Private Methods 

void Cyclops::update_dac(uint16_t chan) {

    // Create data packet
    unsigned int spi_out = DAC_CONF_ACTIVE | (_v_out & 0x0fff);

    // take the SS pin low to select the chip
    digitalWrite(chan, LOW);

    //  send in the address and value via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // take the SS pin high to de-select the chip:
    digitalWrite(chan, HIGH);

    // Update both DAC outputs by loading their
    // output registers w/ >100 ns delay
    digitalWrite(LDAC, LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(LDAC, HIGH);
}

void Cyclops::shutdown_dac(uint16_t chan) {

	// Create data packet
    unsigned int spi_out = DAC_CONF_SHDN;

    // take the SS pin low to select the chip
    digitalWrite(chan, LOW);

    //  send in the address and value via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // take the SS pin high to de-select the chip:
    digitalWrite(chan, HIGH);

}
