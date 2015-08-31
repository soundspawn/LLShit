#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SpawwnCore.h>
#include <SpawwnStd.h>
#include <SpawwnNet.h>
#include <LLShitCore.h>
#include <LLShitFull.h>

#define SDCARD_PIN 4
#define SDCARD_SUPPORT_PIN 10

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

  enum{
    LOG_0_FILE,
    STRING_TABLE_ENUM_COUNT
  };
  const char LOG_0_FILE_STRING[] PROGMEM = "0.log";
  const char DUMMY_STRING[] PROGMEM = "";
  PGM_P const STRING_TABLE[] PROGMEM = {
    LOG_0_FILE_STRING,
    DUMMY_STRING
  };

  #if defined(__AVR_ATmega2560__)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      7269,
      7264,
      7260,
      7252,
      7256,
      7272,
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
#define RAMMOD -18
#define ramIs(target,obj) ramIsWorker(target+RAMMOD,sizeof(obj),__LINE__)

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
  loggerSize(lls);
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
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-4,lls);//subtract size difference of logPath - "" to "logs/"
  loggerSize(lls);
  FullLoggerStringTest(lls);
}

void deleteTestLogs(){
  char buffer[13];
  strcpy_P(buffer,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_0_FILE])));
  SD.remove(buffer);
}

void dumpLog(const char* logName){
  Serial.print(logName);
  Serial.println(F(" Log Contents:"));
  Serial.println(F("---------------------------------------"));
  File fileHandle;
  fileHandle = SD.open(logName);
  if(fileHandle){
    while(fileHandle.available()){
      Serial.write(fileHandle.read());
    }
    fileHandle.close();
  }else{
    Serial.println(F("Error Opening File"));
  }
  Serial.println(F("---------------------------------------"));
}

void checkLogs(){
  char* logName = new char[13];
  strcpy_P(logName,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_0_FILE])));
  dumpLog(logName);
  delete(logName);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){;}
  pinMode(SDCARD_SUPPORT_PIN,OUTPUT);
  digitalWrite(SDCARD_SUPPORT_PIN, HIGH);
  if(!SD.begin(SDCARD_PIN)){
    Serial.println(F("**** sd init failed ****\n"));
  }else{
    deleteTestLogs();
  }

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

  checkLogs();

  Serial.println(F("Closing"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::END_IDLE],NULL);
}

void loop() {
  delay(1000);
  return;
}
