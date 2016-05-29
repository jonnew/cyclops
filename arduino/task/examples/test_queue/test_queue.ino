#include <Wire.h>
#include <SPI.h>
#include <nbSPI.h>
#include <Cyclops.h>
#include <RPC_defs.h>

#include <Task.h>

/*
 * Creates a Queue and verify it's operation.
 * Open the Serial Monitor before flashing this program.
 * Make sure baud rate is matched!
 */
Queue queue;
uint8_t pack0[] = {0x6c, 0x00, 0xa2, 0xf8}; //11 011100, 0000 0000, 1010 0010, 1111 1000
uint8_t pack1[] = {0x6c, 0x01, 0xa3, 0xe9}; //10 011100, 0000 0001, 1010 0011, 1110 1001
uint8_t pack2[] = {0x6c, 0x02, 0xa4, 0xda}; //10 011100, 0000 0010, 1010 0100, 1101 1010
uint8_t pack3[] = {0x6c, 0x03, 0xa5, 0xcb}; //11 011100, 0000 0011, 1010 0101, 1100 1011
uint8_t pack4[] = {0x6c, 0x04, 0xa6, 0xbc}; //01 011100, 0000 0100, 1010 0110, 1011 1100
uint8_t pack5[] = {0x6c, 0x05, 0xa7, 0xad}; //00 011100, 0000 0101, 1010 0111, 1010 1101
uint8_t pack6[] = {0x6c, 0x06, 0xa8, 0x9e}; //11 011100, 0000 0110, 1010 1000, 1001 1110
uint8_t pack7[] = {0x6c, 0x07, 0xa9, 0x8f}; //00 011100, 0000 0111, 1010 1001, 1000 1111

void setup() {
  Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("empty");
  queue.pushTask(pack3, 4);
  queue.pushTask(pack4, 4);
  Serial.println(queue.size);
 
  Task *cur = queue.peek();
  Serial.print(cur->taskID); Serial.print(cur->channelID); Serial.print(cur->commandID);
  for (uint8_t i; i<cur->argsLength; i++)
    Serial.print( *(uint8_t*)(cur->args+i) );
  Serial.println();
  queue.pop();

  queue.pushTask(pack0, 4);
  queue.pushTask(pack1, 4);
  queue.pushTask(pack2, 4);
  queue.pushTask(pack3, 4);
  queue.pushTask(pack4, 4);
  queue.pushTask(pack5, 4);
  Serial.print(queue.head);
  Serial.println(queue.size);

  queue.pushTask(pack6, 4);
  Serial.print(queue.head);
  Serial.println(queue.size);

  cur = queue.peek();
  Serial.print(cur->taskID); Serial.print(cur->channelID); Serial.print(cur->commandID);
  for (uint8_t i; i<cur->argsLength; i++)
    Serial.print( *(uint8_t*)(cur->args+i) );
  Serial.println();
  //queue.pop();

  queue.pushTask(pack7, 4);
  Serial.print(queue.head);
  Serial.println(queue.size);
}
