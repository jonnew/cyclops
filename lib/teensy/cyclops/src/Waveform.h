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
  7. Attach the @ref ns-CyclopsLib "cyclops_timer_isr" to the Timer1 interrupt.
  
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

#include "Source.h"
#include "CLTimerOne.h"
#include "Board.h"

/**
 * @defgroup   ns-cyclops cyclops
 */
namespace cyclops{
using namespace source;

/**
 * @brief      The status of the Waveform instance, with respect to SPI *(and DAC)*.
 * @ingroup    ns-cyclops
 */
enum waveformStatus{
    INIT,      /**< The Waveform has not written to the DAC even once. */
    PREPARED,  /**< DAC has been prepared for *"next"* update "instant". */
    LATCHED    /**< The DAC was latched for a previous "instant", but is not ready for the next one. */
};

class Task;

/**
 * @ingroup    ns-cyclops
 * @brief      Latches the DAC and sets Timer1 period for next IRQ
 * @details
 * 
 * status     | source::status   | time_rem        | Action
 * ---------- | ---------------- | --------------- | -------
 * PREPARED   | ACTIVE           | < 10 \f$μsec\f$ | LDAC latch
 * \<any\>    | \<any\>          | < 10 \f$μsec\f$ | ``stepForward(1)`` <br>``time_rem = holdTime();``
 * 
 * Also computes the next timer period and updates all ``time_rem`` fields
 * 
 * @note       Sets the new Timer1 period.
 */
void cyclops_timer_isr();

/**
 * @brief      Each Waveform Object has pointers to a Cyclops instance and a Source
 *             instance, effectively binding them together.
 * @details    It can be in any one of 3 (waveformStatus) states:
 *             {``INIT``, ``PREPARED``, ``LATCHED``} 
 * @ingroup    ns-cyclops
 */
class Waveform{
 private:
    static Waveform* _list[4];
    static uint8_t   size;
    sourceStatus     backup_sourceStatus;
 public:
    Source     *source_ptr;  /**< Pointer to a instantiated object derived from
                              *   Source.
                              *   @attention Source is an abstract class
                              */
    Board        *board_ptr; /**< Pointer to a Board instance. */
    waveformStatus status;   /**< Current "state" of the object. */
    uint32_t       time_rem; /**< The amount of time remaining for the "next" update. */

    /**
     * @brief      Initialises ``state`` to ``INIT``. Sets the Source::opMode to
     *             ``mode``.
     *
     * @param[in]  _board_ptr   Pointer to Board
     * @param[in]  _source_ptr  Pointer to a *derivation* of Source
     * @param[in]  mode         Optional arg., LOOPBACK by default. Set
     *                          Source::mode as 'this'.
     * @param[in]  _cycles      The cycles for N_SHOT
     */
    Waveform(Board *_board_ptr, Source* _source_ptr, operationMode mode, uint8_t _cycles);

    /**
     * @brief      Initialises ``state`` to ``INIT``. Sets the Source::opMode to
     *             ``mode``.
     *
     * @param[in]  _board_ptr   Pointer to Board
     * @param[in]  _source_ptr  Pointer to a *derivation* of Source
     *
     * @details    Source::operationMode is set to LOOPBACK.
     */
    Waveform(Board *_board_ptr, Source* _source_ptr);

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
     * @param      new_source   Pointer to *derivation* of Source
     * @param[in]  _mode        The required operationMode of the new source
     * @param[in]  shot_cycles  The cycles for ``N_SHOT`` mode
     */
    void useSource(Source* new_source, operationMode _mode, uint8_t shot_cycles=1);

    /**
     * @brief      Swaps the pointers to Cyclops-instances,
     *             w1->cyclops <-> w2->cyclops.
     *
     * @param      w1  Pointer to Waveform
     * @param      w2  Pointer to Waveform
     */
    static void swapChannels(Waveform* w1, Waveform* w2);

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
    static uint32_t initAll();

    /**
     * @brief      Grounds all the signals.
     * @attention  There's no point in calling this during aquisition, because
     *             the DACs will be refreshed eventually. Call this after
     *             aquisition is stopped (and the Timer is stopped) to reliably
     *             turn off all LEDs.
     */
    static void groundAll();

    /**
     * @brief      The ISR routine which must be attached to Timer1 interrupt to
     * implement precise timing.
     */
    friend void cyclops::cyclops_timer_isr();

    /**
     * Task requires internal access to the class because of Task::compute
     */
    friend Task;
};

} // NAMESPACE cyclops
#endif
