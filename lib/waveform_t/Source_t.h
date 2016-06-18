/** @file Source.h
 * 
 * @page source-page Source Objects
 * 
 * @tableofcontents
 * 
 * @section source-overview Overview
 * Source objects are responsible for computing or fetching from a look-up table,
 * the sample point voltages and hold-times. Specialised Source classes, namely
 * 1. squareSource
 * 2. generatedSource
 * 3. storedSource
 * are optimised for generation of specific waveforms. These objects "store" your
 * stimulation signals. The siganls sample points are read off one-by-one to make 
 * a control signal which is fed to the analog circuitry on Cyclops.
 * Details of the timing scheme can be found @ref waveform-timing "here".
 * 
 * @note
 * Multiple _(possibly same)_ instances of a derived class can co-exist, they will 
 * be completely independent apart from the shared data source or technique. 
 * 
 * @section source-ranges The Control Signal
 * This is a voltage signal and the LED current is directly proportional to the 
 * amplitude of this signal. The Source objects encode the amplitude as an integer
 * between 0 and 4095 (2<sup>12</sup>).
 * 
 * The hold-time is the period for which the corresponding voltage level is held and is
 * encoded in ``μsec``. The allowed range is \f$\in [20, 174762] μsec\f$.
 * 
 * @section source-tolerance Fault Tolerant Waveform generation 
 * Source objects keep track of the number of SPI updates missed. When updates are
 * missed, the waveform (and LED current) remains stagnant at the previous level. Since
 * "time" doesn't stop, Waveform behaves as if it performed the SPI write. If this is not
 * done, this waveform would "lag" behind the intended waveform, and this 
 * lag would accumulate until the Source is FROZEN or reset.
 * 
 * @author Ananya Bahadur
*/

#ifndef CL_SOURCE_H
#define CL_SOURCE_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

/** @typedef operationMode */
typedef enum {
    LOOPBACK,
    ONE_SHOT,
    N_SHOT
} operationMode;

/** @typedef sourceStatus */
typedef enum {
    FROZEN,
    ACTIVE
} sourceStatus;

#define ONE_SHOT_FINISHED_HOLD_TIME 1000 /**< The waveform is completed.  
                                          *   The timer would still interrupt
                                          *   for this Waveform, as we don't
                                          *   "pause" the waveform*. Use a long
                                          *   HOLD_TIME.
                                          */

/**
 * @brief 
 * Source is an abstract class, representing a source of Voltage Data (as a 
 * number between 0 and 4095) and the corresponding Time Data (as μsec) for 
 * which the Voltage should be held.
 * 
 * @sa         source-page       
 */
class Source{
 protected:
    uint8_t cycle_index;
 public:
    static uint8_t src_count;
    /** @brief
     *  Mode of operation. All derived sources support all variants.
     *  @details
     *  ``ONE_SHOT``: creates the waveform once and then gets FROZEN.
     *  
     *  ``LOOPBACK``: loops back to the starting to create a periodic waveform.
    */
    operationMode opMode;
    /** @brief
     *  State of the Source
     *  @details
     *  In ``FROZEN`` state stepForward() calls do nothing.
    */    
    sourceStatus status;
    /** @brief Unique uint ID for this object
     *  @details
     *  The order in which _derivations_ of Source instances determines the ID
     */
    const uint8_t src_id;

    uint8_t cycles; /**< Used in N_SHOT opMode */

    Source(operationMode opMode, uint8_t _cycles);

    /**
     * @brief      Returns the data-point's Voltage.
     * @details
     * The voltage is computed by fetching the Source's true voltage and multiplying
     * with the configured Source::VScale.
     * Automatically clamps output to the \f$[0, 4095]\f$ range.
     */
    virtual uint16_t getVoltage() = 0;
    /**
     * @brief      Returns the data-point's Hold-Time (in 1μsec). The voltage will be held on the LED for this much time.
     * @details
     * The hold-time is computed by fetching the Source's true hold-time and multiplying
     * with the configured Source::TScale.
     * Automatically clamps output to the \f$[0, 174]\f$ range.
     */
    virtual uint32_t holdTime() = 0;
    /**
     * @brief      Moves to the next data-point.
     * 
     * @param[in]   num_of_steps Moves `num_of_steps` ahead into the waveform.
     */
    virtual void stepForward(uint8_t num_of_steps) = 0;
    /**
     * @brief      Resets the Source. getVoltage() will return the Voltage of the first data-point.
     */
    virtual void reset() = 0;
};

