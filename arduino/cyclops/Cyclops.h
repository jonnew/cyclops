/*
 Cyclops.h - Cyclops driver header file
 Copyright (c) 2014 Jonathan Newman  All right reserved.

*/

#ifndef Cyclops_h
#define Cyclops_h

#include "Arduino.h"

#define CH1 3       // Chip select pins
#define CH2 4       //
#define CH3 5       //
#define CH4 6       //
#define TRIG1 8     // Trigger lines
#define TRIG2 9     //
#define TRIG3 10    //
#define TRIG4 11    //
#define WIPER_UD 12 // MCP4022 digital pot up/down pin
#define LDAC 7      // MCP4921 DAC SPI lines
#define DAC_CONF 0x
#define OC_COMP 2   // Over-current compensation line

class Cyclops
{
  public:
    Cyclops();
    //void load_waveform(int channel, int[] waveform);
    //void attach_waveform(int channel, int triggerline);
    //void single_shot(int channel, int voltage);
    //void pulse(int channel, int voltage, int duration_usec);
    void send_test_waveform(int);
  private:
    void update_dac(const short = 0x1000);
    int _v_out;
};

#endif



