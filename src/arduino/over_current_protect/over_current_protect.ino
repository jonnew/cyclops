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

// Unfortunately, when using the arudino IDE, these need to be
// re-included here even though they are included in Cyclops.h
#include <SPI.h>
#include <Wire.h>

// Create a single cyclops object. CH0 corresponds to a physical board with
// jumper pads soldered so that OC0, CS0, TRIG0, and A0 are used.
Cyclops cyclops0(CH0);

// Random voltage
long randVoltage;

void setup()
{
    // Nothing
}

// NOTE: you do not need to use the onboard DAC to use overcurrent protection.
// If you want to enable overcurrent protection using the EXT input, simply get
// rid of all signal generation stuff below: rand_voltage,
// cyclops0.dac_prog_voltage(), cyclops0.dac_load().
void loop()
{
    // Each board includes an onboard 12-bit (4095 position) DAC spanning 0-5
    // volts. Use the DAC to generate a randomly stepping random voltage from 0
    // to 5V. This voltage is always scaled by the front dial before being used
    // as a feedback reference.
    int rand_voltage = random(0, 4096);

    // Program the DAC registers with the random voltage and update the output.
    cyclops0.dac_prog_voltage(rand_voltage);
    cyclops0.dac_load();

    // Call the overcurrent protection function to ensure the mean current is
    // limited to < 100 mA during random waveform generation. This function
    // needs to be called continuously to enforce a particular maximum current
    // and to recover from an overcurrent state.
    cyclops0.over_current_protect(100);
}
