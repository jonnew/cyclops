#include <Waveform_t.h>

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*2048;
}

double triangle_update_period(uint8_t seq){
  return 15000 + seq;
}

generatedSource gen(triangle_wave, triangle_update_period, 40);
Cyclops cyclops(CH0);
Waveform waveform(&cyclops, &gen);

void setup()
{
  pinMode(14, OUTPUT); // prepare
  pinMode(16, OUTPUT); // isr
  pinMode(9, OUTPUT); // LDAC pin
  digitalWrite(9, HIGH);
  digitalWrite(14, LOW);
  
  //Serial.begin(57600);
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
