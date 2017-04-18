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

// Parameters of the chirp waveform
#define CHIRP_TIME_MS 5000 // Length of chirp waveform in msec
#define FREQ_START 0.5f    // Start frequency in Hz
#define FREQ_END 30.0f     // End frequency in Hz

// Create a single cyclops object. CH0 corresponds to a physical board with
// jumper pads soldered so that OC0, CS0, TRIG0, and A0 are used.
Cyclops cyclops0(CH0);

// Chirp frequency ramp parameter
float beta = 1.0;

void setup()
{
    // Chirp parameter
    beta = (FREQ_END - FREQ_START) / (((float)CHIRP_TIME_MS) / 1000.0);
    Cyclops::begin();

    // Zero out the DAC
    cyclops0.dac_load_voltage(0);
}

void loop()
{
    float now = ((float)(millis() % CHIRP_TIME_MS)) / 1000.0;

    float freq
        = 2.0 * PI_CONST * (FREQ_START * now + (beta / 2.0) * pow(now, 2));
    unsigned int voltage = (unsigned int)(4095.0 * (sin(freq) / 2.0 + 0.5));

    // Program the DAC and load the voltage
    cyclops0.dac_load_voltage(voltage);
}
