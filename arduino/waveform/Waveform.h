#ifndef CL_WAVEFORM_H
#define CL_WAVEFORM_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"
#include "Source.h"

typedef enum {
    INIT,
    PREPARED,
    LATCHED,
    PAUSE
} _status;

class Waveform{
 private:
    uint16_t time_rem;
    uint8_t step;

    void changeStatus(_status new_status);
 public:
    Source *src;
    uint8_t channel_id;
    _status state;
    _op_mode mode;

    Waveform(uint8_t chid, Source &s);

    void stepForward(uint8_t);
    void markLatched();
    
    void resume();
    void pause();
    void useSource(Source &s);

    void changeChannel(uint8_t chid);
};

#endif