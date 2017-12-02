/*
Copyright (c) Jon Newman (jpnewman ~at~ mit <dot> edu)
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

#include <Cyclops.h>

// Create a single cyclops object. CH0 corresponds to a physical board with
// jumper pads soldered so that OC0, CS0, TRIG0, and A0 are used.
Cyclops cyclops0(CH0);

// 1 msec, full-scale pulse
uint16_t pulse_width_us = 1000;
uint16_t pulse_seq_length = 2;
uint16_t output_pulse[] = {4095, 0};

void setup()
{
    // Start the device
    Cyclops::begin();

    // Zero out the DAC
    cyclops0.dac_load_voltage(0);

    // Enter user-defined function to run on TRIG HIGH
    cyclops0.set_trigger(send_pulse, RISING);
}

void loop()
{
    // Nothing to do, all action in the interrupt handler
}

// Type signature should be void function(void)
void send_pulse()
{
    cyclops0.dac_generate_waveform(
        pulse_width_us, output_pulse, pulse_seq_length);
}
