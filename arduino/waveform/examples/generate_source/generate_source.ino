#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>



/*
 * Sets up a generatedSource to create:
 *     1. (to use with LED)            a sine^2 wave or
 *     2. (to use with Serial Monitor) a square wave
 * 
 * In any case, make a generatedSource object.
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
// Debug using Serial Monitor
  uint16_t vd(uint8_t seq){
    return (1 - seq) * 1023;
  }
  uint16_t htd(uint8_t seq){
    return 500 + 1000*seq;
  }
  generatedSource generator_wave(vd, htd, 2);
#else

// See the waveform on the LED
  uint16_t vd(uint8_t seq){
    double t = sin((float)seq/100.0 * PI);
    t *= t;
    //Serial.print(t*255);
    return (uint16_t) (t*255);
  }
  uint16_t htd(uint8_t seq){
    return 25;
  }
  generatedSource generator_wave(vd, htd, 100);
#endif

// Using the abstract base class pointer
Source *base = &generator_wave;

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
    Serial.print(base->nextVoltage());
    Serial.print(" ");
    Serial.println(base->holdTime());
    delay(base->holdTime());
    base->stepForward(1);
    i = (i+1)%2;
  #else
  // See the wave on the LED
    analogWrite(11, base->nextVoltage());
    delay(base->holdTime());
    base->stepForward(1);
  #endif
}