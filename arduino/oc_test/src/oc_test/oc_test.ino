// Jon Newman
// Testing the digital overcurrent protection with an AD5245

#include <Wire.h>

float Rw            = 50;
float Rab           = 100e3; // Full digital trimmer resistance
float R             = 5.6e3; // Nominal reistance
//byte Raw;
int vIPin           = 0;
float maxCurrent    = 0.5; //Amps
int maxCurrent_int;
int currentVal;
float vRef          = 5.0; // volts
float hist          = 0.1; // Amps
int loopcount       = 0; 

void setup() {

    // Open the i2c connection
    Wire.begin();  

    // initialize serial:
    Serial.begin(9600);

    // Set the trimpot's intial resistance
    setResistance(R);

    // Convert max current to 10-bit integer
    maxCurrent_int = floor(maxCurrent * 1024.0/vRef);
    
    // Analog reference voltage (1.1 v)
    analogReference(DEFAULT);

}


void loop() {
    // Get the current output reading
    currentVal = analogRead(vIPin);
    
    if (loopcount % 1000 == 0) {
        Serial.print("Measured current (amps): "); 
        Serial.print(vRef*(float)currentVal/1024.0);
        Serial.print("\n");
    }

    if (currentVal > maxCurrent_int && R < 90e3) {
        // Overcurrent detected, engage compensation
        R+= (float)1.0e3;
        setResistance(R);
        if (R > 90e3) {
          R = 90e3;
        }

    } 
    else if (currentVal < (maxCurrent - hist) && R > 5.6e3) {
        // No overcurrent, but resistance is too high
        R-= (float)1.0e3;
        setResistance(R);
        if (R < 5.6e3) {
          R = 5.6e3;
        }
    }    

    loopcount++;
}

void setResistance(float R) { 
  // Get data register value
  byte D = (byte)floor(256.0 - 256.0*(R - 2.0*Rw)/Rab);
  //Serial.print(D);
            
  // Set the intial potentiometer value
  Wire.beginTransmission(0x2c); // transmit to device #44 (0x2c)
                              // device address is specified in datasheet
  Wire.write(0x00);            // sends instruction byte  
  Wire.write(D);             // sends potentiometer value byte  
  Wire.endTransmission();     // stop transmitting
}

