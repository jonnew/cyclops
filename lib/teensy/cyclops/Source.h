/** @file Source.h
 * 
 * @page source-page Source Objects
 * 
 * @tableofcontents
 * 
 * @section source-overview Overview
 * Source objects are responsible for computing or fetching from a look-up table,
 * the sample point voltages and hold-times. Specialised Source classes, namely
 * 
 * 1. squareSource
 * 2. generatedSource
 * 3. storedSource
 * 
 * are optimised for generation of specific waveforms. These objects "store" your
 * stimulation signals. The siganls sample points are read off one-by-one to make 
 * a control signal which is fed to the analog circuitry on Cyclops.
 * Details of the timing scheme can be found @ref waveform-timing "here".
 * 
 * @note
 * Multiple ( _possibly same_ ) instances of a derived class can co-exist, they will 
 * be completely independent apart from the shared data source or technique. 
 * 
 * @section source-ranges The Control Signal
 * This is a voltage signal and the LED current is directly proportional to the 
 * amplitude of this signal. The Source objects encode the amplitude as an integer
 * between 0 and 4095 (2<sup>12</sup>).
 * 
 * The hold-time is the period for which the corresponding voltage level is held and is
 * encoded in \f$μsec\f$. The allowed range is \f$\in [20, 174762] μsec\f$.
 * 
 * @section source-tolerance Fault Tolerant Waveform generation 
 * Source objects keep track of the number of SPI updates missed. When updates are
 * missed, the waveform (and LED current) remains stagnant at the previous level. Since
 * "time" doesn't stop, Waveform behaves as if it performed the SPI write.
 *
 * If this is not done, this waveform would "lag" behind the intended waveform, and this 
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

namespace cyclops{
  /**
   * @defgroup   ns-source source
   */
  namespace source{
  /**
   * @ingroup    ns-source
   */
  enum operationMode{
      LOOPBACK,  /**< Run forever. */
      ONE_SHOT,  /**< Run one *sequence*, and then FREEZE. */
      N_SHOT     /**< Run \f$N\f$ *sequences*, and then FREEZE. */
  };
  }
using namespace source;

/**
 * @brief      Status of the *derivation* of Source.
 * @ingroup    ns-cyclops
 */
enum sourceStatus{
    FROZEN,   /**< stepForward calls will have no effect */
    ACTIVE    /**< *vice versa* */
};

/**
 * @brief      The type of *derivation*.
 * @ingroup    ns-cyclops
 */
enum sourceType{
    STORED,
    GENERATED,
    SQUARE
};

/**
 * @brief 
 * Source is an abstract class, representing a source of Voltage Data (as a 
 * number between 0 and 4095) and the corresponding Time Data (as μsec) for 
 * which the Voltage should be held.
 * @ingroup    ns-cyclops
 * See the @ref source-page "Source documentation".
 */
class Source{
 private:
    static uint8_t src_count; /**< Used to keep count of Source objects. */
 protected:
    uint8_t cycle_index;  /**< The no. of cycles completed. */
    uint16_t DCoffset;    /**< DC voltage offset from zero for any source. @sa Source::getVoltage*/
    float VScale,         /**< Voltage scaling factor. */
          TScale;         /**< Time scaling factor. */
 public:
    /** @brief
     *  Mode of operation. All derived sources support all variants.
     *  @details
     *  ``ONE_SHOT``: creates the waveform once and then gets FROZEN.
     *  
     *  ``N_SHOT``  : creates the waveform \f$N\f$ times and then gets FROZEN.
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
     *  The order in which _derivations_ of Source instances are defined
     *  determines the ID.
     */
    const uint8_t src_id;
    const sourceType name;

    uint8_t cycles; /**< Used in N_SHOT opMode */

    Source(sourceType _name, operationMode opMode, uint8_t _cycles);

    /**
     * @brief      Sets the Voltage Scale factor.
     *
     * @param[in]  vscale  The vscale factor
     */
    void setVScale(float vscale);

    /**
     * @brief      Sets the Time Scale factor.
     *
     * @param[in]  tscale  The tscale factor
     */
    void setTScale(float tscale);

    /**
     * @brief      Sets the DC offset for the control signal.
     * @sa         Source::getVoltage
     * @param[in]  dc_offset  The offset should be \f$\in [0, 4095]\f$ range
     */
    void setOffset(uint16_t dc_offset);

    /**
     * @brief      Returns the data-point's Voltage.
     * @details
     * The voltage is computed by fetching the Source's true voltage and multiplying
     * with the configured Source::VScale ad adding the DC offset.
     * \f[
     *      getVoltage = (trueVoltage \times VScale) + DCoffset
     * \f]
     * 
     * @note       This value is lastly "clamped" into the \f$[0, 4095]\f$ range by
     *             Waveform::prepare() by bitmasking.
     */
    virtual uint16_t getVoltage() = 0;
    /**
     * @brief      Returns the data-point's Hold-Time (in 1μsec). The voltage will be held on the LED for this much time.
     * @details
     * The hold-time is computed by fetching the Source's true hold-time and multiplying
     * with the configured Source::TScale.
     * Automatically clamps output to the \f$[0, 174762] (μsec)\f$ range.
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

/**
 * @brief      storedSource reads from an array of data-points.
 */
class storedSource: public Source{
 private:
    uint16_t cur_ind;
 public:
    const uint16_t *voltage_data;
    const uint32_t *hold_time_data;
    const uint16_t size;

