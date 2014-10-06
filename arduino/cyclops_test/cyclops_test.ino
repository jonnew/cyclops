#include <Cyclops.h>
#include <SPI.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

Cyclops cyclops;
float mA;
int16_t test_pulse[] = {100,4095,100};

void setup() {
  
  cyclops.mcp4022_set_nom_AWR(CH0);
  //cyclops.mcp4921_single_shot(CH0, 4095);
  PCintPort::attachInterrupt(TRIG0, sendWave,RISING); // attach a PinChange Interrupt to our pin on the rising edge
}

void loop() {
    
    
    
//    mA = cyclops.measure_current(CH0);
//    Serial.print(mA);
//    Serial.print("\t");
//    Serial.print(cyclops._wiper_position[CH0]);
//    Serial.print("\n");
//    cyclops.over_current_protect(CH0,200);
    
}

void sendWave() {
  cyclops.mcp4921_generate_waveform(CH0, test_pulse, 3, 1000);
}
