#include <Cyclops.h>
#include <SPI.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

Cyclops cyclops0(CH0);
Cyclops cyclops1(CH1);


float mA;
int16_t test_pulse[] = {100,4095,100};

void setup() {
  
  cyclops0.mcp4022_set_nom_AWR();
  cyclops1.mcp4022_set_nom_AWR();
  //cyclops.mcp4921_single_shot(CH0, 4095);
  //PCintPort::attachInterrupt(TRIG0, sendWave,RISING); // attach a PinChange Interrupt to our pin on the rising edge
}

void loop() {
    
    
//    cyclops0.mcp4921_generate_waveform(test_pulse, 3, 1000);
//    cyclops1.mcp4921_generate_waveform(test_pulse, 3, 1000);

      cyclops0.mcp4921_load_voltage(1000);
      cyclops1.mcp4921_load_voltage(4095);
      cyclops0.mcp4921_load_dac();
      
      delayMicroseconds(100);
      
      cyclops0.mcp4921_load_voltage(0);
      cyclops1.mcp4921_load_voltage(0);
      cyclops0.mcp4921_load_dac();
      
            delayMicroseconds(100);
    
//    mA = cyclops.measure_current(CH0);
//    Serial.print(mA);
//    Serial.print("\t");
//    Serial.print(cyclops._wiper_position[CH0]);
//    Serial.print("\n");
//    cyclops.over_current_protect(CH0,200);
    
}

//void sendWave() {
//  
//}
