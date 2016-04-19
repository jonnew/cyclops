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
#include <Wire.h>

#include "RPC_defs.h"

// Each 'channel' defines a board address.  Corresponding pins numbers can be
// accessed through the look-up tables (_a_in_lut, _cs_lut, _trig_lut, etc).
typedef enum
{
    CH0 = 0,
    CH1,
    CH2,
    CH3
} Channel;

// Pin Definitions
#define OC0 				2     // Over-current switch
#define OC1 				3     //
#define OC2 				4     //
#define OC3 				5     //

#define CS0 				6     // Chip select pins
#define CS1 				7     //
#define CS2 				8     //
#define CS3 				9     //
#define LDAC  				10    // Load DAC line (sync all channels)

#define TRIG0 				11    // Trigger lines
#define TRIG1				12    //
#define TRIG2 				SCL   //
#define TRIG3 				SDA   //

#define A0                  0     // Analog input lines
#define A1                  1     //
#define A2                  2     //
#define A3                  3     //

//MCP4921 stuff
#define DAC_CONF_ACTIVE 	  (0x1000)
#define DAC_CONF_SHDN 		  (0x1000)
#define DAC_UPDATE_DELAY_USEC 54 // LEONARDO SPECIFIC

//Function macros for setting bits in registers
#define cbi(sfr,bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr,bit) (_SFR_BYTE(sfr) |= _BV(bit))

// Interupt handling stuff
typedef void (*voidFuncPtr)(void);

// One function pointer for each bit on PORTB
volatile static voidFuncPtr interupt_func[8] = { NULL };

// Last port state
volatile static uint8_t interupt_last;

// Global interupt service routine
static void isr(void);

class Cyclops {

 public:

    Cyclops(Channel channel);

    // Onboard signal generation
    void dac_send_test_waveform(void);
    void dac_prog_voltage(uint16_t voltage);
    void dac_load(void);
    void dac_generate_waveform(uint16_t voltage[],
            uint16_t length,
            uint32_t sample_period_us);
    void dac_shutdown(void);

    // Current measurement and OC protection
    float measure_current(void);
    float over_current_protect(float current_limit_mA);

    // Attach/detach interupt
    void attach_interupt(void (*user_func)(void));

 private:

    // Private properties
    Channel _channel = CH0;

    // Over current triggred
    uint8_t _oc_trig = 0;

    // Channel --> physical pin LUTs
    static const uint8_t *_a_in_lut;
    static const uint8_t *_cs_lut;
    static const uint8_t *_oc_lut;
    static const uint8_t *_trig_lut;
    static const uint8_t *_trig_port_pos_lut;
};

#endif
