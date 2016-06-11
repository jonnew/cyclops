#include "Waveform_t.h"

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
  if (status == PREPARED)
    return 2; // already prepared
  if (SPI0_SR & SPI_SR_TFFF){
    uint16_t spi_frame = 0x1000 | (source->getVoltage() & 0x0fff);
    SPIFIFO.write16(spi_frame); // non blocking
    status = PREPARED;
  }
  else{
    // FIFO full. Could busy wait here...
    return 1;
  }
  return 0;
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

void cyclops_timer_ISR(){
  return;
}