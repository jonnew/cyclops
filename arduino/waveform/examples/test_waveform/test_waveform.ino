#include <Waveform.h>
#include <Source.h>

#include <Wire.h>
#include <SPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>



/*
 * Create a single waveform object but 2 source objects
 * Change source object at runtime.
 * Hence, source objects are like the "stored information" and waveform
 * can just switch between them
 * 
 * Here, there are 2 generatedSource objs: 
 *     1. (sin(t))^2 and (sin(2t))^2              (for LED)
 *     2. 2 square waves of different timeperiods (for Serial Debug)
 * and the waveform object ping-pongs between them.
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
  // square wave of timeperiod 1000ms
  uint16_t vd_g2(uint8_t seq){
    return (1 - seq) * 1023;
  }
  uint16_t htd_g2(uint8_t seq){
    return 250 + 500*seq;
  }
  generatedSource gen_wave2(vd_g2, htd_g2, 2);

  // square wave of timeperiod 2000ms
  uint16_t vd_g(uint8_t seq){
    return (1 - seq) * 1023;
  }
  uint16_t htd_g(uint8_t seq){
    return 500 + 1000*seq;
  }
  generatedSource gen_wave1(vd_g, htd_g, 2);

  uint16_t vda[]  = {0x80, 0x100, 0x180, 0x300, 0x180, 0x100, 0x80, 0x3e};
  uint16_t htda[] = {500, 3000, 500, 3000, 500, 3000, 500, 3000}; // delay in ms
#else
  uint16_t vd_g2(uint8_t seq){
    double t = sin((float)seq/100.0 * PI);
    t *= t;
    //Serial.print(t*255);
    return (uint16_t) (t*255);
  }
  uint16_t htd_g2(uint8_t seq){
    return 15;
  }
  generatedSource gen_wave2(vd_g2, htd_g2, 100);

  uint16_t vd_g(uint8_t seq){
    double t = sin((float)seq/100.0 * PI);
    t *= t;
    //Serial.print(t*255);
    return (uint16_t) (t*255);
  }
  uint16_t htd_g(uint8_t seq){
    return 30;
  }
  generatedSource gen_wave1(vd_g, htd_g, 100);
#endif

Cyclops cyclops(CH0);
Waveform my_waveform(&cyclops, &gen_wave2);

void setup() {
  Serial.begin(57600);
  #ifndef NO_LED
    pinMode(13, INPUT);
  #else
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
  #endif
}

uint8_t src_id = 0;
uint8_t i = 1;

void loop() {
 #ifndef NO_LED
  if (i == 101){
 #else
  if (i == 4){
 #endif
    if (src_id == 0){
      my_waveform.useSource(&gen_wave1);    // switching to slow wave
      Serial.println("sin(t)");
      src_id = 1;
    }
    else{
      my_waveform.useSource(&gen_wave2);   // switching to fast wave
      Serial.println("sin(2t)");
      src_id = 0;
    }
    i = 0;
  }
  
 #ifdef NO_LED
 // Use Serial Monitor to Debug
  Serial.print(my_waveform.source->getVoltage());
  Serial.print(" ");
  Serial.println(my_waveform.source->holdTime());
 #else

 // See thw waves on the LED
  analogWrite(11, my_waveform.source->getVoltage());
 #endif

 delay(my_waveform.source->holdTime());
 my_waveform.source->stepForward(1);

 i++;
}