    /**
     * @brief      Creates a new storedSource which reads data from an array of
     *             data-points.
     *
     * @param[in]  voltage_data    Pointer to (const) sequence of Waveform
     *                             Voltages
     * @param[in]  hold_time_data  Pointer to (const) sequence of Hold Times
     * @param[in]  sz              length of the sequence
     * @param[in]  mode            Operation mode of the waveform
     * @param[in]  _cycles         The cycles (in case of N_SHOT). ``1`` by default.
     */
    storedSource(const uint16_t *voltage_data,
                 const uint32_t *hold_time_data,
                        uint8_t sz,
                 operationMode  mode,
                        uint8_t _cycles = 1);
    virtual uint16_t getVoltage();
    virtual uint32_t holdTime();
    /**
     * @brief      Increments cur_ind if *sourceStatus* is ACTIVE
     * @details
     * Maintains cycle_index for N_SHOT mode. It is guaranteed that each call
     * will result in increase of cur_ind if *sourceStatus* is ACTIVE.
     * Upon _completion_ all cycles, the *sourceStatus* is set to FROZEN. *To reuse
     * the instance it must be reset atleast once.*
     * 
     * Nothing happens if *sourceStatus* is FROZEN.
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
 *             generate just 1 cycle. This source can be played in
 *             ``ONE_SHOT``, ``N_SHOT`` and ``LOOPBACK`` modes.
 *             The generating functions take a sequence_number as argument
 */
class generatedSource: public Source{
 private:
    uint16_t cur_ind;
 public:
    uint16_t (*voltage_data_fn)(uint8_t);
    uint32_t (*hold_time_data_fn)(uint8_t);
    uint32_t constHoldTime;
    uint16_t size;
    /**
     * @brief      Creates a new generatedSource which uses the provided
     *             function pointers to generate data-points.
     *
     * @param[in]  voltage_data_fn    Pointer to function which generates
     *                                Voltage of data-point
     * @param[in]  hold_time_data_fn  Pointer to function which generates Hold
     *                                Time of data-point
     * @param[in]  sz                 length of the sequence
     * @param[in]  mode               Operation mode of the waveform
     * @param[in]  _cycles            The cycles (in case of N_SHOT). ``1`` by default.
     */
    generatedSource(uint16_t      (*voltage_data_fn)(uint8_t),
                    uint32_t      (*hold_time_data_fn)(uint8_t),
                    uint8_t       sz,
                    operationMode mode,
                    uint8_t       _cycles = 1);

    /**
     * @brief      Creates a new generatedSource which uses the provided 
     *             function pointer to generate data-points which are a
     *             fixed-interval apart, the ``constHoldTime``.
     * @todo 
     * The class can possibly be redesigned so that there is no function call
     * in (this) case of ``constHoldTime``.
     * 
     * @param[in]  voltage_data_fn  Pointer to function which generates
     *                              Voltage of data-point
     * @param[in]  c_holdTime       The constHoldTime interval for each voltage
     * @param[in]  sz               length of the sequence
     * @param[in]  mode             Operation mode of the waveform
     * @param[in]  _cycles          The cycles. ``1`` by default.
     */
    generatedSource(uint16_t      (*voltage_data_fn)(uint8_t),
                    uint32_t      c_holdTime,
                    uint8_t       sz,
                    operationMode mode,
                    uint8_t       _cycles = 1);

    void setTimePeriod(uint32_t new_period);

    virtual uint16_t getVoltage();
    virtual uint32_t holdTime();
    /**
     * @brief      Increments cur_ind if *sourceStatus* is ACTIVE
     * @details
     * Maintains cycle_index for N_SHOT mode. It is guaranteed that each call
     * will result in increase of cur_ind if *sourceStatus* is ACTIVE.
     * Upon _completion_ all cycles, the *sourceStatus* is set to FROZEN. *To reuse
     * the instance it must be reset atleast once.
     * 
     * Nothing happens if *sourceStatus* is FROZEN.
     *
     * @param[in]  num_of_steps  The number of steps
     */
    virtual void     stepForward(uint8_t num_of_steps);
    virtual void     reset();
};


/**
 * @brief      squareSource generates _only_ square waves.
 * @details
 * All attributes are public, hence no need of "setters"
 */
class squareSource: public Source{
 private:
    uint8_t onFlag;
 public:
    uint16_t voltage_level[2];
    uint32_t level_time[2];

    /**
     * @brief      Creates a new squareSource which can be used to generate a
     *             square wave
     *
     * @param[in]  mode     Operation mode of the waveform
     * @param[in]  _cycles  The cycles (in case of N_SHOT). ``1`` by default.
     */
    squareSource(operationMode mode, uint8_t _cycles = 1);

    virtual uint16_t getVoltage();
    virtual uint32_t holdTime();
    /**
     * @brief      Increments cur_ind if *sourceStatus* is ACTIVE
     * @details
     * Maintains cycle_index for N_SHOT mode. It is guaranteed that each call
     * will result in increase of cur_ind if *sourceStatus* is ACTIVE.
     * Upon _completion_ all cycles, the *sourceStatus* is set to FROZEN. *To reuse
     * the instance it must be reset atleast once.*
     * 
     * Nothing happens if *sourceStatus* is FROZEN.
     *
     * @param[in]  num_of_steps  The number of steps
     */
    virtual void     stepForward(uint8_t num_of_steps);
    virtual void     reset();
};

namespace source{
/**
 * @ingroup    ns-source
 * @brief      The array of pointers to *derivations* of Source instances.
 * @attention
 * If RPC is to be used, this pointer must by defined (ie, initialised) in a 
 * user sketch. This array is used by Task::compute to access and modify the
 * Source attributes.
 */
extern cyclops::Source** globalSourceList_ptr;
}

} // NAMESPACE cyclops

#endif