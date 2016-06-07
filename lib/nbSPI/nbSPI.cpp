#include "nbSPI.h"

// Global definitions
static volatile bool _busy_0;

void initSPI() {
#if defined(__MK20DX128__) || defined(__MK20DX256__)
    // for teensy. The Cyclops DAC can communicate @ 20MHz max.
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
#else
    SPI.begin();
#endif
    // enable interrupts
    SPCR |= _BV(SPIE);
}

uint8_t sendSPI(uint8_t device_id, uint8_t data){
    if (device_id == 0 && !_busy_0){
        _busy_0 = true;
        SPDR = data;
        return 0;
    }
    /*
    else if (device_id == 1 && !_busy_1){
        _busy_1 = true;
        SPDR = data;
        return 1;
    }
    */
    return 1;
}

/*
 * The ISR just toggles the volatile ``_busy_X`` flag.
 */
ISR(SPI_STC_vect){
    uint8_t sreg = SREG;
    _busy_0 = false;
    SREG = sreg;
}