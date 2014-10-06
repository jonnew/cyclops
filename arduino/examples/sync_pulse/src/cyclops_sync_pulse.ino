#include <Cyclops.h>
#include <SPI.h>

Cyclops cyclops0(CH0);
Cyclops cyclops1(CH1);


float mA;
int16_t test_pulse[] = {100,4095,100};

void setup() {
  
  // Set intput resistors on both boards to nomimal A-W Resistance
  cyclops0.mcp4022_set_nom_AWR();
  cyclops1.mcp4022_set_nom_AWR();
}

void loop() {
  
  // Bring both boards data registers to full scale and 
  // synchronously update the DACs
  cyclops0.mcp4921_load_voltage(4095);
  cyclops1.mcp4921_load_voltage(4095);
  cyclops0.mcp4921_load_dac();
  
  delayMicroseconds(100);
  
  // Bring both boards data registers to 0 scale and 
  // synchronously update the DACs
  cyclops0.mcp4921_load_voltage(0);
  cyclops1.mcp4921_load_voltage(0);
  cyclops0.mcp4921_load_dac();
  
  delayMicroseconds(100);
}

