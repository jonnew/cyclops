#include <Cyclops.h>
#include "CLTask.h"

#include "MySources.h"

cyclops::Board ch0(cyclops::board::CH0);
cyclops::Board ch1(cyclops::board::CH1);
cyclops::Board ch2(cyclops::board::CH2);
cyclops::Board ch3(cyclops::board::CH3);

// Use the objects you've defined in MySources.h
cyclops::Waveform w0(&ch0, &gen_f);
cyclops::Waveform w1(&ch1, &gen_s);
cyclops::Waveform w2(&ch2, &sto_1);
cyclops::Waveform w3(&ch3, &sto_2);

/*  
 * #####################################################################
 * #                        DONT EDIT BELOW THIS!                      #
 * #####################################################################
 */

// Instantiate a global "serial-task" Queue
cyclops::Queue processQueue;

/* +-------------------------------------------------------------------+
 * |                          Arduino Functions                        |
 * +-------------------------------------------------------------------+
 */

void launch();
void runExperiment();
void end();

void setup()
{  
  Serial.begin(115200);
  SPI_fifo.begin(SPI_CLOCK_16MHz); // 16MHz SPI clock, using pin 10 as CS
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  cyclops::AquisitionStatus = false;
}

void loop()
{
  if (cyclops::AquisitionStatus == false){
    // waiting for "launch" from PC
    cyclops::readSerialAndPush(&processQueue);
    if (processQueue.size > 0){
      cyclops::Task* t = processQueue.peek();
      t->checkAndCompute();
      processQueue.pop();
    }
  }
  
  else{
    launch();
    runExperiment();
    end();
  }
}

/* +-------------------------------------------------------------------+
 * |                            Main Functions                         |
 * +-------------------------------------------------------------------+
 */

void launch(){
  Serial.begin(115200);
  double delta = cyclops::Waveform::initAll();
  Timer1.initialize(delta);
  Timer1.attachInterrupt(cyclops::cyclops_timer_isr); // Defined in Waveform_t.h
}

void end(){
  Timer1.detachInterrupt();
  Timer1.stop();
  // ground all LEDs
  cyclops::Waveform::groundAll();
  // reset all sources, even if they are not linked to a Waveform currently
  cyclops::Source::resetAll();
}

void runExperiment(){
  while (cyclops::AquisitionStatus){
    digitalWrite(14, HIGH);
    cyclops::Waveform::processAll();
    digitalWrite(14, LOW);
    
    cyclops::readSerialAndPush(&processQueue);
    if (processQueue.size > 0){
      cyclops::Task* t = processQueue.peek();
      t->compute();
      processQueue.pop();
    }
  }
}
