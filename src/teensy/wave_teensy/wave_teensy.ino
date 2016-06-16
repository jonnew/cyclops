
#include <Waveform_t.h>
void timer_isr();
uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*4095;
}

double triangle_update_period(uint8_t seq){
  return 150000;
}

generatedSource gen(triangle_wave, triangle_update_period, 40);
Cyclops cyclops(CH0);
Waveform waveform(&cyclops, &gen, LOOPBACK);

void setup()
{
  pinMode(14, OUTPUT); // prepare
  pinMode(16, OUTPUT); // isr
  pinMode(9, OUTPUT); // LDAC pin
  digitalWrite(9, HIGH);
  digitalWrite(14, LOW);
  SPI_fifo.begin(CH0, SPI_CLOCK_16MHz); // 16MHz SPI clock, using pin 10 as CS
  waveform.prepare();
  waveform.cyclops->dac_load();
  waveform.source->stepForward(1);
  
  Timer1.initialize(waveform.source->holdTime());
  Timer1.attachInterrupt(timer_isr); // Defined in Waveform_t.h
  //Serial.begin(115200);
}

void loop()
{
  long res = 1;
  digitalWrite(14, HIGH);
  waveform.prepare();
  digitalWrite(14, LOW);
  for (uint8_t i=2; i<=20; i++){
    res *= i;
  }
  digitalWrite(14, HIGH);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(14, LOW);
  // do other work
  // like compute factorial, do not write to Serial -- calls are
  // blocking...
  // They won't be for RPC, I spent many hours understanding the 
  // Teensy USB Serial.
}

void timer_isr(){
  digitalWrite(16, HIGH);
  if (waveform.status == PREPARED){
    waveform.cyclops->dac_load();
    waveform.status = LATCHED;
  }
  double delta = waveform.source->holdTime();
  waveform.time_rem = delta;
  Timer1.setPeriod(delta, 1); // try putting a const value here, like 10000
  waveform.source->stepForward(1);
  digitalWrite(16, LOW);
}
