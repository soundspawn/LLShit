#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SpawwnCore.h>
#include <SpawwnStd.h>
#include <SpawwnNet.h>
#include <LLShitCore.h>
#include <LLShitFull.h>

namespace TESTMETRICS {
  enum{
    START_IDLE,
    BASE_LOGGER_BEGIN,
    BASE_LOGGER_INSTANCE_MADE,
    FULL_LOGGER_BEGIN,
    FULL_LOGGER_INSTANCE_MADE,
    END_IDLE,
    NUM_RAM_VALUES
  };
  #if defined(__AVR_ATmega2560__)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      7274,
      7269,
      7265,
      7265,
      7261,
      7277,
      0
    };
  #elif defined(__AVR_ATmega328P__)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      0
  };
  #else
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {};
  #endif
}

void loggerObjectSize(unsigned int objSize){
  Serial.print(F("Logger Object Size: "));
  Serial.print(objSize);
  Serial.println(F(" bytes"));
}
#define loggerSize(obj) loggerObjectSize(sizeof(obj))

bool ramIsWorker(unsigned int testValue,unsigned int objSize, unsigned long linenum){
  bool ramMatch;
  unsigned int freeRamInt = freeRam() + objSize;
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

void FullLoggerStringTest(LLSLogger &lls){
  String stringTest;
  stringTest.reserve(20);
  stringTest = F("Testing Strings");
  lls.writeEvent(stringTest);
}

void FullLoggerTest(){
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_BEGIN],NULL);
  LLSLogger lls = LLSLogger();
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  // Create a ramless event via char*, normally we'd just use Flash Mem directly
  char* event = new char[20];
  strcpy_P(event,PSTR("Testing Char*"));
  lls.writeEvent(event);
  delete(event);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Use Flash Mem to post an event
  lls.writeEvent(F("Testing Flash"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Use a String (in closure because fuck strings)
  FullLoggerStringTest(lls);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Set the log Path
  char* logPath = new char[20];
  strcpy_P(logPath,PSTR("logs/"));
  lls.setLogPath(logPath);
  delete(logPath);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-4,lls);
  loggerSize(lls);
  FullLoggerStringTest(lls);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Starting BaseLogger Test"));
  BaseLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Starting Full Logger Test"));
  FullLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Re-running Full Logger Test"));
  FullLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Closing"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::END_IDLE],NULL);
}

void loop() {
  delay(1000);
  return;
}
