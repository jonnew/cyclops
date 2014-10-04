#include <Cyclops.h>
#include <SPI.h>

Cyclops cyclops;

void setup() {

}

void loop() {
    cyclops.send_test_waveform(CH1);
}
