#include "Waveform.h"

namespace cyclops{
using namespace board;
using namespace source;
//================================================================================================
//  Waveform Class
//================================================================================================

uint8_t Waveform::size = 0;
Waveform* Waveform::_list[4];

Waveform::Waveform(Board *_board_ptr, Source *_source_ptr) : 
  status(INIT)
{
  board_ptr = _board_ptr;
  source_ptr = _source_ptr;
  if (size < 4){
    // do not add to _list if it is full, silently drop.
    _list[size++] = this;
  }
  time_rem = 0;
}

Waveform::Waveform(Board *_board_ptr, Source *_source_ptr, operationMode mode, uint8_t _cycles) : 
  status(INIT)
{
  board_ptr = _board_ptr;
  source_ptr = _source_ptr;
  source_ptr->opMode = mode;
  source_ptr->cycles = _cycles;
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
    // clamp votage to corect range (0, 4095)
    uint16_t spi_frame = DAC_CONF_ACTIVE | (source_ptr->getVoltage() & 0x0fff);
    SPI_fifo.write16(board_ptr->channel, spi_frame); // non blocking
    status = PREPARED;
    if (board_ptr->trig_config == SOFT_ON_WRITE_START){
      board_ptr->software_hook_fn();
    }
  }
  else{
    // FIFO full. Could busy wait here...
    return 1;
  }
  return 0;
}

void Waveform::resume(){
  source_ptr->status = ACTIVE;
}

void Waveform::pause(){
  source_ptr->status = FROZEN;
}

void Waveform::useSource(Source *new_source, operationMode _mode, uint8_t shot_cycle /* = 1 */){
  source_ptr = new_source;
  source_ptr->opMode = _mode;
  source_ptr->cycles = shot_cycle;
  source_ptr->reset();
}

void Waveform::swapChannels(Waveform *w1, Waveform *w2){
  Board *t = w1->board_ptr;
  w1->board_ptr = w2->board_ptr;
  w2->board_ptr = t;
}

uint32_t Waveform::initAll(){
  uint32_t min_hold_time = 0xffffffff; // arbit
  for (uint8_t i=0; i<size; i++){
    _list[i]->prepare();
    _list[i]->time_rem = _list[i]->source_ptr->holdTime();
    _list[i]->source_ptr->stepForward(1);
    if (_list[i]->time_rem < min_hold_time){
      min_hold_time = _list[i]->time_rem;
    }
  }
  for (uint8_t i=0; i<Waveform::size; i++){
    Waveform::_list[i]->time_rem -= min_hold_time;
  }
  while (((SPI0_SR) & (15 << 12)) > 0); // wait till FIFO is empty
  for (uint8_t i=0; i<Waveform::size; i++){
    Waveform::_list[i]->board_ptr->dac_load();
  }
  return min_hold_time;
}

void Waveform::processAll(){
  Timer1.disableInterrupt();
  for (uint8_t i=0; i<size; i++){
    // Since we use only SPI0 (for now?)
    // which has a 4 level FIFO, we can repeatedly write into SPI TX_FIFO
    if (_list[i]->status != PREPARED && _list[i]->source_ptr->status == ACTIVE){
      // guaranteed non-blocking call
      // Compiler will optimise away the branching if body is empty
      if (_list[i]->prepare() == 1) {;} // FIFO had no space...
      else                          {;} // Pushed frame to FIFO
    }
  }
  Timer1.enableInterrupt();
}

void Waveform::groundAll(){
  for (uint8_t i=0; i<size; i++){
    _list[i]->board_ptr->dac_prog_voltage(0);
  }
  while (((SPI0_SR) & (15 << 12)) > 0); // wait till FIFO is empty
  for (uint8_t i=0; i<size; i++){
    _list[i]->board_ptr->dac_load();
  }
}

} // NAMESPACE cyclops

void cyclops::cyclops_timer_isr(){
  // protect sreg??
  Waveform* wf_ptr;
  uint32_t min_hold_time = 0xffffffff; // arbit
  // loop through all Waveforms and Latch those which need to be LATCHED
  for (uint8_t i=0; i < Waveform::size; i++){
    wf_ptr = Waveform::_list[i];
    // @jonathan: Is the fuzzy 10us window fine?
    if (wf_ptr->time_rem < 10){
      if (wf_ptr->status == PREPARED){
        wf_ptr->board_ptr->dac_load();
        // Call the hook set by the user.
        if (wf_ptr->board_ptr->trig_config == SOFT_ON_LOAD){
          wf_ptr->board_ptr->software_hook_fn();
        }
      }
      wf_ptr->status = LATCHED;
      wf_ptr->source_ptr->stepForward(1);
      wf_ptr->time_rem = wf_ptr->source_ptr->holdTime();
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