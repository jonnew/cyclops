#include "Source_t.h"

// static var definition
uint8_t Source::src_count = 0;

//========================
//  Source Abstract Class
//========================
Source::Source(operationMode mode, sourceStatus status /* = ACTIVE */) : 
	opMode(mode),
	status(status),
	src_id(++Source::src_count)
{
	shift_accumulator = 0;
}	

//========================
//  storedSource Class
//========================
storedSource::storedSource(
	const uint16_t *voltage_data,
	const double *hold_time_data,
	uint8_t sz,
	operationMode mode /* = LOOPBACK */
	) :	Source(mode),
		cur_ind(0),
		voltage_data(voltage_data),
		hold_time_data(hold_time_data),
		size(sz) { /*empty body*/ }

uint16_t storedSource::getVoltage(){
	return voltage_data[cur_ind];
}

double storedSource::holdTime(){
	if (opMode == ONE_SHOT && cur_ind == size-1)
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data[cur_ind];
}

void storedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (opMode == ONE_SHOT && cur_ind + step_sz >= size-1){
			cur_ind = size-1;
			status = FROZEN;
		}
		else
			cur_ind = (cur_ind + step_sz + shift_accumulator) % size;
		shift_accumulator = 0;
	}
}

void storedSource::reset(){
	cur_ind = 0;
}


//========================
//  generatedSource Class
//========================

generatedSource::generatedSource(
	uint16_t (*voltage_data_fn)(uint8_t),
	double (*hold_time_data_fn)(uint8_t),
	uint8_t sz,
	operationMode mode /* = LOOPBACK */
	) :	Source(mode),
		cur_ind(0),
		voltage_data_fn(voltage_data_fn),
		hold_time_data_fn(hold_time_data_fn),
		size(sz) { /*empty body*/ }

uint16_t generatedSource::getVoltage(){
	return voltage_data_fn(cur_ind);
}

double generatedSource::holdTime(){
	if (opMode == ONE_SHOT && cur_ind == size-1)
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data_fn(cur_ind);
}

void generatedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (opMode == ONE_SHOT && cur_ind + step_sz >= size-1){
			cur_ind = size-1;
			status = FROZEN;
		}
		else
			cur_ind = (cur_ind + step_sz) % size;
		shift_accumulator = 0;
	}
}

void generatedSource::reset(){
	cur_ind = 0;
}