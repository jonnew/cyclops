#include "Waveform.h"

// Pre-instantiate the global list.
volatile WaveformList waveformList;

//================================================================================================
//  Waveform Class
//================================================================================================

Waveform::Waveform() : status(INIT) {
	time_rem = 0;
}

Waveform::Waveform(Cyclops *_cyclops, Source *_source) : 
	status(INIT)
{
	setup(_cyclops, _source, _source->opMode);
	time_rem = 0;
}

Waveform::Waveform(Cyclops *_cyclops, Source *_source, operationMode mode) : 
	status(INIT)
{
	setup(_cyclops, _source, mode);
	time_rem = 0;
}

void Waveform::setup(Cyclops *_cyclops, Source *_source, operationMode mode){
	cyclops = _cyclops;
	source = _source;
	source->opMode = mode;
}

uint8_t Waveform::prepare(){
	if (status != PREPARING)
		return 2;
	if (outFrame_id == DAC_BLOCK_SIZE){
		uint16_t voltage = source->getVoltage();
		outFrame[0] = DAC_CONF_ACTIVE | ((voltage >> 8) & 0x0f);
		outFrame[1] = (voltage & 0xff);
		outFrame_id = 0;
		cyclops->selectChip();
	}
	if (send(0, outFrame[outFrame_id]) == 1){
		outFrame_id++;
		if (outFrame_id == DAC_BLOCK_SIZE){
			status = PREPARED;
			cyclops->deselectChip();
			return 0;
		}
	}
	return 1;
}

void Waveform::resume(){
	source->status = ACTIVE;
}

void Waveform::pause(){
	source->status = FROZEN;
}

void Waveform::useSource(Source *new_source){
	source = new_source;
	source->reset();
}

void Waveform::swapChannels(Waveform *w1, Waveform *w2){
	Cyclops *t = w1->cyclops;
	w1->cyclops = w2->cyclops;
	w2->cyclops = t;
}

//================================================================================================
//  WaveformList Class
//================================================================================================

WaveformList::WaveformList(){
	size = 0;
	_claimedChannels = 0;
}

int8_t WaveformList::setWaveform(Cyclops *_cyclops, Source *_source, operationMode mode){
	// definitely protect this.
	if (size == 4){
		return -2; // no more slots available. Use at() and Waveform::setup() instead.
	}
	if (_BV(_cyclops->channel) & _claimedChannels){
		return -1; // the cyclops channel has already been claimed
	}
	_claimedChannels |= _BV(_cyclops->channel);
	TIMSK1 = 0x00; // disable Timer1 interrupt
	waveList[size++].setup(_cyclops, _source, mode);
	// return the index of this waveform in the waveformList
	TIMSK1 = _BV(TOIE1); // enable
	return size-1;
}

Waveform* WaveformList::at(uint8_t index){
	// no need to protect this.
	if (index >= size) return NULL;
	return &waveList[index];
}

void WaveformList::initialPrep(){
	// no need to prepare this. Timer hasn't even been initialised yet!
	int8_t j; // can be -1!!
	for (uint8_t i=0; i < size; i++){
		waveList[i].status = PREPARING;
		while (waveList[i].prepare() > 0);
		waveList[i].time_rem = waveList[i].source->holdTime();
		for (j=i-1; j >= 0; j--){
			if (waveList[sortedWaveforms[j]].time_rem > waveList[i].time_rem){
				sortedWaveforms[j+1] = sortedWaveforms[j];
			}
			else break;
		}
		sortedWaveforms[j+1] = i;
	}
	// all have been PREPARED and sortedWaveforms is also ready.
	double delta = waveList[sortedWaveforms[0]].time_rem;
	for (uint8_t i=0; i < size; i++){
		waveList[i].time_rem -= delta;
		waveList[i].cyclops->dac_load();
	}
	// enable timer ASAP, DON'T FORGET!
	
	// Now, the first waveform in sortedWaveforms is always going to have time_rem = 0
	// This first guy may, may not be PREPARED when the ISR comes, and we are "prepared"
	// both cases. So, bring it on!
}

int8_t WaveformList::forthcoming(waveformStatus _status) volatile{
	// this loop can be slightly optimised *******************************************************************
	// searching for the "forthcoming" ACTIVE && "_status" waveform
	
	// Only reads waveList and sortedWaveforms, so it should be protected.
	uint8_t res;
	TIMSK1 = 0x00; // disable Timer1 interrupt
	for (uint8_t i=0; i<size; i++){
		if (waveList[sortedWaveforms[i]].source->status == ACTIVE &&
			waveList[sortedWaveforms[i]].status == _status){

			res = sortedWaveforms[i];
			TIMSK1 = _BV(TOIE1); // enable
			return res;
		}
	}
	TIMSK1 = _BV(TOIE1); // enable
	return -1;
}

