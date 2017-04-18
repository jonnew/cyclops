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

// Trigger stimulus trains via simple serial commands:
// <10>
// < = Start Character
// 10 = Number of pulses in train
// > = End character

#include <Cyclops.h>

// Create a single cyclops object. CH0 corresponds to a physical board with
// jumper pads soldered so that OC0, CS0, TRIG0, and A0 are used.
Cyclops cyclops0(CH0);

// 1 msec, full-scale pulse
uint32_t pulse_width_msec = 1;
uint16_t pulse_seq_length = 2;
uint16_t output_pulse[] = {4095, 0};

// Train parameters
uint16_t num_pulses = 1;
uint16_t pulse_period_msec = 50;

void setup()
{
    // Start the device
    Cyclops::begin();

    // Zero out the DAC
    cyclops0.dac_load_voltage(0);

    // Start serial port
    Serial.begin(9600);
}

void loop()
{
    num_pulses = processInput();

    if (num_pulses) {
        Serial.print(F("Sending pulses:"));
        Serial.println(num_pulses);

        // Produce pulse train
        uint16_t n = 0;
        while (n < num_pulses) {

            cyclops0.dac_generate_waveform(
                output_pulse, pulse_seq_length, 1000 * pulse_width_msec);
            delay(pulse_period_msec - pulse_width_msec);
            n++;
        }
    }
}

uint16_t processInput()
{
    uint16_t receivedNumber = 0;

    while (Serial.available()) {

        byte c = Serial.read();

        switch (c) {
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
