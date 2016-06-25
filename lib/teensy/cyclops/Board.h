/*
Copyright (c) 2015 Jon Newman (jpnewman ~at~ mit <dot> edu)
All right reserved.

This file is part of the Cyclops Library (CL) for Teensy 3.X.

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

* June 2016 Modified by Ananya Bahadur (ananya95@gmail.com)
  Ported CL::Cyclops.h to the Teenst 3.X device
*/

#ifndef CL_CYCLOPS_H
#define CL_CYCLOPS_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
#include "SPI_fifo.h"

#define DAC_CONF_ACTIVE       (0x1000)
#define DAC_CONF_SHDN         (0x1000)

// Pin Definitions
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
#define TRIG1             31    //
#define TRIG2             32    //
#define TRIG3             33    //

#define ADC0              A1    // Analog input lines
#define ADC1              A2    //
#define ADC2              A3    //
#define ADC3              A8    //

namespace cyclops{
  /**
   * @defgroup   ns-board board
   */
  namespace board{
  /**
   * @brief      Each 'channel' defines a board address.
   * @details
   * Corresponding pin numbers can be accessed through the look-up tables,
   * ``_a_in_lut``, ``_cs_lut``, ``_trig_lut`` and ``_ldac_lut``.
   * @ingroup    ns-board
   */
  enum Channel
  {
      CH0 = 0,
      CH1,
      CH2,
      CH3
  };

  /**
   * @brief      Determines the configuration the TRIG pin
   * @ingroup    ns-board
   */
  enum triggerConfig{
    NO_TRIG,              /**< Default */
    EXT_TRIG,             /**< External Interrupt enabled */
    SOFT_ON_LOAD,         /**< Invoke user method on each *LOAD* */
    SOFT_ON_WRITE_START   /**< Invoke user method on start of SPI write, but @ref spi-fifo-nb "beware!" */
  };
} // NAMESPACE board
using namespace board;
/**
 * @brief      Cyclops is a software abstraction of the digital components on the
 * PCB, chiefly the MCP4921 DAC.
 * @ingroup    ns-cyclops
 */
class Board {

 public:
    Channel channel;                  /**< The LED channel. */
    triggerConfig trig_config; /**< The configuration of the Trigger pin and soft/hardware interrupt. */
    void (*software_hook_fn)(void);   /**< The ``user-function`` which will be invoked if software hook is configured. */

    Board(Channel _channel);
    /**
     * @brief      Loads the (SPI) value buffered in MCP4921 into the DAC
     * @todo
     * Don't use ``digitalWrite()``
     * register
     */
    void dac_load();

    void dac_shutdown();
    /**
     * @brief      Writes the voltage to MCP4921, but does not *load* it!
     *
     * @param[in]  voltage  The voltage \f$\in [0, 4095]\f$
     */
    void dac_prog_voltage(uint16_t voltage);

    // Current measurement and OC protection
    
    /**
     * @brief      Returns the current flowing through the LED.
     *
     * @return     Unit: milli Ampere (mA)
     */
    float measureCurrent(void);

    /**
     * @brief      Kills the power to LED if current exceeds the given threshold.
     *
     * @param[in]  current_limit_mA  The upper limit (threshold) current *in mA*
     *
     * @return     The current through the LED *in mA*
     */
    float overCurrentProtect(float current_limit_mA);

    /**
     * @brief      Rigs the TRIG pin as an external interrupt pin in one of the
     *             [External Interrupt modes](http://playground.arduino.cc/Code/Interrupts)
     * @details
     * When the configured interrupt occurs, the ``user_func()`` will be invoked.
     * 
     * @todo
     * Testing required!
     *
     * @param[in]  user_func  The user function
     * @param[in]  mode       The interrupt mode
     */
    void uponExternalTrigger(void (*user_func)(void), int mode);

    /**
     * @brief      Rigs the system to call an arbitrary function upon events in
     * software
     * @warning
     * The ``user_func`` will block time-critical operations of the library and
     * must finish within few (1-3 μsec) of time. You are advised to check the
     * disassembly of the function.  
     * *Even innocent functions like ``digitalWrite()`` are decievingly slow!*
     * 
     * @details    Also @ref spi-fifo-nb "see" notes on SPI writes.
     *
     * @param[in]  user_func  The user function
     * @param[in]  tr_cfg     Options: ``SOFT_ON_LOAD`` and ``SOFT_ON_SPI_WRITE``
     */
    void setSoftwareHook(void (*user_func)(void), triggerConfig tr_cfg);

 private:

    // Over current triggred
    uint8_t _oc_trig;

    // Channel --> physical pin LUTs
    static const uint8_t *_a_in_lut;
    static const uint8_t *_cs_lut;
    static const uint8_t *_oc_lut;
    static const uint8_t *_trig_lut;
    static const uint8_t *_ldac_lut;
};

} // NAMESPACE cyclops

#endif