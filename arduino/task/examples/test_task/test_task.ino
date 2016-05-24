#include <Wire.h>
#include <SPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>

#include <Task.h>

/*
 * Just create a single Task object, test all methods
 * Open the Serial Monitor before flashing this program.
 * Make sure baud rate is matched!
 */

void setup() {
  Serial.begin(57600);
  uint8_t pack[] = {0x6c, 0xa0, 0xa2, 0xff};
  //01101100
  Task t(pack, 4);

  Serial.println(t.taskID);     // 1
  Serial.println(t.commandID);  // 12
  Serial.println(t.channelID);  // 3
  for (int i=0; i<t.argsLength-1; i++){
    // this dereferencing is not task object's job
    Serial.print( *(  ((uint8_t *)t.args) + i  ));
    Serial.print(", ");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
