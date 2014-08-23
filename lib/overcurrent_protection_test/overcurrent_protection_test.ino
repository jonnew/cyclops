// Jon Newman
// Testing the digital overcurrent protection with an AD5245

#include <Wire.h>
static int Rw   = 50;
static int Rab  = 100e3;
byte R = 5600;
byte Raw;
int vIPin = A0;
float maxCurrent = 0.5; //Amps
int maxCurrent_int;
int currentVal;


void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  
  // Set the trimpot's intial resistance
  setResistance(R);
  
  // Convert max current to 10-bit integer
  maxCurrent_int = floor((maxCurrent/5.0)*1024);
  
}

byte val = 0;

void loop()
{
  currentVal = analogRead(vIPin);
  
  if (currentVal > maxCurrent_int) 
  {
      R += 1000;
      setResistance(R);  
  }

}

void setResistance(int R)
{
  
    // Get data register value
  float D = 256.0 + 256.0*(2.0*Rw - R)/Rab;
            
  // Get the A -> W resistance
  Raw = Rab*(256.0 - floor(D))/256.0 + 2*Rw;
  
  
  // Set the intial potentiometer value
  Wire.beginTransmission(0x2D); // transmit to device #44 (0x2c)
                              // device address is specified in datasheet
  Wire.write(byte(0x00));            // sends instruction byte  
  Wire.write(Raw);             // sends potentiometer value byte  
  Wire.endTransmission();     // stop transmitting
}

