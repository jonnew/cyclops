// Jon Newman
// Cyclops Driver

/*
  Cyclops.h - Cyclops LED driver firmware.
  Created by Jon Newman.
  CC-BY-SA-4.0
*/

#ifndef Cyclops_h
#define Cyclops_h

#include "Arduino.h"

class Cyclops
{
    public:
        Cyclops();
        void setMaxCurrent(int channel, float maxCurrent_mAmps)        
        void attachTriggerWaveform(int channel, int[] waveform);
        void singleShotWaveform(int channel, int[] waveform);
        void flash(int channel, int duration_usec); 
    private:
        // Analog inputs to get current measures of each output channe
        int vI0, vI1, vI2, vI3;

        // 
 
  
}
#endif


