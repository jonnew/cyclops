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

#define PI_CONST 3.14159265358979323846
#define FREQ 0.5f // Frequency in Hz of sine wave

const float freq_rad = 2 * PI_CONST * FREQ;

// Create two cyclops devices, each with its own current limit. e.g. CH0
// corresponds to a physical board with jumper pads soldered so that OC0, CS0,
// TRIG0, and A0 are used.  Set the current limit to 500 mA on CH0 and 200 mA
// on CH1.
Cyclops cyclops0(CH0, 500);
Cyclops cyclops1(CH1, 200);

void setup()
{
    // Start both cyclops devices
    Cyclops::begin();

    // Zero out the DACs
    cyclops0.dac_load_voltage(0);
    cyclops1.dac_load_voltage(0);
}

// NOTE: you do not need to use the onboard DAC to use overcurrent protection.
// If you want to enable overcurrent protection using the EXT input, simply
// provide an empty loop() function here. The overcurrent protection loop will
// run in the background and take over if it detects an overcurrent condition
// in the analog portion of the drive circuitry.
void loop()
{
    // Each board includes an onboard 12-bit (4095 position) DAC spanning 0-5
    // volts. Use the DAC to generate a sine wave and a randomly stepping
    // random voltage from 0 to 5V. This voltage is always scaled by the front
    // dial before being used as a feedback reference.

    float now = ((float)millis()) / 1000.0;
    unsigned int sine_volt
        = (unsigned int)(4095.0 * (0.5 * sin(freq_rad * now) + 0.5));
    const int rand_volt = random(0, 4096);

    // Program the DAC registers with the random voltages
    cyclops0.dac_prog_voltage(sine_volt);
    cyclops1.dac_prog_voltage(rand_volt);

    // Update the outputs simulateously.
    Cyclops::dac_load();
}
