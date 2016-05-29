#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>



/*
 * Sets up a generatedSource to create:
 *     1. (to use with LED)            a slow ramp-up and fast ramp-down signal
 *     2. (to use with Serial Monitor) a ramp-up and ramp-down signal
 * 
 * In any case, make a storedSource object.
 * Refer to it using the Base "Source" class.
 * 
 * Open the Serial Monitor before flashing this program.
 * Make sure baud rate is matched!
 * 
 * Don't forget to connect pin11 and pin13
 * 
 * If you want to see waveform on Serial monitor,
 * uncomment the #define below
 */

//#define NO_LED

#ifdef NO_LED
  uint16_t vd[]  = {0x80, 0x100, 0x180, 0x300, 0x180, 0x100, 0x80, 0x3e};
  uint16_t htd[] = {500, 3000, 500, 3000, 500, 3000, 500, 3000}; // delay in ms
#else
  uint16_t vd[]  = {0x20, 0x40, 0x80, 0xa0, 0xc0, 0x80, 0x20, 0x04};
  uint16_t htd[] = {200, 200, 200, 200, 200, 1500, 1500, 1500}; // delay in ms
#endif

storedSource stored(vd, htd, 8);
// Using the abstract base class pointer
Source *base = &stored;

void setup() {
  Serial.begin(57600);
  #ifndef NO_LED
    pinMode(13, INPUT);
  #else
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
  #endif
}

uint8_t i = 0;

void loop() {
  /*
   * Notice how methods of the derived class are called using the base class pointer
   */
  #ifdef NO_LED
  // Debug using Serial Monitor
    Serial.print(i);
    Serial.print(" ");
    Serial.print(base->getVoltage());
    Serial.print(" ");
    Serial.println(base->holdTime());
    delay(base->holdTime());
    base->stepForward(1);
    i = (i+1)%8;
  #else
  // See the wave on the LED
    analogWrite(11, base->getVoltage());
    delay(base->holdTime());
    base->stepForward(1);
  #endif
}
