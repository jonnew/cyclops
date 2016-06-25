#include <Cyclops.h>
#include "CLTask.h"

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*2048;
}

uint32_t triangle_update_period(uint8_t seq){
  return 30;
}

uint16_t triangle_wave_s(uint8_t seq){
  return (abs(10 - seq)/10.0)*4095;
}

uint32_t triangle_update_period_s(uint8_t seq){
  return 30;
}

uint16_t vd_1[] = {1, 2048};
uint16_t vd_2[] = {1, 4095};

uint32_t ht_1[] = {30, 30};
uint32_t ht_2[] = {30, 30};

cyclops::generatedSource gen_f(triangle_wave, triangle_update_period, 40, cyclops::source::LOOPBACK);

cyclops::generatedSource gen_s(triangle_wave_s, triangle_update_period_s, 20, cyclops::source::LOOPBACK);

cyclops::storedSource sto_1(vd_1, ht_1, 2, cyclops::source::LOOPBACK);

cyclops::storedSource sto_2(vd_2, ht_2, 2, cyclops::source::LOOPBACK);


cyclops::Source* SourceList[] = {&gen_f, &gen_s, &sto_1, &sto_2};
cyclops::Source** cyclops::source::globalSourceList_ptr = SourceList;

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
  pinMode(14, OUTPUT); // prepare
  
  digitalWrite(9, HIGH);
  digitalWrite(14, LOW);
  
  Serial.begin(57600);
  SPI_fifo.begin(SPI_CLOCK_6MHz); // 16MHz SPI clock, using pin 10 as CS

  double delta = cyclops::Waveform::initAll();
  Timer1.initialize(delta);
  Timer1.attachInterrupt(cyclops::cyclops_timer_isr); // Defined in Waveform_t.h
}

void loop()
{
  digitalWrite(14, HIGH);
  cyclops::Waveform::processAll();
  digitalWrite(14, LOW);
  
  cyclops::readSerialAndPush(&processQueue);
  if (processQueue.size > 0){
    cyclops::Task* t = processQueue.peek();
    t->compute();
    processQueue.pop();
    ;
  }
}


