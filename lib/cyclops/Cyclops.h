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
#include "RPC_defs.h"
#ifdef CORE_TEENSY
    #include <SPI_fifo.h>
#else
    #include <SPI.h>
#endif

// Each 'channel' defines a board address.  Corresponding pins numbers can be
// accessed through the look-up tables (_a_in_lut, _cs_lut, _trig_lut, etc).
typedef enum
{
    CH0 = 0,
    CH1,
    CH2,
    CH3
} Channel;

#define DAC_CONF_ACTIVE       (0x1000)
#define DAC_CONF_SHDN         (0x1000)
#define DAC_UPDATE_DELAY_USEC 54 // LEONARDO SPECIFIC
#define DAC_BLOCK_SIZE        2  /**< The no. of bytes for each SPI transfer. HI:LO <=> {Command, 12b Data} */

// Pin Definitions
#ifndef CORE_TEENSY
  // For Arduino
  #define OC0               2     // Over-current switch
  #define OC1               3     //
  #define OC2               4     //
  #define OC3               5     //

  #define CS0               6     // Chip select pins
  #define CS1               7     //
  #define CS2               8     //
  #define CS3               9     //
 
  #define LDAC              10    // Load DAC line (sync all channels)

  #define TRIG0             11
  #define TRIG1             12
  #define TRIG2             SCL
  #define TRIG3             SDA

  #define ADC0              0     // Analog input lines
  #define ADC1              1     //
  #define ADC2              2     //
  #define ADC3              3     //

#else // For Teensy

  #define OC0               A10   // Over-current switch
  #define OC1               A11   //
  #define OC2               A12   //
  #define OC3               A13   //

  #define CS0               10    // Chip select pins
  #define CS1               9     //
  #define CS2               20    //
  #define CS3               21    //
/*  
  //#define LDAC0             24    // Load DAC line for CH0
  #define LDAC0             9     // Load DAC line (for Ananya's test rig only)
  #define LDAC1             25    // Load DAC line for CH1
  #define LDAC2             26    // Load DAC line for CH2
  #define LDAC3             27    // Load DAC line for CH3
*/
  #define LDAC0             16 
  #define LDAC1             17
  #define LDAC2             22
  #define LDAC3             23

  #define TRIG0             30    // Trigger lines
  #define TRIG1			      	31    //
  #define TRIG2 	       		32    //
  #define TRIG3             33    //
  
  #define ADC0              A1    // Analog input lines
  #define ADC1              A2    //
  #define ADC2              A3    //
  #define ADC3              A8    //

#endif
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
//static void isr(void);

class Cyclops {

 public:
    Channel channel;
    Cyclops(Channel _channel);

    #ifndef CORE_TEENSY
      /**
       * @brief      Activate the CS line
       * @deprecated CS is managed automatically by the Teensy SPI
       */
      void selectChip() __attribute__((always_inline));

      /**
       * @brief      Deactivate the CS line
       * @deprecated CS is managed automatically by the Teensy SPI
       */
      void deselectChip() __attribute__((always_inline));
    #endif

    // Onboard signal generation
    void dac_send_test_waveform(void);
    void dac_prog_voltage(uint16_t voltage);

    // These functions can be included once interrupt based SPI transfer is made
    // void dac_prog_initiate(uint16_t voltage);
    // bool isAvailable();

    void dac_load(void);
    void dac_generate_waveform(uint16_t voltage[],
            uint16_t length,
            uint32_t sample_period_us);
    void dac_shutdown(void);

    // Current measurement and OC protection
    float measureCurrent(void);
    float overCurrentProtect(float current_limit_mA);

    // Attach/detach interupt
    void attach_interrupt(void (*user_func)(void), int mode);

 private:

    // Over current triggred
    uint8_t _oc_trig;

    // Channel --> physical pin LUTs
    static const uint8_t *_a_in_lut;
    static const uint8_t *_cs_lut;
    static const uint8_t *_oc_lut;
    static const uint8_t *_trig_lut;
    static const uint8_t *_ldac_lut;
    static const uint8_t *_trig_port_pos_lut;
};

#endif
