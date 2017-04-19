/*
Copyright (c) 2015 Jon Newman (jpnewman ~at~ mit <dot> edu)
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

#ifndef Cyclops_h
#define Cyclops_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>
#include <TimerOne.h>

#define DAC_CONF_ACTIVE  (0x1000)   // DAC commands
#define DAC_CONF_SHDN    (0x1000)   //

#define VI0               A13       // Over-current switch
#define VI1               A12       //
#define VI2               A10       //
#define VI3               A11       //

#define CS0               10        // Chip select pins
#define CS1               9         //
#define CS2               20        //
#define CS3               21        //

#define LDAC              24        // Load DAC lines
//#define LDAC1             25        //
//#define LDAC2             26        //
//#define LDAC3             27        //

#define TRIG0             3         // Trigger pins
#define TRIG1             4         //
#define TRIG2             5         //
#define TRIG3             6         //

#define OC0               A8        // Voltage/current analog input pins
#define OC1               A3        //
#define OC2               A2        //
#define OC3               A1        //

#define MOSI              11        // SPI Bus
#define SCLK              14        //

#define OC_PERIOD_US      50000     // Overcurrent measurement period

// Function macros for setting bits in registers
#define cbi(sfr,bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr,bit) (_SFR_BYTE(sfr) |= _BV(bit))

// The number of channels supported by the library
static const int num_chan = 4;

// ADC -> milliamp conversion coeffecient
static const float mA_conv_coeff = 1000.0 * (3.3 / 1023.0);

// Maximal mA for each each channel
static const float max_mA = 1500.0;

// Each 'channel' defines a board address.  Corresponding pins numbers can be
// accessed through the look-up tables (+_vi_lut, _cs_lut, _trig_lut, etc).
typedef enum
{
    CH0 = 0,
    CH1,
    CH2,
    CH3,
} Channel;

class Cyclops {

public:
    Cyclops(Channel chan, float current_limit_mA = 1500);

    // Program the DAC output register
    int dac_prog_voltage(const uint16_t voltage) const;

    // Load the DAC output register to affect a voltage change on the  output
    // pin. This function affects all running cyclops devices that have had
    // their DAC's programmed using the dac_prog_voltage() function.
    static void dac_load(void);

    // Convenience method for programming and loading a DAC voltage. Equivalent
    // to calling dac_prog_voltage() followed by dac_load()
    int dac_load_voltage(const uint16_t voltage) const;

    // Use the DAC to generate a period waveform
    void dac_generate_waveform(const uint16_t voltage[],
                               const uint16_t length,
                               const uint32_t sample_period_us) const;

    // Current measurement
    inline float measure_current(void) const;

    // Attach/detach interrupt to TRIG pin
    void set_trigger(void (*user_func)(void), const int mode);

    // Call after creating all cyclops channels. This is used for all of them.
    static void begin(void);

    // *this's channel ID
    const Channel channel;

private:
    // Check and respond to over current situation periodically
    static void check_current(void);

    // Channel --> physical pin LUTs
    static const uint8_t *_vi_lut;
    static const uint8_t *_cs_lut;
    static const uint8_t *_oc_lut;
    static const uint8_t *_trig_lut;
    //static const uint8_t *_ldac_lut;
    static const uint8_t *_trig_port_pos_lut;
    static float *_oc_limit_lut;
};

#endif
