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
static const uint8_t vi_lut_data[]
    = {(uint8_t)VI0, (uint8_t)VI1, (uint8_t)VI2, (uint8_t)VI3};
const uint8_t *Cyclops::_vi_lut = vi_lut_data;
static const uint8_t cs_lut_data[]
    = {(uint8_t)CS0, (uint8_t)CS1, (uint8_t)CS2, (uint8_t)CS3};
const uint8_t *Cyclops::_cs_lut = cs_lut_data;
static const uint8_t oc_lut_data[]
    = {(uint8_t)OC0, (uint8_t)OC1, (uint8_t)OC2, (uint8_t)OC3};
const uint8_t *Cyclops::_oc_lut = oc_lut_data;
static const uint8_t trig_lut_data[]
    = {(uint8_t)TRIG0, (uint8_t)TRIG1, (uint8_t)TRIG2, (uint8_t)TRIG3};
const uint8_t *Cyclops::_trig_lut = trig_lut_data;
//static const uint8_t ldac_lut_data[]
//    = {(uint8_t)LDAC0, (uint8_t)LDAC1, (uint8_t)LDAC2, (uint8_t)LDAC3};
//const uint8_t *Cyclops::_ldac_lut = ldac_lut_data;
static float oc_limit_data[] = {max_mA, max_mA, max_mA, max_mA};
float *Cyclops::_oc_limit_lut = oc_limit_data;

void Cyclops::check_current()
{
    // Measure the current on each channel. Shut it down if it is OC.
    for (int i = 0; i < num_chan; i++) {

        // No overcurrent protection
        if (_oc_limit_lut[i] >= max_mA)
            continue;

        auto v = analogRead(_vi_lut[i]);
        const float mA = mA_conv_coeff * (float)v;

        if (mA > _oc_limit_lut[i]) {
            digitalWrite(_oc_lut[i], LOW); // Switch off the input
        } else
            digitalWrite(_oc_lut[i], HIGH); // Allow input
    }
}

Cyclops::Cyclops(Channel chan, float current_limit_mA)
: channel(chan)
{
    // Over current
    pinMode(_oc_lut[channel], OUTPUT);
    digitalWrite(_oc_lut[channel], HIGH);

    // Trigger input
    pinMode(_trig_lut[channel], INPUT);
    digitalWrite(_trig_lut[channel], LOW); // Pull down

    // CS
    pinMode(_cs_lut[channel], OUTPUT);
    digitalWrite(_cs_lut[channel], HIGH);

    // Configure load-DAC line
    pinMode(LDAC, OUTPUT);
    digitalWrite(LDAC, HIGH);

    // Current read analog input
    pinMode(_vi_lut[channel], INPUT);

    // Set the overcurrent limit
    oc_limit_data[channel] = current_limit_mA;

    // Set the analog read resolution
    analogReference(DEFAULT);  // Use 3.3V reference
}

void Cyclops::begin()
{
    // Attach overcurrent interupt an start overcurrent check timer
    Timer1.attachInterrupt(check_current);
    Timer1.initialize(OC_PERIOD_US);
    Timer1.start();

    // Start SPI Bus
    SPI.setDataMode(SPI_MODE3);
    SPI.setBitOrder(MSBFIRST);
    SPI.setMOSI(MOSI);
    SPI.setSCK(SCLK);
    SPI.begin();
}

float Cyclops::measure_current(void) const
{
    return mA_conv_coeff * (float)analogRead(_vi_lut[channel]);
}

// TODO: User timer interupt for better accuracy
void Cyclops::dac_generate_waveform(const uint16_t voltage[],
                                    const uint16_t length,
                                    const uint32_t sample_period_us) const
{
    for (uint16_t i = 0; i < length; i++) {
        dac_load_voltage(voltage[i]);
        delayMicroseconds(sample_period_us);
    }
}

int Cyclops::dac_prog_voltage(const uint16_t voltage) const
{
    if (voltage > 4095)
        return 1;

    // Create data packet and send
    uint16_t spi_out = DAC_CONF_ACTIVE | (voltage & 0x0fff);
    digitalWrite(_cs_lut[channel], LOW);
    SPI.transfer(spi_out >> 8 & 0xff);
    SPI.transfer(spi_out >> 0 & 0xff);
    digitalWrite(_cs_lut[channel], HIGH);

    return 0;
}

void Cyclops::dac_load(void)
{
    digitalWrite(LDAC, LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(LDAC, HIGH);
}

int Cyclops::dac_load_voltage(const uint16_t voltage) const
{
    int rc = dac_prog_voltage(voltage);
    dac_load();
    return rc;
}

void Cyclops::set_trigger(void (*user_func)(void), const int mode)
{
    attachInterrupt(_trig_lut[channel], user_func, mode);
}
