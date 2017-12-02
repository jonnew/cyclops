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

// Pot is set to mid range (5V out of DAC -> 750 mA).
const float mA_cyclops_limit = 1500;
const float pot_position = 0.5;
const float mA_max = 500;

// DAC setting to get mA_max
uint16_t dac_mA_max;

// Create a cyclops device on CH0 limited to  500 mA
Cyclops cyclops(CH0, mA_max);

void setup()
{
    // Start device
    Cyclops::begin();

    // Zero out the DAC
    cyclops.dac_load_voltage(0);

    dac_mA_max = (4095.0/5.0) * (mA_max / (0.5 * mA_cyclops_limit));
}

void loop()
{
    delay(1000);

    while (1) {

        // 0.5 max output for 2 seconds
        cyclops.dac_load_voltage(0.5 * dac_mA_max);
        delay(2000);

        // 1.0 output for 5 seconds
        cyclops.dac_load_voltage(dac_mA_max);
        delay(5000);

        // 0.5 max output for 2 seconds
        cyclops.dac_load_voltage(0.5 * dac_mA_max);
        delay(2000);

        // 0 output for 1 second
        cyclops.dac_load_voltage(0);
        delay(1000);
    }
}
