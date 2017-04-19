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

// Create 2 cyclops objects. These correspond to physical boards that are
// stacked on top of one another and share a common arduino. They should have
// their jumper pads soldered so that the first board uses OC0, CS0, TRIG0, and
// A0.  The second board uses OC1, CS1, TRIG1, and A1.
Cyclops cyclops0(CH0);
Cyclops cyclops1(CH1);

void setup()
{
    // Start the devices
    Cyclops::begin();

    // Zero out the DAC
    cyclops0.dac_load_voltage(0);
    cyclops1.dac_load_voltage(0);
}

void loop()
{
    // Each board includes an onboard 12-bit (4095 position)
    // DAC spanning 0-5 volts.

    // Bring both boards data registers to full scale and
    // synchronously update the DACs
    cyclops0.dac_prog_voltage(4095);
    cyclops1.dac_prog_voltage(4095);
    Cyclops::dac_load(); // dac_load is tied to both devices

    delayMicroseconds(100);

    // Bring both boards data registers to 0 scale and
    // synchronously update the DACs
    cyclops0.dac_prog_voltage(0);
    cyclops1.dac_prog_voltage(0);
    Cyclops::dac_load(); // dac_load is tied to both devices

    delayMicroseconds(100);
}
