#include "Waveform.h"

Waveform::Waveform(Cyclops *_cyclops, Source *s) : 
	state(INIT)
{
	mode = src->mode;
	cyclops = _cyclops;
	src = s;
	//time_rem = ???
}

Waveform::Waveform(Cyclops *_cyclops, Source *s, _op_mode _mode) : 
	state(INIT)
{
	src->mode = _mode;
	cyclops = _cyclops;
	src = s;
	//time_rem = ???
}

void Waveform::resume(){
	state = backup_this;
	src->status = ACTIVE;
}

void Waveform::pause(){
	backup_this = state;
	src->status = PAUSED;
}

void Waveform::useSource(Source *s){
	src = s;
	src->reset();
}

void Waveform::swapChannels(Waveform *w1, Waveform *w2){
	Cyclops *t = w1->cyclops;
	w1->cyclops = w2->cyclops;
	w2->cyclops = t;
}