#include "Waveform.h"

Waveform::Waveform(uint8_t chid, Source *s) : src(s), state(INIT), backup(INIT){
	if (chid < 0 || chid > 3); //do something!
	channel_id = chid;
	mode = src->mode;
	//time_rem = ???
}

Waveform::changeStatus(_status new_state){
	state = new_state;
	// some state transitions might require more work
}

Waveform::markLatched(){
	state = LATCHED;
}

Waveform::resume(){
	state = backup;
}

Waveform::pause(){
	backup = state;
}

Waveform::useSource(Source *s){
	src = s;
}

void Waveform::swapChannels(Waveform *w1, Waveform *w2){
	uint8_t t = w1->channel_id;
	w1->channel_id = w2->channel_id;
	w2->channel_id = t;
}