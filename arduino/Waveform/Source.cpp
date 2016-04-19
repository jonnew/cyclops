#include "Source.h"

uint8_t Source::src_count = 0;
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
	uint8_t fetch_index;
	switch (mode){
	case LOOPBACK:
		fetch_index = (cur_ind == size-1)? 0 : cur_ind+1;
		break;
	case ONE_SHOT:
		return (cur_ind < size-1)? voltage_data[cur_ind+1] : 0;
		break;
	}
	return voltage_data[fetch_index];
}

uint16_t st_Source::holdTime(){
	uint8_t fetch_index;
	switch (mode){
	case LOOPBACK:
		fetch_index = (cur_ind == size-1)? 0 : cur_ind+1;
		break;
	case ONE_SHOT:
		return (cur_ind < size-1)? hold_time_data[cur_ind+1] : ONE_SHOT_FINISHED_HOLD_TIME;
		break;
	}
	return hold_time_data[fetch_index];
}

void st_Source::stepForward(uint8_t step_sz){
	cur_ind = (cur_ind + step_sz) % size;
}

gen_Source::gen_Source(
	uint16_t (*vdf)(uint8_t),
	uint16_t (*htdf)(uint8_t),
	uint8_t sz,
	_op_mode _mode /* = LOOPBACK */
	) :	Source(_mode),
		cur_ind(0),
		voltage_data(vdf),
		hold_time_data(htdf),
		size(sz) { /*empty body*/ }

uint16_t gen_Source::nextVoltage(){
	uint8_t fetch_index;
	switch (mode){
	case LOOPBACK:
		fetch_index = (cur_ind == size-1)? 0 : cur_ind+1;
		break;
	case ONE_SHOT:
		return (cur_ind < size-1)? voltage_data(cur_ind+1) : 0;
		break;
	}
	return voltage_data(fetch_index);
}

uint16_t gen_Source::holdTime(){
	uint8_t fetch_index;
	switch (mode){
	case LOOPBACK:
		fetch_index = (cur_ind == size-1)? 0 : cur_ind+1;
		break;
	case ONE_SHOT:
		return (cur_ind < size-1)? hold_time_data(cur_ind+1) : ONE_SHOT_FINISHED_HOLD_TIME;
		break;
	}
	return hold_time_data(fetch_index);
}

void gen_Source::stepForward(uint8_t step_sz){
	switch (mode){
	case LOOPBACK:
		cur_ind = (cur_ind + step_sz) % size;
		break;
	case ONE_SHOT:
		if (cur_ind < size-1) cur_ind++;
		break;	
	}
}