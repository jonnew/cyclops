/*
 Cyclops.h - Cyclops driver header file
 Copyright (c) 2014 Jonathan Newman  All right reserved.

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
//#include <avr/pgmspace.h>

// Each 'channel' each defines a board address.
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

class Cyclops {
  public:
    Cyclops();

    // Onboard signal generation
	void mcp4921_send_test_waveform(uint16_t chan);
    void mcp4921_single_shot(uint16_t chan, uint16_t voltage);
    void mcp4921_generate_waveform(uint16_t chan, int voltage[], uint16_t length, uint16_t sample_period_us);
	void mcp4921_shutdown_dac(uint16_t chan);
    
    // Current measurement and OC protection
    float measure_current(uint16_t chan);
    void over_current_protect(uint16_t chan, float current_limit_mA);
    
    // Set/save input divider resistance
    void mcp4022_set_nom_AWR(uint16_t chan);
    void mcp4022_decrement_pot(uint16_t chan, byte n);
    void mcp4022_increment_pot(uint16_t chan, byte n); 
    void mcp4022_save_pot_resistance(uint16_t chan);
   
  private:
    void mcp4921_load_data(uint16_t chan, uint16_t voltage);
    void mcp4921_load_dac(void);
    void mcp4022_pulse_pot(byte n);
    void mcp4022_unpulse_pot(byte n);
    uint16_t _v_out;
    float _current_mA;
	boolean _initialized;
    byte _wiper_position[4];

    // Channel --> physical pin LUTs
    static const uint16_t *_a_in_lut;
    static const uint16_t *_cs_lut;


};

#endif



