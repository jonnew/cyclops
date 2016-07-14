/*
Copyright (c) 2014 Jon Newman (jpnewman ~at~ mit <dot> edu)
All right reserved.

This file is part of the Cyclops Library (CL) for Arduino.

CL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Cyclops.h"

// Populate the LUTs
static const uint8_t a_in_lut_data[] = {(uint8_t)A0, (uint8_t)A1, (uint8_t)A2, (uint8_t)A3};
const uint8_t *Cyclops::_a_in_lut = a_in_lut_data;
static const uint8_t cs_lut_data[] = {(uint8_t)CS0, (uint8_t)CS1, (uint8_t)CS2, (uint8_t)CS3};
const uint8_t *Cyclops::_cs_lut = cs_lut_data;
static const uint8_t oc_lut_data[] = {(uint8_t)OC0, (uint8_t)OC1, (uint8_t)OC2, (uint8_t)OC3};
const uint8_t *Cyclops::_oc_lut = oc_lut_data;
static const uint8_t trig_lut_data[] = {(uint8_t)TRIG0, (uint8_t)TRIG1, (uint8_t)TRIG2, (uint8_t)TRIG3};
const uint8_t *Cyclops::_trig_lut = trig_lut_data;

Cyclops::Cyclops(Channel channel) : _channel(channel)
{
    // For ISR
    channel_copy = _channel;

    // Set pin modes
    pinMode(_oc_lut[_channel], OUTPUT);
    pinMode(_cs_lut[_channel], OUTPUT);

    // Trigger default is output
    pinMode(_trig_lut[_channel], OUTPUT);
    digitalWrite(_trig_lut[_channel], LOW);

    // Load DAC line
    pinMode(LDAC, OUTPUT);

    // Get the CS and load-DAC lines ready
    digitalWrite(_cs_lut[_channel], HIGH);
    digitalWrite(LDAC, HIGH);

    // Initialize the OC pin
    digitalWrite(_oc_lut[_channel], LOW);

	// Initialize SPI bus
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();

    // Start DAC at 0V
    dac_prog_voltage(0);
    dac_load();
}

float Cyclops::over_current_protect(float current_limit_mA)
{
    // Measure the current
    float current_mA = measure_current();

    // If we are over-current, then turn off input
    if (current_mA > current_limit_mA) {
        _oc_trig = 1;
        digitalWrite(_oc_lut[_channel], HIGH);  // Switch off the input
    } else if (_oc_trig) {
        _oc_trig = 0;
        digitalWrite(_oc_lut[_channel], LOW);  // Allow input
    }

    return current_mA;
}

float Cyclops::measure_current(void)
{
    int sensor_value = analogRead(_a_in_lut[_channel]);
    return 1000.0 * (float)sensor_value * (5.0 / 1023.0);
}

void Cyclops::dac_send_test_waveform(void)
{
    uint16_t v_out = 0;

    while (v_out < 4085) {
        v_out += 10;
        dac_prog_voltage(v_out);
        dac_load();
    }
    while (v_out > 10) {
        v_out -= 10;
        dac_prog_voltage(v_out);
        dac_load();
    }
}

// TODO: User timer interupt for better accuracy
void Cyclops::dac_generate_waveform(uint16_t voltage[], uint16_t length, uint32_t sample_period_us)
{
    for (uint16_t i = 0; i < length; i++) {
        dac_prog_voltage(voltage[i]);
        dac_load();
        delayMicroseconds(sample_period_us - DAC_UPDATE_DELAY_USEC);
    }
}

void Cyclops::dac_prog_voltage(uint16_t voltage)
{
    // Create data packet
    unsigned int spi_out = DAC_CONF_ACTIVE | (voltage & 0x0fff);

    // take the SS pin low to select the chip
    digitalWrite(_cs_lut[_channel], LOW);

    //  send in the address and value via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // take the SS pin high to de-select the chip:
    digitalWrite(_cs_lut[_channel], HIGH);

}

void Cyclops::dac_load(void) {

    digitalWrite(LDAC, LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(LDAC, HIGH);
}

void Cyclops::dac_shutdown(void) {

	// Create data packet
    unsigned int spi_out = DAC_CONF_SHDN;

    // Take the CS pin low to select the chip
    digitalWrite(_cs_lut[_channel], LOW);

    // Send in the config and data via SPI:
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);

    // Take the CS pin high to de-select the chip:
    digitalWrite(_cs_lut[_channel], HIGH);
}

// Only works with first channel if using Arudino Leonardo (Rev 3.5x of
// Cyclops)
void Cyclops::attach_interupt(void (*user_func)(void)) {

	// Set trigger line as input and pull down weakly
    pinMode(_trig_lut[_channel], INPUT);
	digitalWrite(_trig_lut[_channel], LOW);

	// Enable pin-change interupt on port B, which is the only one
	// used by the Cyclops for triggering
    //
    // TODO: No longer true...
    //       Now we have Trig 1 = PD6, Trig 2 = PD1, Trig 3 = PD0
	sbi(PCICR, PCIE0);

	switch (_trig_lut[_channel]) {
		case TRIG0:
			sbi(PCMSK0, PCINT7);
			break;
		//case TRIG1:
		//	sbi(PCMSK0, PCINT5);
		//	break;
		//case TRIG2:
		//	sbi(PCMSK0, PCINT6);
		//	break;
		//case TRIG3:
		//	sbi(PCMSK0, PCINT7);
		//	break;
	}

	// Assign function pointer for the interput handler to execute
	interupt_func[_channel] = user_func;
}

// Interupt handler
void isr(void) {

    Serial.println(PINB);

	// get the pin states for the indicated port.
	if (PINB & PCMSK0 && interupt_func[channel_copy] != NULL)
        interupt_func[channel_copy]();
}

SIGNAL(PCINT0_vect) {

    char sreg_bak = SREG;
    cli();
    isr();
    SREG = sreg_bak;
}