int8_t WaveformList::forthcoming(waveformStatus _status){
	// this loop can be slightly optimised *******************************************************************
	// searching for the "forthcoming" ACTIVE && "_status" waveform
	
	// Only reads waveList and sortedWaveforms, so it should be protected.
	uint8_t res;
	TIMSK1 = 0x00; // disable Timer1 interrupt
	for (uint8_t i=0; i<size; i++){
		if (waveList[sortedWaveforms[i]].source->status == ACTIVE &&
			waveList[sortedWaveforms[i]].status == _status){

			res = sortedWaveforms[i];
			TIMSK1 = _BV(TOIE1); // enable
			return res;
		}
	}
	TIMSK1 = _BV(TOIE1); // enable
	return -1;
}

uint8_t WaveformList::process(){
	// most of this function should be protected.

	// ignore FROZEN
	// service PREPARING
	uint8_t _preparing = 0;
	TIMSK1 = 0x00; // disable Timer1 interrupt
	for (uint8_t i=1; i < size; i++){
		if (waveList[i].status == PREPARING && waveList[i].source->status == ACTIVE){
			_preparing = waveList[i].prepare();
			// there's atmost one PREPARING, so just break out if we found it.
			break;
		}
	}
	if (_preparing == 0){
		// If not preparing any more, ie, either it got PREPARED or no PREPARING was found,
		// then service LATCHED / INIT
		int8_t w_index = forthcoming(LATCHED);
		if (w_index < 0){ // non latched even once!
			int8_t w_index = forthcoming(INIT);
			if (w_index < 0){ // Do nothing, all waveforms in PREPARED / PREPARING
				TIMSK1 = _BV(TOIE1); // enable
				return 1;
			}
		}
		waveList[w_index].status = PREPARING;
		waveList[w_index].prepare();
	}
	else{
		// Still PREPARING someone, so can't do anything. Probably CPU invoked the
		// function too soon and SPI did not finish transmitting even the first byte!
		// This is perfectly fine.
		;
	}
	TIMSK1 = _BV(TOIE1); // enable
	return 0;
}

//================================================================================================
//  WaveformList Class :: ISR helper functions
//================================================================================================

Waveform* WaveformList::top() volatile{
	return (Waveform* volatile) &waveList[sortedWaveforms[0]];
}

double WaveformList::update_time_rem() volatile{
	// No need to protect this function as it is always called inside an ISR.
	// Sorts, reads waveList, decrements `time_rem`
	updateSortedSeq();
	// this waveform is going to be processed in the next interrupt
	int8_t w_index = forthcoming(PREPARED);
	// update the time_rems
	double delta;
	if (w_index > 0){
		delta = waveList[w_index].time_rem; // this should be the smallest time_rem anyways
		for (uint8_t i; i < size; i++){
			waveList[i].time_rem -= delta;
		}
	}
	else{
		// stop timer? who will restart it?
		// for now, keeping the same period as "before"
	}
	return delta;
}

void WaveformList::updateSortedSeq() volatile{
	// this loop can be slightly optimised *******************************************************************
	// PRIVATE function. Always called by `update_time_rem` which is pseudo-protected
	// Hence this is also pseudo protected.
	
	// The top-of-sorted-seq has been serviced, now move it behind.
	// Assuming that this elem has been "stepForward"ed.
	uint8_t temp = sortedWaveforms[0], i;
	double target = waveList[sortedWaveforms[0]].time_rem;
	for (i=0; i < size-1; i++){
		if (waveList[sortedWaveforms[i+1]].time_rem <= target){
			sortedWaveforms[i] = sortedWaveforms[i+1];
		}
		else
			break;
	}
	sortedWaveforms[i] = temp;
}

//================================================================================================
//  TIMER1 ISR
//================================================================================================

void cyclops_timer_ISR(){
	// reads sortedWaveform, changes time_rem, sorts sortedWaveform, decrements
	// `time_rem`. Sets new timer period.
	
	uint8_t sreg = SREG;
	// latch on this now.
	Waveform *target_waveform = waveformList.top();
	// A more fuzzy sanity check and a loop can tie together multiple "latching"
	// events close by. Might add that later.
	if (target_waveform->time_rem < 5){ // sanity check (5 usec)
		if (target_waveform->status == PREPARED){ // this guy is ready?
			target_waveform->cyclops->dac_load();
		}
		// Must stepForward even if not ready...
		target_waveform->source->stepForward(1);
		target_waveform->time_rem = target_waveform->source->holdTime();
	}
	else{ // Critical Error / all paused. No Waveform::time_rem is ~0.
	    ;
	}
	// sort, decrement, set new period
	double newPeriod = waveformList.update_time_rem();
	Timer1.setPeriod(newPeriod);
	SREG = sreg;
}