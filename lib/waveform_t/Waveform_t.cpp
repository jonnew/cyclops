#include "Waveform_t.h"

//================================================================================================
//  Waveform Class
//================================================================================================

uint8_t Waveform::size = 0;
Waveform* Waveform::_list[4];

Waveform::Waveform(Cyclops *_cyclops, Source *_source) : 
  status(INIT)
{
  cyclops = _cyclops;
  source = _source;
  if (size < 4){
    // do not add to _list if it is full, silently drop.
    _list[size++] = this;
  }
  time_rem = 0;
}

Waveform::Waveform(Cyclops *_cyclops, Source *_source, operationMode mode, uint8_t _cycles) : 
  status(INIT)
{
  cyclops = _cyclops;
  source = _source;
  source->opMode = mode;
  source->cycles = _cycles;
  if (size < 4){
    // do not add to _list if it is full, silently drop.
    _list[size++] = this;
  }
  time_rem = 0;
}

uint8_t Waveform::prepare(){
  if (status == PREPARED)
    return 2; // already prepared
  if (SPI0_SR & SPI_SR_TFFF){
    uint16_t spi_frame = DAC_CONF_ACTIVE | (source->getVoltage() & 0x0fff);
    SPI_fifo.write16(cyclops->channel, spi_frame); // non blocking
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

void Waveform::useSource(Source *new_source, operationMode _mode, uint8_t shot_cycle /* = 1 */){
  source = new_source;
  source->opMode = _mode;
  source->cycles = shot_cycle;
  source->reset();
}

void Waveform::swapChannels(Waveform *w1, Waveform *w2){
  Cyclops *t = w1->cyclops;
  w1->cyclops = w2->cyclops;
  w2->cyclops = t;
}

double Waveform::initAll(){
  double min_hold_time = 9e12; // arbit, 9 mega sec
  for (uint8_t i=0; i<size; i++){
    _list[i]->prepare();
    _list[i]->time_rem = _list[i]->source->holdTime();
    _list[i]->source->stepForward(1);
    if (_list[i]->time_rem < min_hold_time){
      min_hold_time = _list[i]->time_rem;
    }
  }
  for (uint8_t i=0; i<Waveform::size; i++){
    Waveform::_list[i]->time_rem -= min_hold_time;
  }
  while (((SPI0_SR) & (15 << 12)) > 0); // wait till FIFO is empty
  for (uint8_t i=0; i<Waveform::size; i++){
    Waveform::_list[i]->cyclops->dac_load();
  }
  return min_hold_time;
}

void Waveform::processAll(){
  Timer1.disableInterrupt();
  for (uint8_t i=0; i<size; i++){
    // Since we use only SPI0 (for now?)
    // which has a 4 level FIFO, we can repeatedly write into SPI TX_FIFO
    if (_list[i]->status != PREPARED && _list[i]->source->status == ACTIVE){
      // guaranteed non-blocking call
      // Compiler will optimise away the branching if body is empty
      if (_list[i]->prepare() == 1) {;} // FIFO had no space...
      else                          {;} // Pushed frame to FIFO
    }
  }
  Timer1.enableInterrupt();
}

void cyclops_timer_isr(){
  // protect sreg??
  Waveform* wf_ptr;
  double min_hold_time = 9e12; // arbit, 9 mega-sec
  // loop through all Waveforms and Latch those which need to be LATCHED
  for (uint8_t i=0; i < Waveform::size; i++){
    wf_ptr = Waveform::_list[i];
    // @jonathan: Is the fuzzy 2us window small enough?
    if (wf_ptr->time_rem < 2){
      if (wf_ptr->status == PREPARED){
        wf_ptr->cyclops->dac_load();
      }
      wf_ptr->status = LATCHED;
      wf_ptr->source->stepForward(1);
      wf_ptr->time_rem = wf_ptr->source->holdTime();
    }
    // check if this has min_hold_time
    if (wf_ptr->time_rem < min_hold_time){
      min_hold_time = wf_ptr->time_rem;
    }
  }
  // if (min_hold_time < 1) // something is wrong
  for (uint8_t i=0; i < Waveform::size; i++){
    Waveform::_list[i]->time_rem -= min_hold_time;
  }
  Timer1.setPeriod(min_hold_time, 1);
  // protect sreg?
}