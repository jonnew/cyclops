/*
  Written by: Jon Newman
  2014.07.31
  
  Test control of the Cyclops/MCP4921
  
*/

// include the SPI library:
#include <SPI.h>
#define CS 6

// use the Serial channel?
const bool useSerial = false;         // If set to false, the board will produce a full scale triangle wave on both channels

// SPI setup
const short mcp_cs = 2;               // MCP4921 ~CS pin
const short mcp_ldac = 6;             // MCP4921 ~LDAC pin
const short DAC_config = 0x1000;      // write to dac A, do not buffer vref, 2X gain, vout on

// DAC Ch. A, B output voltages
unsigned short amplitude;           // Voltage amplitude of channel a
unsigned short v_out = 0;           // DAC Ch. A output voltage

// Serial comm. variables
int packetSize;                       // # of bytes in a serial packet
byte data[2];                         // Serial data buffer

void setup() {

  // set the slaveSelectPin as an output:
  pinMode (mcp_cs, OUTPUT);
  pinMode (mcp_ldac, OUTPUT);

  // Get the load-dac line ready
  digitalWrite(mcp_ldac, HIGH);

  // Initialize SPI bus
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();

  // Start serial port
  Serial.begin(57600);
  packetSize = sizeof(data) / sizeof(byte);


}

void loop() {

  if (useSerial)
  {
    v_out = amplitude;
    update_dac();
  }
  else
  {
    while (v_out < 4085)
    {
      v_out += 10;
      update_DAC();
      //  // DEBUG; COMMENT OUT IF THE CODE IS WORKING!
      //Serial.print(v_out, HEX);
      //Serial.print("\n");    // prints a tab
    }
    while (v_out > 10)
    {
      v_out -= 10;
      update_DAC();
      //  // DEBUG; COMMENT OUT IF THE CODE IS WORKING!
      //Serial.print(v_out, HEX);
      //Serial.print("\n");    // prints a tab
    }
  }
}

void serialEvent()
{
  if (Serial.available() < packetSize || !useSerial)
    return;

  int count = 0;
  while (Serial.available() > 0)
  {
    data[count] = Serial.read();
    count++;
  }

  amplitude = data[1] << 8 | data[0];        // Two bytes provide the pulse voltage             

//  // DEBUG; COMMENT OUT IF THE CODE IS WORKING!
//  Serial.print("D[00]: ");
//  Serial.print(data[0], HEX);
//  Serial.print(" ");    // prints a tab
//  Serial.print("D[01]: ");
//  Serial.print(data[1], HEX);
//  Serial.print("\n");    // prints a tab
}


void update_dac() {

  mcp4921_write(DAC_config, v_out & 0x0fff);  

  // Update both DAC outputs by loading their
  // output registers w/ >100 ns delay
  digitalWrite(mcp_ldac, LOW);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  digitalWrite(mcp_ldac, HIGH);
}

void mcp4921_write(int address, int data) {

  // Create data packet
  int spi_out = address | data;

  // take the SS pin low to select the chip
  digitalWrite(mcp_cs, LOW);

  //  send in the address and value via SPI:
  SPI.transfer(spi_out >> 8 & 0xff);
  SPI.transfer(spi_out >> 0 & 0xff);

  // take the SS pin high to de-select the chip:
  digitalWrite(mcp_cs, HIGH);

}


