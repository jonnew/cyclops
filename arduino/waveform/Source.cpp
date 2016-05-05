#include "Source.h"

// static var definition
uint8_t Source::src_count = 0;

Source::Source(_op_mode _mode, src_status _status /* = ACTIVE */) : 
	mode(_mode),
	status(_status)
{
	src_id = ++Source::src_count,
	shift_accumulator = 0;
}	


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
	return voltage_data[cur_ind];
}

uint16_t st_Source::holdTime(){
	if (mode == ONE_SHOT && cur_ind == size-1)
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data[cur_ind];
}

void st_Source::stepForward(uint8_t step_sz){
	if (status == PAUSED){
		shift_accumulator += step_sz;
		return;
	}
	else{
		if (mode == ONE_SHOT && cur_ind + step_sz >= size-1)
			cur_ind = size-1;
		else
			cur_ind = (cur_ind + step_sz + shift_accumulator) % size;
		shift_accumulator = 0;
	}
}

void st_Source::reset(){
	cur_ind = 0;
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
	return voltage_data(cur_ind);
}

uint16_t gen_Source::holdTime(){
	if (mode == ONE_SHOT && cur_ind == size-1)
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data(cur_ind);
}

void gen_Source::stepForward(uint8_t step_sz){
	if (status == PAUSED){
		shift_accumulator += step_sz;
		return;
	}
	else{
		if (mode == ONE_SHOT && cur_ind + step_sz >= size-1)
			cur_ind = size-1;
		else
			cur_ind = (cur_ind + step_sz) % size;
		shift_accumulator = 0;
	}
}

void gen_Source::reset(){
	cur_ind = 0;
}