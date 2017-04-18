#include <Cyclops.h>
#include <CLTask.h>

#include "MySources.h"

cyclops::Board myb(cyclops::board::CH0);
cyclops::Waveform waveform(&myb, &gen);
cyclops::Queue processQueue;

void setup()
{
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  
  Serial.begin(57600);
  SPI_fifo.begin(SPI_CLOCK_6MHz); // 16MHz SPI clock, using pin 10 as CS
  double delta = cyclops::Waveform::initAll();
  Timer1.initialize(delta);
  Timer1.attachInterrupt(cyclops::cyclops_timer_isr); // Defined in Waveform_t.h
}

void loop()
{
  cyclops::Waveform::processAll();
  cyclops::readSerialAndPush(&processQueue);
  if (processQueue.size > 0){
    cyclops::Task* t = processQueue.peek();
    t->compute();
    processQueue.pop();
  }
}
