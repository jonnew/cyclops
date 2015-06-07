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

#include <Cyclops.h>

// Unfortunately, when using the arudino IDE, these need to be 
// re-included here even though they are included in Cyclops.h
#include <SPI.h>
#include <Wire.h>

// Create 1 cyclops object. This corresponds to a physical board
// with jumper pads soldered so that CS0, TRIG0, and A0 are used
Cyclops cyclops0(CH0);

// Random voltage
long randVoltage;

void setup() {
  
  // Set input resistors on both boards to nominal A-W Resistance
  cyclops0.mcp4022_set_nom_AW_resistance();
  
}

void loop() {
  
  // Each board includes an onboard 12-bit (4095 position)
  // DAC spanning 0-5 volts.
  
  // Generate random voltage from 0 to 4095
  //randVoltage = random(0,4096);
  
  // Load the DAC registers with the random voltage and update
  // the output. 
  //cyclops0.mcp4921_load_voltage(randVoltage);
  //cyclops0.mcp4921_load_dac();
  
  // Call the overcurrent protection function to ensure
  // the mean current is limited to < 250 mA during random
  // waveform generation 
  //cyclops0.over_current_protect(250);

}

