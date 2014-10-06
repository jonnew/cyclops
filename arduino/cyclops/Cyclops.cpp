/*
 Cyclops.cpp - Cyclops Driver
 Copyright (c) 2014 Jonathan Newman  All right reserved.

*/

#include "Cyclops.h"

// Initialize the CS and analog in look-up tables
static const uint16_t cs_lut_data[] = {(uint16_t)CS0, (uint16_t)CS1, (uint16_t)CS2, (uint16_t)CS3};
const uint16_t *Cyclops::_cs_lut = cs_lut_data;
static const uint16_t a_in_lut_data[] = {(uint16_t)A0, (uint16_t)A1, (uint16_t)A2, (uint16_t)A3};
const uint16_t *Cyclops::_a_in_lut = a_in_lut_data;

Cyclops::Cyclops(void) {

    // Set pin modes
    pinMode(CS0, OUTPUT);
    pinMode(CS1, OUTPUT);
    pinMode(CS2, OUTPUT);
    pinMode(CS3, OUTPUT);
    pinMode(TRIG0, INPUT);
    pinMode(TRIG1, INPUT);
    pinMode(TRIG2, INPUT);
    pinMode(TRIG3, INPUT);
    pinMode(WIPER_UD, OUTPUT);
    pinMode(LDAC, OUTPUT);
    pinMode(OC_COMP, OUTPUT);

    // Get the CS and load-dac lines ready
    digitalWrite(CS0, HIGH);
    digitalWrite(CS1, HIGH);
    digitalWrite(CH2, HIGH);
    digitalWrite(CH3, HIGH);
    digitalWrite(LDAC, HIGH);

    // Initialize the OC_COMP pin
    digitalWrite(OC_COMP, LOW);
	
	// Initialize SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();

    // Initialize private variables
    _v_out = 0;
    _current_mA = 0;
	_initialized = true;
}

void Cyclops::over_current_protect(uint16_t chan, float current_limit_mA) {

    // Measure the current
    _current_mA = measure_current(chan);

    // If we are over-current, then adjust in the input voltage divider
    if (_current_mA > current_limit_mA) {

        // Signal that we are in compenstation mode
        digitalWrite(OC_COMP, HIGH);

        // Raise the resistance of the top of the voltage divder
        mcp4022_decrement_pot(chan, 1);
    }
    // If the input stage is not at nominal resistance, then move toward it
    else if (_wiper_position[chan] > NOM_WIPER_POS-1) {

        // Signal that we are in compenstation mode
        digitalWrite(OC_COMP, HIGH);
        mcp4022_decrement_pot(chan, 1);
    }
    // If the input stage is not at nominal resistance, then move toward it
    else if (_wiper_position[chan] < NOM_WIPER_POS-1) {

        // Signal that we are in compenstation mode
        digitalWrite(OC_COMP, HIGH);
        mcp4022_increment_pot(chan, 1);
    }
    else {
        
        // Signal that we are out of compensation mode
        digitalWrite(OC_COMP, LOW);
    }
}

float Cyclops::measure_current(uint16_t chan) {
    
    int sensor_value = analogRead(_a_in_lut[chan]);
    return 1000.0 * (float)sensor_value * (5.0 / 1023.0);
}

void Cyclops::mcp4921_send_test_waveform(uint16_t chan) {

    uint16_t v_out = 0;

    while (_v_out < 4085) {
        v_out += 10;
        mcp4921_load_data(chan, v_out);
        mcp4921_load_dac();
    }
    while (_v_out > 10) {
        v_out -= 10;
        mcp4921_load_data(chan, v_out);
        mcp4921_load_dac();
    }
}

void Cyclops::mcp4921_single_shot(uint16_t chan, uint16_t voltage) {

    mcp4921_load_data(chan, voltage);
    mcp4921_load_dac();
}

void Cyclops::mcp4921_generate_waveform(uint16_t chan, int voltage[], uint16_t length, uint16_t sample_period_us) {

    for (int i = 0; i < length; i++) {
        mcp4921_single_shot(chan, voltage[i]);
        delayMicroseconds(sample_period_us);  
    }
}

