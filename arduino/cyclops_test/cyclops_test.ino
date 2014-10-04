#include <Cyclops.h>
#include <SPI.h>

Cyclops cyclops;

void setup() {
  
}

void loop() {
    cyclops.mcp4921_send_test_waveform(CH0);
    
    cyclops.over_current_protect(CH0,200);
}
