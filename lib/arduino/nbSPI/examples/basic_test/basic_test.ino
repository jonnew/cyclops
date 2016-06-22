#include <nbSPI.h>

void setup()
{
  pinMode(6, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(6, HIGH);
  digitalWrite(10, HIGH);
  initSPI();
  Serial.begin(115200);
}

uint8_t cnt = 0;

void loop()
{
  // Produce DC level
  digitalWrite(6, LOW);
  while ( sendSPI(0, (uint8_t)(0x1f)) == 1)
    cnt++;
  while ( sendSPI(0, (uint8_t)(0xff)) == 1)
    cnt++; // gets incremented once here
  digitalWrite(6, HIGH);
  
  digitalWrite(10, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(10, HIGH);
  
  delay(500);
  // Produce DC level
  digitalWrite(6, LOW);
  while ( sendSPI(0, (uint8_t)(0x10)) == 1)
    cnt++;
  while ( sendSPI(0, (uint8_t)(0x00)) == 1)
    cnt++; // gets incremented once here
  digitalWrite(6, HIGH);
  
  digitalWrite(10, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(10, HIGH);

  Serial.println(cnt);
  cnt = 0;
  delay(500);
}