extern Source** globalSourceList_ptr;

/**
 * @brief      storedSource reads from an array of data-points.
 */
class storedSource: public Source{
 private:
    uint8_t cur_ind;
 public:
    const uint16_t *voltage_data;
    const uint32_t *hold_time_data;
    const uint8_t size;

    /**
     * @brief      Creates a new storedSource which reads data from an array of data-points.
     *
     * @param[in]  voltage_data    Pointer to (const) sequence of Waveform Voltages
     * @param[in]  hold_time_data  Pointer to (const) sequence of Hold Times
     * @param[in]  sz              length of the sequence
     * @param[in]  mode            ``LOOPBACK`` by default
     */
    storedSource(const uint16_t *voltage_data,
                 const uint32_t *hold_time_data,
                        uint8_t sz,
                 operationMode  mode,
                        uint8_t _cycles = 1);
    virtual uint16_t getVoltage();
    virtual uint32_t holdTime();
    /**
     * @brief      Increments cur_ind if Source::sourceStatus is ACTIVE
     * @details
     * Maintains cycle_index for N_SHOT mode. It is guaranteed that each call
     * will result in increase of cur_ind if sourceStatus is ACTIVE.
     * Upon _completion_ all cycles, the sourceStatus is set to FROZEN. *To reuse
     * the instance it must be reset atleast once.*
     * 
     * Nothing happens if sourceStatus is FROZEN.
     *
     * @param[in]  num_of_steps  The number of steps
     */
    virtual void     stepForward(uint8_t num_of_steps);
    virtual void     reset();
};

/**
 * @brief      generatedSource creates data-points from a mathematical
 *             expressions in a function.
 * @details    The waveform is assumed to be cyclic and the function should
 *             generate just 1 cycle. Hence this source can be played in
 *             ``ONE_SHOT`` and ``LOOPBACK`` modes.
 *             The functions take a sequence_number as argument
 */
class generatedSource: public Source{
 private:
    uint8_t cur_ind;
 public:
    uint16_t (*voltage_data_fn)(uint8_t);
    uint32_t (*hold_time_data_fn)(uint8_t);
    uint8_t size;
    /**
     * @brief      Creates a new generatedSource which uses the provided 
     *             function pointers to denerate data-points.
     *
     * @param[in]  voltage_data_fn    Pointer to function which generates 
     *                                Voltage of data-point
     * @param[in]  hold_time_data_fn  Pointer to function which generates 
     *                                Hold Time of data-point
     * @param[in]  sz                 Length of one cycle of the waveform
     * @param[in]  mode               ``LOOPBACK`` by deafult
     */
    generatedSource(uint16_t      (*voltage_data_fn)(uint8_t),
                    uint32_t      (*hold_time_data_fn)(uint8_t),
                    uint8_t       sz,
                    operationMode mode,
                    uint8_t       _cycles = 1);
    virtual uint16_t getVoltage();
    virtual uint32_t holdTime();
    /**
     * @brief      Increments cur_ind if Source::sourceStatus is ACTIVE
     * @details
     * Maintains cycle_index for N_SHOT mode. It is guaranteed that each call
     * will result in increase of cur_ind if sourceStatus is ACTIVE.
     * Upon _completion_ all cycles, the sourceStatus is set to FROZEN. *To reuse
     * the instance it must be reset atleast once.*
     * 
     * Nothing happens if sourceStatus is FROZEN.
     *
     * @param[in]  num_of_steps  The number of steps
     */
    virtual void     stepForward(uint8_t step_sz);
    virtual void     reset();
};

#endif