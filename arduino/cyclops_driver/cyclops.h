#ifndef cyclops_h
#define cyclops_h

#include "Arduino.h"

class cyclops
{
  public:
    cyclops(int pin);
    void load_waveform(int channel, int[] waveform);
    void trigger_waveform(int channel);
    void single_shot(int channel, int voltage);
    void pulse(int channel, int voltage, int duration_usec);
    void dash();
  private:
    void serialEvent();
    void update_dac();
    void mcp_write();
    int _pin;
    
    // Pin defs
    const int 
};

#endif



