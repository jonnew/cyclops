#include "Task.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 *                         Task Class                           |
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 */
class Waveform;

uint8_t Task::_taskCount = 0;

Task::Task(){}

void Task::setArgs(uint8_t arg_len){
  argsLength = arg_len;
  Serial.readBytes(args, arg_len);
}

uint8_t Task::compute(){
  // Single Byte
  if (argsLength == 0){
    uint8_t chs = channelID;
    if (commandID == 3){
      //swap
      uint8_t ch_a, ch_b;
      for (ch_a=0   ; ((chs & 1) == 0); ch_a++);
      for (ch_b=ch_a; ((chs & 1) == 0); ch_b++);
      Waveform::swapChannels(Waveform::_list[ch_a], Waveform::_list[ch_b]);
      //Serial.write(0xf0);
      return 0;
    }
    else if (commandID < 3){
      for (uint8_t i=0; i<Waveform::size; i++){
        if ((chs & 1) == 0) continue;
        switch (commandID){
        case 0:
        // start
          Waveform::_list[i]->resume();
          break;
        case 1:
        // stop
          Waveform::_list[i]->pause();
          break;
        case 2:
        // reset
          Waveform::_list[i]->source->reset();
          break;
        }
        chs >>= 1;
      }
      Serial.write(Waveform::_list[0]->source->opMode);
      //Serial.write('\n');
      //Serial.write(0xf0);
      return 0;
    }
    // commandID > 3
    else{
      Serial.print(F("Teensy 3.2 on Cyclops rev3.6\n"));
      Serial.print(Waveform::size);
      return 0;
    }
  }
  // Multi Byte Command
  else{
    switch (commandID){
    case 0:
      // change_source_l
      Waveform::_list[channelID]->useSource(globalSourceList_ptr[(uint8_t)args[0]], LOOPBACK);
      break;
    case 1:
      // change_source_o
      Waveform::_list[channelID]->useSource(globalSourceList_ptr[(uint8_t)args[0]], ONE_SHOT);
      break;
    case 2:
      // change_source_n
      Waveform::_list[channelID]->useSource(globalSourceList_ptr[(uint8_t)args[0]], N_SHOT, (uint8_t)args[1]);
      break;
    case 3:
      // change_time_period
      // complex
      break;
    case 4:
      // time_factor
      break;
    case 5:
      // voltage_offset
      break;
    case 6:
      // square_on_time
      break;
    case 7:
      // square_off_time
      break;
    }
    Serial.write(Waveform::_list[channelID]->source->opMode);
    //Serial.write('\n');
    //Serial.write(0xf0);
    return 0;
  }
  return 1;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 *                         Queue Class                          |
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 */
Queue::Queue(){
  size = 0;
  head = 0;
}

uint8_t Queue::pushTask(uint8_t header, uint8_t arg_len){
  if (size < QUEUE_CAPACITY){
    uint8_t insert = head + size;
    if (insert >= QUEUE_CAPACITY)
      insert -= QUEUE_CAPACITY;
    container[insert].setArgs(arg_len);
    if (arg_len == 0){
      container[insert].channelID = EXTRACT_SB_CHANNELS(header);
      container[insert].commandID = EXTRACT_SB_CMD(header);
    }
    else{
      container[insert].channelID = EXTRACT_MB_CHANNEL(header);
      container[insert].commandID = EXTRACT_MB_CMD(header);
    }
    /*
    Serial.print("p");
    Serial.write(container[insert].commandID);
    Serial.write(container[insert].channelID);
    Serial.print("P");
    */
    size++;
    return 0;
  }
  return 1;
}

Task* Queue::peek(){
  return (size > 0)? &container[head] : NULL;
}

void Queue::pop(){
  head = (head == QUEUE_CAPACITY - 1)? 0 : head+1;
  size--;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 *                       Serial Parsing                         |
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 */

void readSerial(Queue *q){
  static uint8_t arg_len = 0, header_byte = 0;
  uint8_t avl;
  avl = Serial.available();
  while (avl){
    //Serial.print("q");
    if (arg_len == 0){
      header_byte = Serial.read();
      arg_len = getPacketSize(header_byte)-1;
      Serial.write(header_byte);
      avl--;
    }
    //Serial.print(q->size);
    if (arg_len <= avl){
      q->pushTask(header_byte, arg_len);
      //Serial.write(q->size);
      //Serial.write('\n');
      avl -= arg_len;
      arg_len = 0;
    }
    else
      break;
  }
}