#include <MsTimer2.h>

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
  uint16_t vd[]  = {0x80, 0x100, 0x180, 0x300, 0x180, 0x100, 0x80, 0x3e};
  uint16_t htd[] = {500, 3000, 500, 3000, 500, 3000, 500, 3000}; // delay in ms
#else
  uint16_t vd[]  = {0x20, 0x40, 0x80, 0xa0, 0xc0, 0xa0, 0x80, 0x40};
  uint16_t htd[] = {500, 200, 200, 200, 200, 1500, 1500, 1500}; // delay in ms
#endif

st_Source stored(vd, htd, 8);
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
  #ifdef NO_LED
    Serial.print(i);
    Serial.print(" ");
    Serial.print(base->nextVoltage());
    Serial.print(" ");
    Serial.println(base->holdTime());
    delay(base->holdTime());
    base->stepForward(1);
    i = (i+1)%8;
  #else
    analogWrite(11, base->nextVoltage());
    //Serial.println(base->nextVoltage());
    delay(base->holdTime());
    base->stepForward(1);
  #endif
}