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
static const uint8_t a_in_lut_data[] = {(uint8_t)ADC0, (uint8_t)ADC1, (uint8_t)ADC2, (uint8_t)ADC3};
const uint8_t *Cyclops::_a_in_lut = a_in_lut_data;
static const uint8_t cs_lut_data[] = {(uint8_t)CS0, (uint8_t)CS1, (uint8_t)CS2, (uint8_t)CS3};
const uint8_t *Cyclops::_cs_lut = cs_lut_data;
static const uint8_t oc_lut_data[] = {(uint8_t)OC0, (uint8_t)OC1, (uint8_t)OC2, (uint8_t)OC3};
const uint8_t *Cyclops::_oc_lut = oc_lut_data;
static const uint8_t trig_lut_data[] = {(uint8_t)TRIG0, (uint8_t)TRIG1, (uint8_t)TRIG2, (uint8_t)TRIG3};
const uint8_t *Cyclops::_trig_lut = trig_lut_data;
static const uint8_t ldac_lut_data[] = {(uint8_t)LDAC0, (uint8_t)LDAC1, (uint8_t)LDAC2, (uint8_t)LDAC3};
const uint8_t *Cyclops::_ldac_lut = ldac_lut_data;
static const uint8_t trig_port_pos_lut_data[] = {7, 255, 255, 255};  // TODO: see note at ISR below
const uint8_t *Cyclops::_trig_port_pos_lut = trig_port_pos_lut_data;

Cyclops::Cyclops(Channel _channel) : channel(_channel), _oc_trig(0)
{
    // Set pin modes
    pinMode(_oc_lut[channel], OUTPUT);
    pinMode(_trig_lut[channel], INPUT);
    // Pull the trigger line down weakly
    digitalWrite(_trig_lut[channel], LOW);
    // Initialize the OC pin
    digitalWrite(_oc_lut[channel], LOW);

    #ifndef CORE_TEENSY
        // Configure CS and load-DAC line (for Arduino)
        pinMode(LDAC, OUTPUT);
        pinMode(_cs_lut[channel], OUTPUT);
        digitalWrite(_cs_lut[channel], HIGH);
    #else
        // (for Teensy)
        // Configure load-DAC line
        pinMode(_ldac_lut[channel], OUTPUT);
        digitalWrite(_ldac_lut[channel], HIGH);
        SPI_fifo.setup_pcs_pin(channel, _cs_lut[channel]);
    #endif

    
}

#ifndef CORE_TEENSY
    void Cyclops::selectChip(){
        digitalWrite(_cs_lut[channel], LOW);
    }

    void Cyclops::deselectChip(){
        digitalWrite(_cs_lut[channel], HIGH);   
    }
#endif

float Cyclops::overCurrentProtect(float current_limit_mA)
{
    // Measure the current
    float current_mA = measureCurrent();

    // If we are over-current, then turn off input
    if (current_mA > current_limit_mA) {
        _oc_trig = 1;
        digitalWrite(_oc_lut[channel], HIGH);  // Switch off the input
    } else if (_oc_trig) {
        _oc_trig = 0;
        digitalWrite(_oc_lut[channel], LOW);  // Allow input
    }
    return current_mA;
}

float Cyclops::measureCurrent(void)
{
    int sensor_value = analogRead(_a_in_lut[channel]);
    return 1000.0 * sensor_value * (5.0 / 1023.0);
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
    uint16_t spi_out = DAC_CONF_ACTIVE | (voltage & 0x0fff);
    #ifndef CORE_TEENSY
        // take the SS pin low to select the chip
        digitalWrite(_cs_lut[channel], LOW);
        SPI.transfer(spi_out >> 8 & 0xff);
        SPI.transfer(spi_out >> 0 & 0xff);
        digitalWrite(_cs_lut[channel], HIGH);
    #else
        SPI_fifo.write16(channel, spi_out);
    #endif
}

void Cyclops::dac_load(void)
{
    digitalWrite(_ldac_lut[channel], LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(_ldac_lut[channel], HIGH);
}

void Cyclops::dac_shutdown(void) {

    // Create data packet
    unsigned int spi_out = DAC_CONF_SHDN;    
    #ifndef CORE_TEENSY
        // Take the CS pin low to select the chip
        digitalWrite(_cs_lut[channel], LOW);
        SPI.transfer(spi_out >> 8 & 0xff);
        SPI.transfer(spi_out >> 0 & 0xff);
        // Take the CS pin high to de-select the chip:
        digitalWrite(_cs_lut[channel], HIGH);
    #else
        SPI_fifo.write16(channel, spi_out);
    #endif
    
}

void Cyclops::attach_interrupt(void (*user_func)(void), int mode) {
    #ifndef CORE_TEENSY
        // Enable pin-change interupt on port B, which is the only one
        // used by the Cyclops for triggering
        //
        // TODO: No longer true...
        //       Now we have Trig 1 = PD6, Trig 2 = PD1, Trig 3 = PD0
        sbi(PCICR, PCIE0);

        switch (_trig_lut[channel]) {
            case TRIG0:
                sbi(PCMSK0, PCINT7);
                break;
            //case TRIG1:
            //  sbi(PCMSK0, PCINT5);
            //  break;
            //case TRIG2:
            //  sbi(PCMSK0, PCINT6);
            //  break;
            //case TRIG3:
            //  sbi(PCMSK0, PCINT7);
            //  break;
        }

        // Assign function pointer for the interput handler to execute
        interupt_func[_trig_port_pos_lut[channel]] = user_func;
    #else
        // Teensy core does the job of handling pin change
        // interrupts pretty well. See "/cores/pins_teensy.c"
        attachInterrupt(_trig_lut[channel], user_func, mode);
    #endif
}

#ifndef CORE_TEENSY
    // TODO: Way too much going on here.
    // Interupt handler
    void isr(void) {
        uint8_t bit;
        uint8_t curr;
        uint8_t mask;
        uint8_t pin;

        // get the pin states for the indicated port.
        curr = PINB;
        mask = curr ^ interupt_last; // Bitwise xor for change detection
        interupt_last = curr;

        // mask is pins that have changed. screen out non pcint pins.
        if ((mask &= PCMSK0) == 0)
            return;

        // mask is pcint pins that have changed.
        for (uint8_t i = 0; i < 8; i++) {
            bit = 0x01 << i;
            pin = i;
            if (bit & mask) {
                // Trigger interrupt if bit is currently high.
                if ((interupt_func[pin] != NULL)) { // (curr & bit) &&
                    interupt_func[pin]();
                }
            }
        }
    }

    SIGNAL(PCINT0_vect) {

        char sreg_bak = SREG;
        cli();
        isr();
        SREG = sreg_bak;
    }
#endif