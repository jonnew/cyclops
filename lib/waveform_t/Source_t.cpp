#include "Source_t.h"

// static var definition
uint8_t Source::src_count = 0;

//========================
//  Source Abstract Class
//========================
Source::Source(operationMode mode, uint8_t _cycles) : 
	opMode(mode),
	status(ACTIVE),
	src_id(++Source::src_count),
	cycles(_cycles)
{
	cycle_index = 0;
}

//========================
//  storedSource Class
//========================
storedSource::storedSource(
	const uint16_t *voltage_data,
	const uint32_t *hold_time_data,
	uint8_t sz,
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(mode, _cycles),
		cur_ind(0),
		voltage_data(voltage_data),
		hold_time_data(hold_time_data),
		size(sz) { /*empty body*/ }

uint16_t storedSource::getVoltage(){
	return voltage_data[cur_ind];
}

uint32_t storedSource::holdTime(){
	if (opMode == ONE_SHOT && cur_ind == size-1)
		// it shouldn't come to this
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data[cur_ind];
}

void storedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (cur_ind + step_sz > size-1){
			if (opMode == ONE_SHOT)	status = FROZEN;
			else if (opMode == N_SHOT){
				cycle_index++;
				if (cycle_index == cycles) status = FROZEN;
			}
		}
		cur_ind = (cur_ind + step_sz) % size;
	}
}

void storedSource::reset(){
	cur_ind = 0;
	cycle_index = 0;
}


//========================
//  generatedSource Class
//========================

generatedSource::generatedSource(
	uint16_t (*voltage_data_fn)(uint8_t),
	uint32_t (*hold_time_data_fn)(uint8_t),
	uint8_t sz,
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(mode, _cycles),
		cur_ind(0),
		voltage_data_fn(voltage_data_fn),
		hold_time_data_fn(hold_time_data_fn),
		size(sz) { /*empty body*/ }

uint16_t generatedSource::getVoltage(){
	return voltage_data_fn(cur_ind);
}

uint32_t generatedSource::holdTime(){
	if (opMode == ONE_SHOT && cur_ind == size-1)
		// it shouldn't come to this
		return ONE_SHOT_FINISHED_HOLD_TIME;
	else
		return hold_time_data_fn(cur_ind);
}

void generatedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (cur_ind + step_sz > size-1){
			if (opMode == ONE_SHOT)
				status = FROZEN;
			else if(opMode == N_SHOT){
				cycle_index++;
				if (cycle_index == cycles)
					status = FROZEN;
			}
		}
		cur_ind = (cur_ind + step_sz) % size;
	}
}

void generatedSource::reset(){
	cur_ind = 0;
	cycle_index = 0;
}