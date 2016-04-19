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

typedef Waveform[] WaveformList;


/**
 * @brief      Each Waveform Object is assigned to a channel. Using the Source object, a Waveform generates or reads signal points.
 *             It can be in any one of 4 states.
 */
class Waveform{
 private:
    uint16_t time_rem;
    _status backup;
    void changeStatus(_status);
 public:
    Source *src;            /**< Pointer to a instantiated object derived from Source.
                              * @attention Source is an abstract class */
    uint8_t channel_id;
    _status state;
    _op_mode mode;          /**< This is the same as Source::mode, `LOOPBACK` or `ONE_SHOT`. */

    /**
     * @brief      Creates a Waveform object and assigns the prescribed `channel`.
     *
     * @param      Source*     Pointer to a virtual class, any derived class of Source can be used.
     */
    Waveform(uint8_t, Source*);

    /**
     * @brief      Invoked by the ISR
     */
    void markLatched();   
    void resume();
    void pause();
    void useSource(Source*);

    static void swapChannels(Waveform*, Waveform*);
};

#endif