void Cyclops::mcp4921_load_data(uint16_t chan,  uint16_t voltage) {

    // Create data packet
    unsigned int spi_out = DAC_CONF_ACTIVE | (voltage & 0x0fff);

    // take the SS pin low to select the chip
    digitalWrite(_cs_lut[chan], LOW);

    //  send in the address and value via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // take the SS pin high to de-select the chip:
    digitalWrite(_cs_lut[chan], HIGH);

}

void Cyclops::mcp4921_load_dac(void) {

    digitalWrite(LDAC, LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(LDAC, HIGH);
}

void Cyclops::mcp4921_shutdown_dac (uint16_t chan) {

	// Create data packet
    unsigned int spi_out = DAC_CONF_SHDN;

    // Take the CS pin low to select the chip
    digitalWrite(_cs_lut[chan], LOW);

    // Send in the config and data via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // Take the CS pin high to de-select the chip:
    digitalWrite(_cs_lut[chan], HIGH);

}

void Cyclops::mcp4022_set_nom_AWR (uint16_t chan) {

	// This trimmer is 50k with 64 positions
	// Decrement the wiper 64 times to 0x00
	// ensures that the wiper is connected
	// to terminal B. Incrementing the wiper
	// 64 times ensures that the wiper is
	// connected to terminal A. 
	mcp4022_decrement_pot(chan, 64);

	// 5.6k = 50k * x/64. x ~ 7
	mcp4022_increment_pot(chan, NOM_WIPER_POS);

    //_wiper_position[chan] = NOM_WIPER_POS;

	// Write to EEPROM
	mcp4022_save_pot_resistance(chan);
}

void Cyclops::mcp4022_unpulse_pot(byte n) {

	// Increcment n times
	for (int i = 0; i < n; i++) {
		digitalWrite(WIPER_UD, LOW);
		digitalWrite(WIPER_UD, HIGH);
	}
}

void Cyclops::mcp4022_pulse_pot(byte n) {

	// Decrement n times
	for (int i = 0; i < n; i++) {
		digitalWrite(WIPER_UD, HIGH);
		digitalWrite(WIPER_UD, LOW);
	}
}

void Cyclops::mcp4022_increment_pot(uint16_t chan, byte n) {
	
	//Bring WIPER_UD HIGH
	digitalWrite(WIPER_UD, HIGH);

    // Take the CS pin low to select the chip
    digitalWrite(_cs_lut[chan], LOW);

	// Provide n U/D pulses
	mcp4022_unpulse_pot(n);	

    // Increment the wiper position
    if (_wiper_position[chan] + n >= 63) {
        _wiper_position[chan] = 63;
    }
    else {
        _wiper_position[chan] += n;
    }

    // Take the CS pin high to deselect the chip
    digitalWrite(_cs_lut[chan], HIGH);
}

void Cyclops::mcp4022_decrement_pot(uint16_t chan, byte n) {
	
	// Bring WIPER_UD LOW
	digitalWrite(WIPER_UD, LOW);

    // Take the CS pin low to select the chip
    digitalWrite(_cs_lut[chan], LOW);

	// Provide n U/D pulses
	mcp4022_pulse_pot(n);	

    // Decrment the wiper position
    if (_wiper_position[chan] - n <= 0) {
        _wiper_position[chan] = 0;
    }
    else {
        _wiper_position[chan] -= n;
    }

    // Take the CS pin high to deselect the chip
    digitalWrite(_cs_lut[chan], HIGH);
}

void Cyclops::mcp4022_save_pot_resistance(uint16_t chan) {

	// Bring WIPER_UD HIGH
	digitalWrite(WIPER_UD, HIGH);

    // Take the CS pin low to select the chip
    digitalWrite(_cs_lut[chan], LOW);

	// Delay for 10 us
	delayMicroseconds(10);

    // Take the CS pin high to deselect the chip
    digitalWrite(_cs_lut[chan], HIGH);
}

