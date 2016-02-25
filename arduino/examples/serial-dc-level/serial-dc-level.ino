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


// Set the DC voltage of the onboard DAC for LED power testing, e.g. <4095>
// < = Start Character
// 4095 = 12-bit voltage value
// > = End character

#include <Cyclops.h>

// Unfortunately, when using the arudino IDE, these need to be
// re-included here even though they are included in Cyclops.h
#include <SPI.h>
#include <Wire.h>

// Create a single cyclops object. CH0 corresponds to a physical board with
// jumper pads soldered so that OC0, CS0, TRIG0, and A0 are used.
Cyclops cyclops0(CH0);

// 1 msec, full-scale pulse
uint16_t dac_level = 0;

void setup()
{
    Serial.begin(115200);
}

void loop()
{

    dac_level = processInput();

    if (dac_level > 0 && dac_level <= 4095) {
        Serial.print(F("Updating DAC volage to "));
        Serial.println(dac_level);

        // Produce DC level
        cyclops0.dac_prog_voltage(dac_level);
        cyclops0.dac_load();
    }
}

uint16_t processInput()
{

    uint16_t receivedNumber = 0;

    while (Serial.available()) {

        byte c = Serial.read();

        switch (c)
        {
            case '<':
                receivedNumber = 0;
                break;

            case '>':
                return receivedNumber;

            case '0' ... '9':
                receivedNumber *= 10;
                receivedNumber += c - '0';
                break;
        }
    }
    
    return 0;
}
