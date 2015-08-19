//#define DEBUG

#define TESTBOARD_MEGA
//#define TESTBOARD_UNO

#include "Arduino.h"
#include <SpawwnCore.h>
#include <SpawwnStd.h>
#include <LLShitCore.h>

namespace TESTMETRICS {
  enum{
    START_IDLE,
    BASE_LOGGER_BEGIN,
    BASE_LOGGER_INSTANCE_MADE,
    END_IDLE,
    NUM_RAM_VALUES
  };
  #if defined(TESTBOARD_MEGA)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      7976,
      7971,
      7967,
      7979,
      0
    };
  #elif defined(TESTBOARD_UNO)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      0
  };
  #else
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {};
  #endif
}

bool ramIsWorker(unsigned int testValue,unsigned int objSize, unsigned long linenum){
  bool ramMatch;
  unsigned int freeRamInt = freeUnfragRam() + objSize;
  ramMatch = (freeRamInt == testValue);
  if(ramMatch){
    Serialprint("Line Number %lu: Ram match @ %u\n",linenum,testValue);
    return true;
  }else{
    Serialprint("Line Number %lu: **** RAM FAILURE, SB %u - WAS %u\n",linenum,testValue,freeRamInt);
    return false;
  }
}
#define ramIs(target,obj) ramIsWorker(target,sizeof(obj),__LINE__)

void BaseLoggerTest(){
  ShitLoggerBase *lls;
  ramIs(TESTMETRICS::ram[TESTMETRICS::BASE_LOGGER_BEGIN],lls);
  lls = new ShitLoggerBase();
  ramIs(TESTMETRICS::ram[TESTMETRICS::BASE_LOGGER_INSTANCE_MADE],lls);
  delete(lls);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Starting BaseLogger Test"));
  BaseLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Re-running BaseLogger Test"));
  BaseLoggerTest();
  Serial.println(F("Closing"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::END_IDLE],NULL);
}

void loop() {
  delay(1000);
  return;
}
