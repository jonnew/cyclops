/** @file Waveform.h
  
  This library pre-instantiates a WaveformList object (``waveformList``). It is
  possible to use the library _without_ a WaveformList instance, see
  ``examples/pseudo_loop.ino``.
  
  See ``examples/main_loop.ino`` for intended usage. The basic idea is to:
  
  1. Create Source derivative instances, how many ever the application might
  require.
  2. Create as many Cyclops instances as the no. of LEDs to be controlled.

  @note Do the following inside ``setup()``!
  
  3. [Prepare the _waveform list_]
    - Use ``setWaveform()`` to bind the Cyclops and Source insances.
  
  4. Initialise ``nbSPI`` _(or even normal blocking SPI)_
  5. Call ``WaveformList::initialPrep()`` to write and latch the very first sample
  points
  6. Initialise ``Timer1`` with the return-value of ``initialPrep()``.
  7. Attach the **cyclops_timer_isr** to the Timer1 interrupt.
  
  @note Do the following inside ``loop()``!
  
  8. Call ``WaveformList::process()`` and forget about waveforms!
  
  @section spi_magic How does it work!?
  The **cyclops_timer_isr** and ``WaveformList::process()`` work together to deliver
  waveform updates with least possible jitter and highest possible accuracy.
  
  ``process()`` maintains an ordering of waveform instances based on the _time
  remaining for next update_. It aggressively issues SPI transfers, whenever the SPI
  device is available, to _PREPARE_ the waveforms.
  
  The ISR just latches the DAC programmed value at the right moment and marks the
  waveform LATCHED.
  
  @author Ananya Bahadur
*/
#ifndef CL_TEENSY_WAVEFORM_H
#define CL_TEENSY_WAVEFORM_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Source_t.h"
#include "TimerOne.h"
#include "Cyclops.h"

class Cyclops;

/** @typedef waveformStatus */
typedef enum {
    INIT,
    PREPARED,
    LATCHED
} waveformStatus;


/**
 * @brief      Each Waveform Object has pointers to a Cyclops instance and a Source
 *             instance, effectively binding them together.
 * @details    It can be in any one of 3 (waveformStatus) states:
 *             {``INIT``, ``PREPARED``, ``LATCHED``} 
 */
class Waveform{
 private:
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
     * run might not result in transition to ``PREPARED``, if the SPI FIFO is full.
     *
     * @return     Returns ``0`` if state transition to ``PREPARED`` else ``1``.
     *             If waveform is already ``PREPARED`` state when invoked, 
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

void cyclops_timer_isr();

#endif
