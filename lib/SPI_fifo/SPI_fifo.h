#ifndef CL_SPIFIFO_H
#define CL_SPIFIFO_H

#include "avr_emulation.h"

#ifdef KINETISK

#if F_BUS == 60000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(60 / 3) * ((1+1)/2) = 20 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0))                //(60 / 2) * ((1+0)/2) = 15 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(60 / 5) * ((1+1)/2)
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1))                //(60 / 2) * ((1+0)/4) = 7.5 MHz
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0))                //(60 / 5) * ((1+0)/2)
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(2) | SPI_CTAR_DBR) //(60 / 5) * ((1+1)/6)

#elif F_BUS == 56000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(56 / 3) * ((1+1)/2) = 18.67
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0))                //(56 / 2) * ((1+0)/2) = 14
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(56 / 5) * ((1+1)/2) = 11.2
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(3) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(56 / 7) * ((1+1)/2)
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0))                //(56 / 5) * ((1+0)/2)
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(3) | SPI_CTAR_BR(0))                //(56 / 7) * ((1+0)/2)

#elif F_BUS == 48000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(48 / 2) * ((1+1)/2)
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(48 / 3) * ((1+1)/2)
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0))                //(48 / 2) * ((1+0)/2)
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(2) | SPI_CTAR_DBR) //(48 / 2) * ((1+1)/6)
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1))                //(48 / 2) * ((1+0)/4)
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(2))                //(48 / 2) * ((1+0)/6)

#elif F_BUS == 40000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(40 / 2) * ((1+1)/2) = 20
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(40 / 3) * ((1+1)/2) = 13.33
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0))                //(40 / 2) * ((1+0)/2) = 10
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(40 / 5) * ((1+1)/2)
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(3) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(40 / 7) * ((1+1)/2) = 5.71
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(1))                //(40 / 5) * ((1+0)/2)

#elif F_BUS == 36000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(36 / 2) * ((1+1)/2) = 18
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(36 / 3) * ((1+1)/2) = 12
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(36 / 3) * ((1+1)/2) = 12
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(36 / 5) * ((1+1)/2) = 7.2
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(2) | SPI_CTAR_DBR) //(36 / 2) * ((1+1)/6)
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(2) | SPI_CTAR_DBR) //(36 / 3) * ((1+1)/6)

#elif F_BUS == 24000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(24 / 2) * ((1+1)/2)  12 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(24 / 2) * ((1+1)/2)  12 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(24 / 2) * ((1+1)/2)
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(24 / 3) * ((1+1)/2)
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0))                //(24 / 2) * ((1+0)/2)
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(2) | SPI_CTAR_DBR) //(24 / 2) * ((1+1)/6)

#elif F_BUS == 16000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) | SPI_CTAR_DBR) //(16 / 2) * ((1+1)/8) = 2 MHz

#elif F_BUS == 8000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_DBR) //(8 / 2) * ((1+1)/4) = 2 MHz

#elif F_BUS == 4000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 2 MHz

#elif F_BUS == 2000000
#define HAS_CL_SPIFIFO
#define SPI_CLOCK_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz
#define SPI_CLOCK_16MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz
#define SPI_CLOCK_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz
#define SPI_CLOCK_8MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz
#define SPI_CLOCK_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz
#define SPI_CLOCK_4MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_DBR) //(4 / 2) * ((1+1)/2) = 1 MHz

#endif // F_BUS

#endif // KINETISK

/*
#! /usr/bin/perl
$clock = 60;
for $i (2, 3, 5, 7) {
        for $j (0, 1) {
                for $k (2, 4, 6, 8, 16, 32) {
                        $out = $clock / $i * (1 + $j) / $k;
                        printf "%0.2f :  ", $out;
                        print "$clock / $i * (1 + $j) / $k = $out\n";
                }
        }
}
*/

// sck = F_BUS / PBR * ((1+DBR)/BR)
//  PBR = 2, 3, 5, 7
//  DBR = 0, 1         -- zero preferred
//  BR = 2, 4, 6, 8, 16, 32, 64, 128, 256, 512


