#include "Source.h"

Source::src_count = 0;
Source::Source(_op_mode _mode) : mode(_mode), src_id(++Source::src_count) {}

st_Source::st_Source(
	uint16_t *vd,
	uint16_t *htd,
	uint8_t  sz,
	_op_mode _mode /* = LOOPBACK */
	) :	Source(_mode),
		cur_ind(0),
		voltage_data(vd),
		hold_time_data(htd),
		size(sz) { /*empty body*/ }

uint16_t st_Source::nextVoltage(){
	switch (mode){
	case LOOPBACK:
		return (cur_ind == size-1) voltage_data[0] : voltage_data[cur_ind+1];
		break;
	case ONE_SHOT:
		return (cur_ind < size-1) : voltage_data[cur_ind+1] : 0;
		break;
	}
}

uint16_t st_Source::holdTime(){
	switch (mode){
	case LOOPBACK:
		return (cur_ind == size-1) hold_time_data[0] : hold_time_data[cur_ind+1];
		break;
	case ONE_SHOT:
		return (cur_ind < size-1) : hold_time_data[cur_ind+1] : ONE_SHOT_OVER_HOLD_TIME;
		break;
	}
}

void st_Source::stepForward(uint8_t step_sz){
	cur_ind = (cur_ind + step_sz) % size;
}

gen_Source::gen_Source(
	uint16_t (*vdf)(uint8_t);
	uint16_t (*htdf)(uint8_t);
	uint8_t  sz,
	_op_mode _mode /* = LOOPBACK */
	) :	Source(_mode),
		cur_ind(0),
		voltage_data(vdf),
		hold_time_data(htdf),
		size(sz) { /*empty body*/ }

uint16_t gen_Source::nextVoltage(){
	switch (mode){
	case LOOPBACK:
		return (cur_ind == size-1) voltage_data(0): voltage_data(cur_ind+1);
		break;
	case ONE_SHOT:
		return (cur_ind < size-1) : voltage_data(cur_ind+1) : 0;
		break;
	}
}

uint16_t gen_Source::holdTime(){
	switch (mode){
	case LOOPBACK:
		return (cur_ind == size-1) hold_time_data(0) : hold_time_data(cur_ind+1);
		break;
	case ONE_SHOT:
		return (cur_ind < size-1) : hold_time_data(cur_ind+1) : ONE_SHOT_OVER_HOLD_TIME;
		break;
	}
}

void gen_Source::stepForward(uint8_t step_sz){
	cur_ind = (cur_ind + step_sz) % size;
}