#include <Waveform_t.h>
#include <Task.h>

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*2048;
}

uint32_t triangle_update_period(uint8_t seq){
  return 25;
}

uint16_t triangle_wave_s(uint8_t seq){
  return (abs(10 - seq)/10.0)*4095;
}

uint32_t triangle_update_period_s(uint8_t seq){
  return 25;
}

uint16_t vd_1[] = {1, 2048};
uint16_t vd_2[] = {1, 4095};

uint32_t ht_1[] = {25, 25};
uint32_t ht_2[] = {25, 25};

generatedSource gen_f(triangle_wave, triangle_update_period, 40, LOOPBACK);

generatedSource gen_s(triangle_wave_s, triangle_update_period_s, 20, LOOPBACK);

storedSource sto_1(vd_1, ht_1, 2, LOOPBACK);

storedSource sto_2(vd_2, ht_2, 2, LOOPBACK);

Source* globalSourceList[] = {&gen_f, &gen_s, &sto_1, &sto_2};
Source **globalSourceList_ptr = globalSourceList;

Cyclops ch0(CH0);
Cyclops ch1(CH1);
Cyclops ch2(CH2);
Cyclops ch3(CH3);

Waveform w0(&ch0, &gen_f);
Waveform w1(&ch1, &gen_s);
Waveform w2(&ch2, &sto_1);
Waveform w3(&ch3, &sto_2);

Queue processQueue;

void setup()
{
  pinMode(14, OUTPUT); // prepare
  
  digitalWrite(9, HIGH);
  digitalWrite(14, LOW);
  
  Serial.begin(57600);
  SPI_fifo.begin(SPI_CLOCK_6MHz); // 16MHz SPI clock, using pin 10 as CS

  double delta = Waveform::initAll();
  Timer1.initialize(delta);
  Timer1.attachInterrupt(cyclops_timer_isr); // Defined in Waveform_t.h
}

void loop()
{
  long res = 1;
  digitalWrite(14, HIGH);
  Waveform::processAll();
  digitalWrite(14, LOW);
  
  readSerial(&processQueue);
  digitalWrite(14, HIGH);
  if (processQueue.size > 0){
    Task* t = processQueue.peek();
    t->compute();
    processQueue.pop();
    ;
  }
}

/*
blu 2 ch0
pink 3 ch1
yello 1 ch2
green 4 ch3
*/

