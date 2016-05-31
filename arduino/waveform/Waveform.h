/** @file Waveform.h
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
#include "TimerOne.h"

/** @typedef waveformStatus */
typedef enum {
    INIT,
    PREPARING,
    PREPARED,
    LATCHED
} waveformStatus;


/**
 * @brief      Each Waveform Object has pointers to a Cyclops instance and a Source instance, effectively binding them together.
 * @details    It can be in any one of 4 (waveformStatus) states: {``INIT``, ``PREPARING``, ``PREPARED``, ``LATCHED``} 
 */
class Waveform{
 private:
    uint8_t        outFrame[DAC_BLOCK_SIZE]; /**< The SPI frame is temporarily stored here. */
    uint8_t        outFrame_id;             /**< The index of element in the frame to be written next. */
    sourceStatus   backup_sourceStatus;
 public:
    Source         *source;  /**< Pointer to a instantiated object derived from
                              *   Source.
                              *   @attention Source is an abstract class
                              */
    Cyclops        *cyclops; /**< Pointer to a Cyclops instance. */
    waveformStatus status;   /**< Current "state" of the object. */
    double         time_rem;

    Waveform();

    /**
     * @brief      
     * Initialises ``state`` to `INIT`.
     *
     * @param[in]  _cyclops  Pointer to Cyclops
     * @param[in]  _source   Pointer to a *derivation* of Source
     */
    Waveform(Cyclops *_cyclops, Source* _source);

    /**
     * @brief      
     * Initialises ``state`` to ``INIT``. Sets the Source::opMode to ``mode``.
     *
     * @param[in] _cyclops  Pointer to Cyclops
     * @param[in] _source   Pointer to a *derivation* of Source
     * @param[in] mode      Ignore mode of source, use 'this' instead.
     */
    Waveform(Cyclops *_cyclops, Source* _source, operationMode mode);

    /**
     * @brief      
     * Sets the Source::opMode to ``mode`` apart from the obvious.
     *
     * @param[in] _cyclops  Pointer to Cyclops
     * @param[in] _source   Pointer to a *derivation* of Source
     * @param[in] mode      Ignore mode of source, use 'this' instead.
     */
    void setup(Cyclops *_cyclops, Source* _source, operationMode mode=LOOPBACK);

    /**
     * @brief
     * Moves the state from ``PREPARING`` to ``PREPARED`` and at outermost level.
     * 
     * @details
     * The scheduler calls this function if the waveform is ``PREPARING``. A Single
     * run might not result in transition to ``PREPARED``.
     *
     * @return     Returns ``0`` if state transition to ``PREPARED`` else ``1``.
     *             If waveform is not in ``PREPARING`` state when invoked, 
     *             imediately returns ``2``.
     */
    uint8_t prepare();

    /**
     * @brief
     * Loads Waveform::status from Waveform::backup_myStatus and sets 
     * source::status to ``ACTIVE``.
     */
    void resume();


    /**
     * @brief
     * Stores Waveform::status into Waveform::backup_myStatus and sets
     * source::status to ``PAUSED``.
     */
    void pause();

    /**
     * @brief      Replaces Waveform::source with new_source.
     *
     * @param      new_source  Pointer to *derivation* of Source
     */
    void useSource(Source* new_source);

    /**
     * @brief      Swaps the pointers to Cyclops-instances,
     *             w1->cyclops <-> w2->cyclops.
     *
     * @param      w1  Pointer to Waveform
     * @param      w2  Pointer to Waveform
     */
    static void swapChannels(Waveform* w1, Waveform* w2);
};

class WaveformList{
private:
    Waveform waveList[4];
    uint8_t _claimedChannels;
    uint8_t sortedWaveforms[4];
    uint8_t size;

    void updateSortedSeq() volatile;
public:
    WaveformList();
    double    initialPrep() volatile;
    int8_t    setWaveform(Cyclops *_cyclops, Source *_source, operationMode mode);
    Waveform* at(uint8_t index);

    int8_t    forthcoming(waveformStatus _status);
    int8_t    forthcoming(waveformStatus _status) volatile;
    Waveform* top() volatile;
    double    update_time_rem() volatile;

    /**
     * @brief
     * Called by the Task scheduler. Maintains waveforms.
     * @details
     * Performs SPI write for ``PREPARING`` waveforms (if SPI not ``_busy``)  
     * If full SPI frame has been written, marks the waveform ``PREPARED``  
     * Picks the forthcoming ``LATCHED`` and moves it into ``PREPARING``
     * _The scheduler can poll on this function too._
     *
     * @return     1 if any errors else 0
     */
    uint8_t process() volatile;
};

extern volatile WaveformList waveformList;

void cyclops_timer_ISR();

#endif