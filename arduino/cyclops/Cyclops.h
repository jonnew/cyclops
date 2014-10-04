/*
 Cyclops.h - Cyclops driver header file
 Copyright (c) 2014 Jonathan Newman  All right reserved.

*/

#ifndef Cyclops_h
#define Cyclops_h

#include <Arduino.h>
#include <SPI.h>
//#include <avr/pgmspace.h>

// Pin Definitions
#define CH1 				3       	// Chip select pins
#define CH2 				4       	//
#define CH3 				5       	//
#define CH4 				6       	//
#define TRIG1 				8     		// Trigger lines
#define TRIG2				9     		//
#define TRIG3 				10    		//
#define TRIG4 				11    		//
#define WIPER_UD 			12 			// MCP4022 digital pot up/down pin
#define LDAC  				7      		// MCP4921 DAC SPI lines
#define OC_COMP  			2   		// Over-current compensation line

//MCP4921 commands
#define DAC_CONF_ACTIVE 	(0x1000)
#define DAC_CONF_SHDN 		(0x1000)

class Cyclops {
  public:
    Cyclops();
	void send_test_waveform(uint16_t chan);
	void shutdown_dac(uint16_t chan);
    
	//void load_waveform(int channel, int[] waveform);
    //void attach_waveform(int channel, int triggerline);
    //void single_shot(int channel, int voltage);
    //void pulse(int channel, int voltage, int duration_usec);

  private:
    void update_dac(uint16_t chan);
	//void initialize(void);
    int _v_out;
	boolean _initialized;
};

#endif



