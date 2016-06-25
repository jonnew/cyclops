#include "Source.h"

namespace cyclops{
using namespace source;

// static var definition
uint8_t Source::src_count = 0;

//================================================================================================
//  Source Abstract Class
//================================================================================================
Source::Source(sourceType _name, operationMode mode, uint8_t _cycles) : 
	DCoffset(0),
	opMode(mode),
	status(ACTIVE),
	src_id(++Source::src_count),
	name(_name),
	cycles(_cycles)
{
	TScale = 1;
	VScale = 1;
	cycle_index = 0;
}

void Source::setVScale(float vscale){
	if (vscale >= 0)
		VScale = vscale;
}

void Source::setTScale(float tscale){
	if (tscale >= 0)
		TScale = tscale;
}

void Source::setOffset(uint16_t offset){
	DCoffset = offset;
}

//================================================================================================
//  storedSource Class
//================================================================================================
storedSource::storedSource(
	const uint16_t *voltage_data,
	const uint32_t *hold_time_data,
	uint8_t sz,
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(STORED, mode, _cycles),
			cur_ind(0),
			voltage_data(voltage_data),
			hold_time_data(hold_time_data),
			size(sz)
{ /*empty body*/ }

uint16_t storedSource::getVoltage(){
	// no need to clamp it here, that will happen automatically when the value is
	// bitmasked into 12bits.
	return (voltage_data[cur_ind] * VScale) + DCoffset;
}

uint32_t storedSource::holdTime(){
	return min(hold_time_data[cur_ind] * TScale, 174762);
}

void storedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (cur_ind + step_sz > size-1){
			/*
			 * Possible Optimisation see squareSource::stepForward
			 */
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
	status = ACTIVE;
}


//================================================================================================
//  generatedSource Class
//================================================================================================

generatedSource::generatedSource(
	uint16_t (*voltage_data_fn)(uint8_t),
	uint32_t (*hold_time_data_fn)(uint8_t),
	uint8_t sz,
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(GENERATED, mode, _cycles),
			cur_ind(0),
			voltage_data_fn(voltage_data_fn),
			hold_time_data_fn(hold_time_data_fn),
			size(sz)
{ /*empty body*/ }

generatedSource::generatedSource(
	uint16_t (*voltage_data_fn)(uint8_t),
	uint32_t c_holdTime,
	uint8_t sz,
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(GENERATED, mode, _cycles),
			cur_ind(0),
			voltage_data_fn(voltage_data_fn),
			size(sz)
{
	hold_time_data_fn = nullptr;
	if (c_holdTime > 174762)
		constHoldTime = 174762;
	else
		constHoldTime = c_holdTime;
}

uint16_t generatedSource::getVoltage(){
	// no need to clamp it here, that will happen automatically when the value is
	// bitmasked into 12bits.
	return (voltage_data_fn(cur_ind) * VScale) + DCoffset;
}

void generatedSource::setTimePeriod(uint32_t new_period){
	//hold_time_data_fn = default_HT_func;
	constHoldTime = new_period;
}

uint32_t generatedSource::holdTime(){
	return
	min( ((hold_time_data_fn == nullptr)? constHoldTime : hold_time_data_fn(cur_ind)) * TScale,	174762 );
}

void generatedSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (cur_ind + step_sz > size-1){
			/*
			 * Possible Optimisation see squareSource::stepForward
			 */
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
	status = ACTIVE;
}

//================================================================================================
//  squareSource Class
//================================================================================================
squareSource::squareSource(
	operationMode mode,
	uint8_t _cycles /* = 1 */
	) :	Source(SQUARE, mode, _cycles),
		  onFlag(0)
{
	voltage_level[0] = 0;
	voltage_level[1] = 4095;
	level_time[0] = 50000;
	level_time[1] = 50000;
}

uint16_t squareSource::getVoltage(){
	// no need to clamp it here, that will happen automatically when the value is
	// bitmasked into 12bits.
	return VScale * voltage_level[onFlag] + DCoffset;
}

uint32_t squareSource::holdTime(){
	return min(level_time[onFlag] * TScale, 174762);
}

void squareSource::stepForward(uint8_t step_sz){
	if (status == FROZEN){
		return;
	}
	else{
		if (onFlag == 0){
			if (opMode == ONE_SHOT && cycle_index == 1)
				status = FROZEN;
			if (opMode == N_SHOT && cycle_index == cycles)
				status = FROZEN;
			/*
			 * Possible Optimisation if operationMode is changed by a method.
			 * Method must set cycles to 1 for transition from N_SHOT to other
			 * then,
			 * 
			 * if (opMode != LOOPBACK && cycle_index == cycles)
			 *   status = FROZEN
			 * 
			 * will be enough
			*/
		}
		else{
			cycle_index++;
		}
		onFlag = 1 - onFlag;
	}
}

void squareSource::reset(){
	onFlag = 0;
	cycle_index = 0;
	status = ACTIVE;
}

} // NAMESPACE cyclops