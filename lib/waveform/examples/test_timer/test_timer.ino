#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <nbSPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>
#include <TimerOne.h>

/*
 * Sets up a generatedSource to create:
 * (to use with Serial Monitor) a ramp-up and ramp-down signal
 * Uses Timer1 for scheduling. The count printed in angle brackets <> was incremented
 * from 0 till that between 2 IRQs.
 * 
 * In any case, make a storedSource object.
 * Refer to it using the Base "Source" class.
 * 
 * Open the Serial Monitor before flashing this program.
 * Make sure baud rate is matched!
 * 
 * If you want to see waveform on Serial monitor,
 * uncomment the #define below
 */

void timer_one_isr();

uint16_t vd[]  = {0x80, 0x100, 0x180, 0x300, 0x180, 0x100, 0x80, 0x3e};
double   htd[] = {5e5, 3e5, 5e5, 3e5, 5e5, 3e5, 5e5, 3e5}; // delay in us

storedSource stored(vd, htd, 8);
// Using the abstract base class pointer
Source *base = &stored;

void setup() {
  Serial.begin(57600);
  Timer1.initialize(base->holdTime());
  Timer1.attachInterrupt(timer_one_isr);
}

volatile bool must_prepare = true;
uint8_t i = 0;
long count = 0;

void loop() {
  /*
   * Notice how methods of the derived class are called using the base class pointer
   * And the Timer1 ISR.
   */
  // Keep trying to "prepare"
  i = prepare(i);
  count++;
  //do other productive tasks, like:
  // RPC
  // OCP
}

uint8_t prepare(uint8_t i){
  // This is NOT AT ALL analogous to Waveform::prepare
  if (must_prepare){ // just a 1B read, so no need to protect
    Serial.print(i);
    Serial.print(" ");
    Serial.print(base->getVoltage());
    Serial.print(" ");
    Serial.print(base->holdTime());
    Serial.print(" ");
    Serial.println(count);
    must_prepare = false; // just a 1B write, so no need to protect
    count = 0;
    return ((i+1) & 0x07); // (i+1)%8
  }
  return i;
}

void timer_one_isr(){
  must_prepare = true;
  base->stepForward(1);
  Timer1.setPeriod(base->holdTime());
}