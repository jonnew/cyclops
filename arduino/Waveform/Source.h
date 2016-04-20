#ifndef CL_SOURCE_H
#define CL_SOURCE_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "Cyclops.h"

/** @typedef _op_mode */
typedef enum {
    LOOPBACK,
    ONE_SHOT,
} _op_mode;

/** @typedef src_status */
typedef enum {
    PAUSED,
    ACTIVE
} src_status;

#define ONE_SHOT_FINISHED_HOLD_TIME 1000 /**< The waveform is completed.
                                          *   The timer would still interrupt for this Waveform, *as we don't "pause" the waveform*. Use a long HOLD_TIME. 
                                          */

class Source{
 protected:
    uint8_t shift_accumulator;
 public:
    static uint8_t src_count;
    _op_mode mode;
    src_status status;
    uint8_t src_id;

    Source(_op_mode _mode, src_status = ACTIVE);
    virtual uint16_t nextVoltage() = 0;
    virtual uint16_t holdTime() = 0;
    virtual void stepForward(uint8_t) = 0;
};

class st_Source: public Source{
 private:
    uint8_t cur_ind;
 public:
    uint16_t *voltage_data,
             *hold_time_data;
    uint8_t size;

    st_Source(uint16_t *vd, uint16_t *htd, uint8_t sz, _op_mode _mode = LOOPBACK);
    virtual uint16_t nextVoltage();
    virtual uint16_t holdTime();
    virtual void     stepForward(uint8_t step_sz);
};

class gen_Source: public Source{
 private:
    uint8_t cur_ind;
 public:
    uint16_t (*voltage_data)(uint8_t);
    uint16_t (*hold_time_data)(uint8_t);
    uint8_t size;

    gen_Source(uint16_t (*vdf)(uint8_t), uint16_t (*htdf)(uint8_t), uint8_t sz, _op_mode _mode = LOOPBACK);
    virtual uint16_t nextVoltage();
    virtual uint16_t holdTime();
    virtual void     stepForward(uint8_t step_sz);
};

#endif