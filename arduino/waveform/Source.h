/** @file Source.h
  
    @author Ananya Bahadur
*/

#ifndef CL_SOURCE_H
#define CL_SOURCE_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"

/** @typedef operationMode */
typedef enum {
    LOOPBACK,
    ONE_SHOT,
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
 * number between 0 and 4095) and the corresponding Time Data (as msec) for 
 * which the Voltage should be held.
 * 
 * @details
 * Multiple instances of a derived class can co-exist, they will be completely
 * independent apart from the shared data source or technique.             
 */
class Source{
 protected:
    /** Keeps track of the number of SPI updates missed. When updates are
     *  missed, the waveform remains stagnant. Since "time" doesn't stop,
     *  Cyclops should behave as if it performed the SPI write. If this is not
     *  done, this waveform would "lag" behind the intended waveform, and this 
     *  lag would accumulate until FROZEN or reset.
    */
    uint8_t shift_accumulator;
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
    const uint8_t src_id; /**< Unique uint ID for this object */

    Source(operationMode opMode, sourceStatus = ACTIVE);

    /**
     * @brief      Returns the next data-point's Voltage.
     */
    virtual uint16_t getVoltage() = 0;
    /**
     * @brief      Returns the next data-point's Hold-Time (in 10μsec). The voltage will be held on the LED for this much time.
     */
    virtual uint16_t holdTime() = 0;
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

/**
 * @brief      storedSource reads from an array of data-points.
 */
class storedSource: public Source{
 private:
    uint8_t cur_ind;
 public:
    const uint16_t *voltage_data,
                   *hold_time_data;
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
                 const uint16_t *hold_time_data,
                 uint8_t sz,
                 operationMode mode = LOOPBACK);
    virtual uint16_t getVoltage();
    virtual uint16_t holdTime();
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
    uint16_t (*hold_time_data_fn)(uint8_t);
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
    generatedSource(uint16_t (*voltage_data_fn)(uint8_t),
                    uint16_t (*hold_time_data_fn)(uint8_t),
                    uint8_t sz,
                    operationMode mode = LOOPBACK);
    virtual uint16_t getVoltage();
    virtual uint16_t holdTime();
    virtual void     stepForward(uint8_t step_sz);
    virtual void     reset();
};

#endif