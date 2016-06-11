#include <SPIFIFO.h>

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  SPIFIFO.begin(10, SPI_CLOCK_6MHz);
  //Serial.begin(115200);
}

uint8_t cnt = 0;

void loop()
{
  // Produce DC level
  SPIFIFO.write16(0x1ff0);
  
  digitalWrite(9, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(9, HIGH);
  
  delay(15);
  // Produce DC level
  SPIFIFO.write16(0x1000);
  
  digitalWrite(9, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(9, HIGH);

  delay(15);
}
