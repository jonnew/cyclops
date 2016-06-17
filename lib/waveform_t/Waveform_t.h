/** @file Waveform.h
  
  @page teensy-waveform Using the Waveform Object

  @tableofcontents

  This library can be used as is (in cusom sketches) or with the Open Ephys GUI
  plugin.
  @sa Waveform

  @section t-waveform-intended-usage Intended Usage
  See ``src/teensy/wave2_teensy.ino`` for intended usage. The basic idea is to:
  
  1. Create Source derivative instances, how many ever the application might
     require.
  2. Create as many Cyclops instances as the no. of LEDs to be controlled.
  @note       A Cyclops instance cannot drive the LED on it's own.
  3. Create as many Waveform Objects as the no. of LEDs. Each Waveform instance
     binds Cyclops and Source instances.

  @note Do the following inside ``setup()``!
  
  4. Initialise ``SPI_fifo`` with upto 16MHz SPI bus speed.
  5. Call ``Waveform::initAll()`` to write and latch the very first sample
     points
  6. Initialise ``Timer1`` with the return-value of ``initAll()``.
  7. Attach the **cyclops_timer_isr** to the Timer1 interrupt.
  
  @note       Do the following inside ``loop()``!
  
  8. Call ``Waveform::processAll()`` and forget about waveforms!
  
  @section spi_magic How does it work!?
  The **cyclops_timer_isr** and ``Waveform::processAll()`` work together to deliver
  waveform updates with least possible jitter and highest possible accuracy.
  
  ``processAll()`` aggressively issues SPI transfers, whenever the SPI device is 
  available, to _PREPARE_ the waveforms.    
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
    static Waveform* _list[4];
    static uint8_t   size;
    sourceStatus     backup_sourceStatus;
 public:
    Source         *source;  /**< Pointer to a instantiated object derived from
                              *   Source.
                              *   @attention Source is an abstract class
                              */
    Cyclops        *cyclops; /**< Pointer to a Cyclops instance. */
    waveformStatus status;   /**< Current "state" of the object. */
    double         time_rem;

    /**
     * @brief      
     * Initialises ``state`` to ``INIT``. Sets the Source::opMode to ``mode``.
     *
     * @param[in] _cyclops  Pointer to Cyclops
     * @param[in] _source   Pointer to a *derivation* of Source
     * @param[in] mode      Optional arg., LOOPBACK by default. Set Source::mode
     *                      as 'this'.
     */
    Waveform(Cyclops *_cyclops, Source* _source, operationMode mode=LOOPBACK);

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
    inline void resume() __attribute__((always_inline));


    /**
     * @brief
     * Stores Waveform::status into Waveform::backup_myStatus and sets
     * source::status to ``PAUSED``.
     */
    inline void pause() __attribute__((always_inline));

    /**
     * @brief      Replaces Waveform::source with new_source.
     *
     * @param      new_source  Pointer to *derivation* of Source
     */
    inline void useSource(Source* new_source) __attribute__((always_inline));

    /**
     * @brief      Swaps the pointers to Cyclops-instances,
     *             w1->cyclops <-> w2->cyclops.
     *
     * @param      w1  Pointer to Waveform
     * @param      w2  Pointer to Waveform
     */
    inline static void swapChannels(Waveform* w1, Waveform* w2) __attribute__((always_inline));

    /**
     * @brief      Agressively PREPAREs Waveforms which have been latched.
     * @details
     * Non-blocking function. Just consists of <#-of-channels> no. of
     * ``Waveform::prepare()`` calls.
     * 
     * @note       This function is protected against Timer1 Interrupts.
     */
    static void processAll();

    /**
     * @brief      Perform the first SPI write and latching for all channels
     * 
     * @return     ``holdTime`` of the Waveform with least ``holdTime``.
     */
    static double initAll();

    friend void cyclops_timer_isr();
};

/**
 * @brief      Latches the DAC and sets Timer1 period for next IRQ
 * @details
 * 
 * status   | source::status | time_rem       | Action
 * ------   | -------------- | --------       | -------
 * PREPARED | ACTIVE         | < 2 \f$μsec\f$ | LDAC latch
 * <any>    | <any>          | < 2 \f$μsec\f$ | ``stepForward(1)`` <br>``time_rem = holdTime();``
 * 
 * Also computes the next timer period and updates all ``time_rem`` fields
 * 
 * @note       Sets the new Timer1 period.
 */
void cyclops_timer_isr();

#endif
