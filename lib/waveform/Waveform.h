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
#ifndef CL_WAVEFORM_H
#define CL_WAVEFORM_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

//#include "Cyclops.h"
#include "Source.h"
#include "TimerOne.h"
#include "Cyclops.h"

class Cyclops;

/** @typedef waveformStatus */
typedef enum {
    INIT,
    PREPARING,
    PREPARED,
    LATCHED
} waveformStatus;

class WaveformList;

/**
 * @brief      Each Waveform Object has pointers to a Cyclops instance and a Source
 *             instance, effectively binding them together.
 * @details    It can be in any one of 4 (waveformStatus) states:
 *             {``INIT``, ``PREPARING``, ``PREPARED``, ``LATCHED``} 
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

/**
 * @brief      Makes the Waveform interface user-friendly.
 * @details
 * @see Waveform.h
 */

class WaveformList{
private:
    Waveform waveList[4];
    uint8_t _claimedChannels;
    uint8_t sortedWaveforms[4];
    uint8_t size;

    void updateSortedSeq();
public:
    /**
     * @brief      Creates 4 empty Waveforms internally.
     */
    WaveformList();

    /**
     * @brief      Binds the arguments to each other.
     * @details
     * Bind iff Cyclops Channel has not already been bound!
     * @warning    This **cannot** be used to _update_ a Waveform that has been
     *             setup previously.
     *
     * @param[in]  _cyclops  Pointer to a Cyclops instance
     * @param[in]  _source   Pointer to a *derivation* of Source
     * @param[in]  mode      Required operation-mode of the derivation* of Source
     *
     * @return     -2 if 4 Waveforms have already been setup.<br>
     *             -1 if the Cyclops Channel of ``_cyclops`` has already been bound
     *             in a different Waveform.<br>
     *             <id> ID# \f$\in [0-3]\f$ of the Waveform being set-up, if
     *             successfully completed.
     */
    int8_t    setWaveform(Cyclops *_cyclops, Source *_source, operationMode mode);

    /**
     * @brief      Performs the first Latching of all Waveforms.
     * @details
     * First prepares all the Waveforms and finds out the **forthcoming**
     * waveform, the one with least ``holdTime``.
     * 
     * It then latches them all together, and updates Waveform::time_rem
     * fields.
     *
     * @return     ``holdTime`` of the forthcoming Waveform.
     * @post
     * The Timer1 should be initialised _as soon as_ this function returns.
     */
    double    initialPrep();

    /**
     * @brief      Provides access to the Waveforms inside the list.
     * @details
     * Invalid accesses are rewarded by a ``NULL`` pointer.
     * 
     * @param[in]  index  The ID# that was provided by ``WaveformList::setWaveform()``
     *
     * @return     Pointer to the requested instance, or ``NULL``.
     */
    Waveform* at(uint8_t index);

    /**
     * @brief      Fetches the _index_ of the _first_ ACTIVE Waveform with given
     *             status.
     * @details
     * The Waveform with least ``time_rem`` fitting the search criteria.
     *
     * @param[in]  _status  ``waveformStatus`` of the target.
     *
     * @return     index of target or,<br>
     *             -1 if not found.
     */ 
    int8_t    forthcoming(waveformStatus _status);

    /**
     * @brief      Fetches Waveform with least ``time_rem`` regardless of state
     *
     * @return     Pointer to Waveform inside WaveformList
     */
    Waveform* top();

    /**
     * @brief      Determines which Waveform to latch in the next IRQ
     * @details
     * Updates the internal sorted-list, based on ``time_rem`` and then picks the
     * _first_ Waveform (regardless of it's ``waveformStatus``).
     * Also updates ``time_rem`` field of all Waveforms.
     * 
     * @see architecture
     * @note       This funtion is called **only from ISR context!**
     *
     * @return     The time period (in \f$\mu sec\f$) after which the next IRQ
     *             should occur.<br>
     */
    double    update_time_rem();

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
    uint8_t process();
};

extern WaveformList waveformList; /**< The _pre-instantiated_ WaveformList */

/**
 * @brief      Latches the DAC and sets Timer1 period for next IRQ
 * @details
 * Latches DAC only if the _top_ Waveform is PREPARED, but marks it LATCHED 
 * regardless. Calls ``Source::stepForward()`` and ``WaveformList::update_time_rem()``.
 * 
 * @note       Sets the new Timer1 period.
 */
void cyclops_timer_ISR();

#endif
