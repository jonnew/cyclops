/** @file Waveform.h
  These functions can be included once interrupt based SPI transfer is available via Cyclops.
    @code{.cpp}

 void Waveform::prepare(){
    if (cyclops->isAvailable()){
        cyclops->dac_initiate_prog(src->nextVoltage()); // initiate will do chip selection. Non-blocking.
        status = PREPARED;
    }
}
  
Waveform::latch(uint16_t delta){
    if (state == PREPARED){
        cyclops->dac_load();
        state = LATCHED;
        // time_rem was zero, this channel just got latched.
        time_rem = src->holdTime() - delta;
    }
    src->stepForward(1);
}
    @endcode
    @author Ananya Bahadur
*/
#ifndef CL_WAVEFORM_H
#define CL_WAVEFORM_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"
#include "Source.h"

/** @typedef _status */
typedef enum {
    INIT,
    PREPARED,
    LATCHED,
    PAUSE
} _status;


/**
 * @brief      Each Waveform Object has a pointer to a Cyclops instance. Using the Source object, a Waveform generates or reads signal points.
 *             It can be in any one of 4 states.
 *             
 *             @see Waveform.h
 */
class Waveform{
 private:
    uint16_t time_rem;
    _status    backup_this;
    src_status backup_src;
 public:
    Source *src;            /**< Pointer to a instantiated object derived from Source.
                              * @attention Source is an abstract class */
    Cyclops *cyclops;       /**< Pointer to a Cyclops instance. */
    _status state;          /**< Current "state" of the object. */
    _op_mode mode;          /**< This is the same as Source::mode, `LOOPBACK` or `ONE_SHOT`. */

    /**
     * @brief      
     * Initialises `state` (and `backup_this`) to `INIT`. Copies mode from the Source.
     *
     * @param[in]  _cyclops  Pointer to Cyclops
     * @param[in]  s         Pointer to a *derivation* of Source
     */
    Waveform(Cyclops *_cyclops, Source* s);

    /**
     * @brief      
     * Initialises `state` (and `backup_this`) to `INIT`. Sets the Source::mode to `_mode`.
     *
     * @param[in]  _cyclops  Pointer to Cyclops
     * @param[in]  s         Pointer to a *derivation* of Source
     * @param[in] _mode      Ignore mode of src, use 'this' instead.
     */
    Waveform(Cyclops *_cyclops, Source* s, _op_mode _mode);
    
    /* These functions can be included once interrupt based SPI transfer is made
    void latch(uint16_t delta);
    void prepare();
    */


    /**
     * @brief      Loads Waveform::state from Waveform::backup_this and sets
     *             src::status to ACTIVE.
     */
    void resume();


    /**
     * @brief      Stores Waveform::state into Waveform::backup_this and sets
     *             src::status to PAUSED.
     */
    void pause();

    /**
     * @brief      Replaces Waveform::src with new_src.
     *
     * @param      new_src  Pointer to *derivation* of Source
     */
    void useSource(Source* new_src);

    /**
     * @brief      Swaps the pointers-to-Cyclops-instances.
     *
     * @param      w1  Pointer to Waveform
     * @param      w2  Pointer to Waveform
     */
    static void swapChannels(Waveform* w1, Waveform* w2);
};

#endif