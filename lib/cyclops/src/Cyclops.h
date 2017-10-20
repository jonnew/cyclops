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

#define VI0               A13       // Voltage/current analog input pins
#define VI1               A12       //
#define VI2               A10       //
#define VI3               A11       //

#define CS0               10        // Chip select pins
#define CS1               9         //
#define CS2               20        //
#define CS3               21        //

#define LDAC              24        // Load DAC lines
#define LDAC1             25        //
#define LDAC2             26        //
#define LDAC3             27        //

#define TRIG0             3         // Trigger pins
#define TRIG1             4         //
#define TRIG2             5         //
#define TRIG3             6         //

#define OC0               A8        // Over-current switch
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

// Each 'channel' defines a board address. A single microcontroller supports up
// to 4 stacked Cyclops boards.
// Corresponding pins numbers can be
// accessed through the look-up tables (+_vi_lut, _cs_lut, _trig_lut, etc).
typedef enum
{
    CH0 = 0,
    CH1,
    CH2,
    CH3,
} Channel;

// One Cyclops object should be created for each LED channel. For single LED
// devices (most cases) only CH0 is used.
class Cyclops {

public:
    // Construct a Cyclops object
    //   chan: Cyclops channel to control
    //   current_limit_mA: optional current limit for this Cyclops channel
    Cyclops(Channel chan, float current_limit_mA = 1500);

    // Program the DAC output register
    //    voltage: 12-bit integer (0-4095) specify the voltage to program the
    //              DAC with. Voltage is scaled into a 0-5V range.
    //    returns: 0 on success, 1 otherwise.
    int dac_prog_voltage(const uint16_t voltage) const;

    // Load the DAC output register to affect a voltage change on the output
    // pin. This function affects all running cyclops devices that have had
    // their DAC's programmed using the dac_prog_voltage() function.
    static void dac_load(void);

    // Convenience method for programming and loading a DAC voltage. Equivalent
    // to calling dac_prog_voltage() followed by dac_load()
    //    voltage: 12-bit integer (0-4095) specify the voltage to program the
    //              DAC with. Voltage is scaled into a 0-5V range.
    //    returns: 0 on success, 1 otherwise.
    int dac_load_voltage(const uint16_t voltage) const;

    // Use the DAC to generate a period waveform
    //    voltage: Pointer to array of 12-bit integers (0-4095) specify the
    //             voltage sequence to program the DAC with. Voltage is scaled
    //             into a 0-5V range.
    //    length: Length of voltage sequence.
    //    sample_period_us: Sample period of voltage sequence in microseconds.
    void dac_generate_waveform(const uint16_t voltage[],
                               const uint16_t length,
                               const uint32_t sample_period_us) const;

    // Get an LED current measurement in milliamps
    //    returns: LED current measurement in milliamps.
    inline float measure_current(void) const;

    // Attach/detach user provided interrupt function to TRIG pin
    // mode indicates the pin trasition state at which the function is called,
    // RISING, FALLING or CHANGED
    //    user_func: User define function specifying action to take when the
    //               TRIG pin transtions according the the mode arguement. e.g.
    //               can be a function that generates a wavefomr using
    //               dac_load_voltage() or similar.
    //    mode: indicates the TRIG trasition state at which the user_func is
    //          called. Can be either RISING, FALLING or CHANGED.
    void set_trigger(void (*user_func)(void), const int mode);

    // Call this static method after creating all cyclops channels to start the
    // internal overcurent protection engine and waveform generation machinery.
    // This funciton is used to start all channels.
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
