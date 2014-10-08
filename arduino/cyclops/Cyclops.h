/*
Copyright (c) 2014 Jon Newman (jpnewman ~at~ mit <dot> edu) 
All right reserved.

This file is part of the Cyclops Library (CL) for Arduino.

CL is free software: you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
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

// Each 'channel' defines a board address.
// Corresponding pins numbers can be accessed through
// the look-up tables (_a_in_lut, _cs_lut, _trig_lut).
#define CH0 				0       	// Channels
#define CH1 				1       	//
#define CH2 				2       	//
#define CH3 				3       	//

// Pin Definitions
#define CS0 				3       	// Chip select pins
#define CS1 				4       	//
#define CS2 				5       	//
#define CS3 				6       	// 
#define TRIG0 				8     		// Trigger lines
#define TRIG1				9     		//
#define TRIG2 				10    		//
#define TRIG3 				11    		//
#define A0                  0           // Analog input lines
#define A1                  1           //
#define A2                  2           //
#define A3                  3           // 
#define WIPER_UD 			12 			// MCP4022 digital pot up/down pin
#define LDAC  				7      		// MCP4921 load DAC line (sync all channels)
#define OC_COMP  			2   		// Over-current compensation line

//MCP4921 commands
#define DAC_CONF_ACTIVE 	(0x1000)
#define DAC_CONF_SHDN 		(0x1000)

//MCP4022 
#define NOM_WIPER_POS       56          // This is the number of B --> A steps to get ~5k
                                        // between the W-A connection on the MCP4022

//Function macros for setting bits in registers
#define cbi(sfr,bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr,bit) (_SFR_BYTE(sfr) |= _BV(bit))

// Interupt handling stuff
typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr interupt_func[8] // One function pointer for each bit on PORTB
	= { NULL };
volatile static uint8_t interupt_last;		 // Last port state
static void isr( void ); 					 // Global interupt service routine	

class Cyclops {

  public:

    Cyclops( uint16_t channel );

    // Object properties
    uint16_t get_channel( void );
    uint16_t get_chip_select_pin( void );
    uint16_t get_trigger_pin( void );
    uint16_t get_analog_pin( void );

    // Onboard signal generation
	void mcp4921_send_test_waveform( void );
    void mcp4921_load_voltage( uint16_t voltage );
    void mcp4921_load_dac(void );
    void mcp4921_generate_waveform( int voltage[], uint16_t length, uint16_t sample_period_us );
	void mcp4921_shutdown_dac( void );
    
    // Current measurement and OC protection
    float measure_current( void );
    void over_current_protect( float current_limit_mA );
	
	// Attach/detach interupt
	void attach_interupt( void (*user_func)(void) );
	
    // Set/save input divider resistance
    void mcp4022_set_nom_AW_resistance( void );
    void mcp4022_decrement_pot( byte n );
    void mcp4022_increment_pot( byte n ); 
    void mcp4022_save_AW_resistance( void );
   
  private:
    
    // Low-level digital pot functions        
    void mcp4022_pulse_pot( byte n );
    void mcp4022_unpulse_pot( byte n );

	// Private properties
    uint16_t _channel;
	boolean _initialized;
    byte _wiper_position[4];

    // Channel --> physical pin LUTs
    static const uint16_t *_a_in_lut;
    static const uint16_t *_cs_lut;
    static const uint16_t *_trig_lut;
	static const uint16_t *_trig_port_pos_lut;

};

#endif



