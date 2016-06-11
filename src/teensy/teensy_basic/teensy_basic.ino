#include <SPI.h>

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  SPI.begin();
  //Serial.begin(115200);
}

uint8_t cnt = 0;

void loop()
{
  // Produce DC level
  digitalWrite(10, LOW);
  SPI.transfer16(0x1fff);
  digitalWrite(10, HIGH);
  
  digitalWrite(9, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(9, HIGH);
  
  delay(25);
  // Produce DC level
  digitalWrite(10, LOW);
  SPI.transfer16(0x1000);
  digitalWrite(10, HIGH);
  
  digitalWrite(9, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(9, HIGH);

  delay(25);
}
