#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <nbSPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>
#include <TimerOne.h>

//#define SERIAL_DBG

/*
 * Sets up a storedSourcep to create:
 * (to use with Serial Monitor) a ramp-up and ramp-down signal
 * Uses Timer1 for scheduling.
 * 
 * This is a POC for the operations of WaveformList.
 */

void timer_one_isr();

uint16_t vd[]  = {0x80, 0x200, 0x80, 0x400, 0x80, 0x800, 0x80, 0xf00, 0x80, 0x800, 0x80, 0x400, 0x80, 0x200, 0x80};
double   htd[] = {5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 8e5}; // delay in us
storedSource stored(vd, htd, 15);

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*4095;
}

double triangle_update_period(uint8_t seq){
  return 15000;
}

generatedSource generated(triangle_wave, triangle_update_period, 40);

Cyclops ch0(CH0);
Waveform W0(&ch0, &generated);

void setup() {
  #ifdef SERIAL_DBG
    Serial.begin(115200);
  #endif
  initSPI();
  pinMode(3, OUTPUT); // error LED, should NEVER turn ON!
  
  // This block is analogous to WaveformList::initialPrep()
    W0.status = PREPARING;
    while (W0.prepare() == 1);
    W0.time_rem = 0;
    W0.status = LATCHED;
    W0.cyclops->dac_load();
  // endblock

  Timer1.initialize(W0.source->holdTime());
  Timer1.attachInterrupt(timer_one_isr);
  digitalWrite(3, LOW);
}

void loop() {
  // Keep trying to "prepare"
  process_wave();
  //do other productive tasks, like:
  // RPC
  // OCP
}

uint8_t process_wave(){
  // This is analogous to WaveformList::process()
  uint8_t _preparing = 0;
  TIMSK1 = 0x00; // disable Timer1 interrupt
  if (W0.status == PREPARING && W0.source->status == ACTIVE){
    _preparing = W0.prepare();
  }
  if (_preparing == 0){
    // Not preparing any more,
    if (W0.status != LATCHED && W0.status != INIT){
      // then skip if neither LATCHED nor INIT
      TIMSK1 = _BV(TOIE1); // enable
      return 1;
    }
    //It's been latched!
    W0.status = PREPARING;
    W0.prepare();
  }
  else{
    // Still PREPARING someone, so can't do anything. Probably CPU invoked the
    // function too soon and SPI did not finish transmitting even the first byte!
    // This is perfectly fine.
    ;
  }
  TIMSK1 = _BV(TOIE1); // enable
  return 0;
}

void timer_one_isr(){
  uint8_t sreg = SREG;
  #ifdef SERIAL_DBG
    Serial.print("~");
    Serial.println(W0.status);
  #endif
  if (W0.status == PREPARED){
    #ifdef SERIAL_DBG
      Serial.print("*");
      Serial.println(W0.status);
    #endif
    W0.cyclops->dac_load();
  }
  else{
    digitalWrite(3, HIGH);
  }
  W0.status = LATCHED;
  W0.source->stepForward(1);
  //W0.time_rem = W0.source->holdTime();
  Timer1.setPeriod(W0.source->holdTime());
  SREG = sreg;
}


