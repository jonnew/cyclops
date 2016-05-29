#include "Waveform.h"

Waveform::Waveform(Cyclops *_cyclops, Source *_source) : 
	status(INIT)
{
	cyclops = _cyclops;
	source = _source;
	//time_rem = ???
}

Waveform::Waveform(Cyclops *_cyclops, Source *_source, operationMode mode) : 
	status(INIT)
{
	_source->opMode = mode;
	cyclops = _cyclops;
	source = _source;
	//time_rem = ???
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
	status = backup_myStatus;
	source->status = ACTIVE;
}

void Waveform::pause(){
	backup_myStatus = status;
	status = PAUSED;
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

uint8_t processWaveform(Waveform* waveList[], uint8_t sz){
	// ignore PAUSED
	// service PREPARING
	// service LATCHED / INIT
	uint16_t min_time_rem = 0xffff;
	int8_t min_time_rem_index = -1;
	for (uint8_t i=0; i<sz; i++){
		switch (waveList[i]->status){
			case PREPARING:
				waveList[i]->prepare();
			break;

			case LATCHED:
			case INIT:
				//searching for the forthcoming waveform update
				if (waveList[i]->time_rem <= min_time_rem){
					min_time_rem = waveList[i]->time_rem;
					min_time_rem_index = i;
				}
			break;
		}
	}
	// forthcoming waveform has been found
	waveList[min_time_rem_index]->status = PREPARING;
	waveList[min_time_rem_index]->prepare();
	return 0;
}