#ifdef HAS_CL_SPIFIFO

#ifndef SPI_MODE0
#define SPI_MODE0 0x00  // CPOL = 0, CPHA = 0
#define SPI_MODE1 0x04  // CPOL = 0, CPHA = 1
#define SPI_MODE2 0x08  // CPOL = 1, CPHA = 0
#define SPI_MODE3 0x0C  // CPOL = 1, CPHA = 1
#endif

#define SPI_CONTINUE 1

class CL_SPIFIFOclass
{
public:
  inline void begin(uint32_t speed, uint32_t mode=SPI_MODE0) __attribute__((always_inline)) {
    uint32_t ctar = speed;
    SIM_SCGC6 |= SIM_SCGC6_SPI0;

    KINETISK_SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_MDIS | SPI_MCR_HALT | SPI_MCR_PCSIS(0x1F);
    if (mode & 0x08){ ctar |= SPI_CTAR_CPOL;
    if (mode & 0x04) {
    }
      ctar |= SPI_CTAR_CPHA;
      ctar |= (ctar & 0x0F) << 8;
    } else {
      ctar |= (ctar & 0x0F) << 12;
    }
    KINETISK_SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
    KINETISK_SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
    
    clear();
    SPCR.enable_pins();
  }
  /**
   * @brief      Configures the Signal Multiplexer to enabls Auto-CS mode for
   *             the channel's CS pin.
   *
   * @param[in]  channel  Cyclops::Channel
   * @param[in]  cs_pin   The CS pin for channel
   */
  void setup_pcs_pin(uint8_t channel, uint8_t cs_pin){
    if (cs_pin == 10){  // PTC4
      CORE_PIN10_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (1 << 16);
    }
    if (cs_pin == 2){   // PTD0
      CORE_PIN2_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (1 << 16);
    }
    if (cs_pin == 9){   // PTC3
      CORE_PIN9_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (2 << 16);
    }
    if (cs_pin == 6){   // PTD4
      CORE_PIN6_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (2 << 16);
    }
    if (cs_pin == 20){  // PTD5
      CORE_PIN20_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (4 << 16);
    }
    if (cs_pin == 23){  // PTC2
      CORE_PIN23_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (4 << 16);
    }
    if (cs_pin == 21){  // PTD6
      CORE_PIN21_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (8 << 16);
    }
    if (cs_pin == 22){  // PTC1
      CORE_PIN22_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (8 << 16);
    }
    if (cs_pin == 15){  // PTC0
      CORE_PIN15_CONFIG = PORT_PCR_MUX(2);
      PCS_BITS_TABLE[channel] = (16 << 16);
    }
  }

  inline void write(uint8_t channel, uint32_t b, uint32_t cont=0) __attribute__((always_inline)) {
    uint32_t pcsbits = PCS_BITS_TABLE[channel];
    KINETISK_SPI0.PUSHR = (b & 0xFF) | pcsbits | (cont ? SPI_PUSHR_CONT : 0);
    while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
  }

  inline void write16(uint8_t channel, uint32_t b, uint32_t cont=0) __attribute__((always_inline)) {
    uint32_t pcsbits = PCS_BITS_TABLE[channel];
    KINETISK_SPI0.PUSHR = (b & 0xFFFF) | pcsbits | (cont ? SPI_PUSHR_CONT : 0) | SPI_PUSHR_CTAS(1);
    while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)) ;
  }

  inline uint32_t read(void) __attribute__((always_inline)) {
    while ((KINETISK_SPI0.SR & (15 << 4)) == 0) ;  // TODO, could wait forever
    return KINETISK_SPI0.POPR;
  }

  inline void clear(void) __attribute__((always_inline)) {
    KINETISK_SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
  }
private:
  static uint32_t PCS_BITS_TABLE[4];
};

extern CL_SPIFIFOclass SPI_fifo;

#endif // HAS_CL_SPIFIFO

#endif
