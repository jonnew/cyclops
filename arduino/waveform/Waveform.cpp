#include "Waveform.h"

Waveform::Waveform(Cyclops *_cyclops, Source *_source) : 
	status(INIT)
{
	mode = _source->opMode;
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