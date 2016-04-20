#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>



/*
 * Just create a single Task object, test all methods
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
  uint16_t vd(uint8_t seq){
    return (1 - seq) * 1023;
  }
  uint16_t htd(uint8_t seq){
    return 500 + 1000*seq;
  }
  gen_Source generator_wave(vd, htd, 2);
#else
  uint16_t vd(uint8_t seq){
    double t = sin((float)seq/100.0 * PI);
    t *= t;
    //Serial.print(t*255);
    return (uint16_t) (t*255);
  }
  uint16_t htd(uint8_t seq){
    return 25;
  }
  gen_Source generator_wave(vd, htd, 100);
#endif

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
  #ifdef NO_LED
    Serial.print(i);
    Serial.print(" ");
    Serial.print(base->nextVoltage());
    Serial.print(" ");
    Serial.println(base->holdTime());
    delay(base->holdTime());
    base->stepForward(1);
    i = (i+1)%2;
  #else
    analogWrite(11, base->nextVoltage());
    //Serial.println(base->nextVoltage());
    delay(base->holdTime());
    base->stepForward(1);
  #endif
}