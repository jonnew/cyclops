#include <Cyclops.h>
#include "CLTask.h"

#include "MySources.h"

cyclops::Board ch0(cyclops::board::CH0);
cyclops::Board ch1(cyclops::board::CH1);
cyclops::Board ch2(cyclops::board::CH2);
cyclops::Board ch3(cyclops::board::CH3);

cyclops::Waveform w0(&ch0, &gen_f);
cyclops::Waveform w1(&ch1, &gen_s);
cyclops::Waveform w2(&ch2, &sto_1);
cyclops::Waveform w3(&ch3, &sto_2);


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


