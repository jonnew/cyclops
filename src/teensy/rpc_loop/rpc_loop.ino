#include <Cyclops.h>
#include <CLTask.h>

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*2048;
}

uint32_t triangle_update_period(uint8_t seq){
  return 15000 + seq;
}

uint16_t vd_1[] = {1, 2048};
uint32_t ht_1[] = {70000, 70000};

cyclops::generatedSource gen(triangle_wave, triangle_update_period, 40, cyclops::source::LOOPBACK);
cyclops::storedSource sto_1(vd_1, ht_1, 2, cyclops::source::LOOPBACK);
cyclops::squareSource square(cyclops::source::LOOPBACK);
  
cyclops::Source* globalSourceList[] = {&gen, &sto_1, &square};

cyclops::Source** cyclops::globalSourceList_ptr = globalSourceList;
cyclops::Board myb(cyclops::board::CH0);
cyclops::Waveform waveform(&myb, &gen);
cyclops::Queue processQueue;

void setup()
{
  pinMode(17, OUTPUT); // isr
  pinMode(16, OUTPUT); // LDAC pin
  digitalWrite(17, LOW);
  
  Serial.begin(57600);
  SPI_fifo.begin(SPI_CLOCK_16MHz); // 16MHz SPI clock, using pin 10 as CS

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
  digitalWrite(14, HIGH);
  if (processQueue.size > 0){
    cyclops::Task* t = processQueue.peek();
    t->compute();
    processQueue.pop();
  }
}


