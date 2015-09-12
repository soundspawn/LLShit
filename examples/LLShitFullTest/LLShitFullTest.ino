#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SpawwnCore.h>
#include <SpawwnStd.h>
#include <SpawwnNet.h>
#include <Time.h>
#include <LLShit.h>

#define SDCARD_PIN 4
#define SDCARD_SUPPORT_PIN 10

namespace TESTMETRICS {
  enum{
    START_IDLE,
    FULL_LOGGER_BEGIN,
    FULL_LOGGER_INSTANCE_MADE,
    END_IDLE,
    NUM_RAM_VALUES
  };

  enum{
    LOG_1_FILE,
    LOG_22_FILE,
    STRING_TABLE_ENUM_COUNT
  };
  const char LOG_1_FILE_STRING[] PROGMEM = "1.log";
  const char LOG_22_FILE_STRING[] PROGMEM = "logs/22.log";
  const char DUMMY_STRING[] PROGMEM = "";
  PGM_P const STRING_TABLE[] PROGMEM = {
    LOG_1_FILE_STRING,
    LOG_22_FILE_STRING,
    DUMMY_STRING
  };

  #if defined(__AVR_ATmega2560__)
    const PROGMEM unsigned int ram[NUM_RAM_VALUES+1] = {
      7230,
      7196,
      7215,
      7233,
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
#define RAMMOD 0
#define ramIs(target,obj) ramIsWorker(target+RAMMOD,sizeof(obj),__LINE__)

void FullLoggerStringTest(LLSLogger &lls){
  String stringTest;
  stringTest.reserve(20);
  stringTest = F("(3) Testing Strings");
  lls.writeEvent(stringTest);
}

void FullLoggerTest(){
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_BEGIN],NULL);
  LLSLogger lls = LLSLogger();
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  // Create a ramless event via char*, normally we'd just use Flash Mem directly
  char* event = new char[20];
  strcpy_P(event,PSTR("(1) Testing Char*"));
  lls.writeEvent(event);
  delete(event);
  loggerSize(lls);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Use Flash Mem to post an event
  lls.writeEvent(F("(2) Testing Flash"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Use a String (in closure because fuck strings)
  FullLoggerStringTest(lls);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);

  //Set the log Path
  char* logPath = new char[20];
  strcpy_P(logPath,PSTR("logs/"));
  lls.setLogPath(logPath);
  delete(logPath);
  lls.setRTC(1440261234);
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-4,lls);//subtract size difference of logPath - "" to "logs/"
  loggerSize(lls);
  FullLoggerStringTest(lls);

  Serial.println(F("**** Printing last 10 event messages ****"));
  logPath = new char[20];
  strcpy_P(logPath,PSTR("/"));
  lls.setLogPath(logPath);
  delete(logPath);
  lls.setRTC(50);
  lls.setAverageMessageLength(50);
  LLSLoggerEventList* list = NULL;
  list = lls.getRecentEventList(list,3);
  LLSLoggerEventList* node = list;
  while(node != NULL){
    Serial.println(node->message);
    node = node->next;
  }
  LLSLoggerEvent::clearList(list);
  Serial.println(F("**** END ****"));

  //Force the RTC off in case we want to retest the lls object
  lls.setRTC(0);
}

void RamLoggerFiveMessagesTest(){
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_BEGIN],NULL);
  LLSLogger lls = LLSLogger();
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);
  lls.ramMode(0);
  //No change
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE],lls);
  lls.ramMode(1);
  //+10 (blank string plus pointer)
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-10,lls);
  lls.ramMode(5);
  //Increase to five... sb 50 bytes
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-50,lls);

  char* event = new char[20];
  strcpy_P(event,PSTR("(1) Testing Char*"));
  for(uint8_t i = 0; i < 20; i++){//If this is a low number... like 9... it will burn 2 bytes (wtf)
    lls.writeEvent(event);
  }
  delete(event);
  //Now 16 bytes per message (message length 18 with '\0' but old was 2), plus 10 per object
  ramIs(TESTMETRICS::ram[TESTMETRICS::FULL_LOGGER_INSTANCE_MADE]-(26*5),lls);

  Serial.println(F("Dumping Ram Log Mode"));
  LLSLoggerEventList* list = NULL;
  list = lls.getRecentEventList(list,0);
  LLSLoggerEventList* node = list;
  while(node != NULL){
    Serial.println(node->message);
    node = node->next;
  }
  LLSLoggerEvent::clearList(list);
  Serial.println(F("**** END ****"));
}

void deleteTestLogs(){
  char buffer[13];
  strcpy_P(buffer,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_1_FILE])));
  SD.remove(buffer);
  strcpy_P(buffer,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_22_FILE])));
  SD.remove(buffer);
  strcpy_P(buffer,PSTR("logs"));
  SD.rmdir(buffer);
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
  strcpy_P(logName,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_1_FILE])));
  dumpLog(logName);
  strcpy_P(logName,(PGM_P)pgm_read_word(&(TESTMETRICS::STRING_TABLE[TESTMETRICS::LOG_22_FILE])));
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
  Serial.println(F("Starting Full Logger Test"));
  FullLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  Serial.println(F("Re-running Full Logger Test"));
  FullLoggerTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);
  RamLoggerFiveMessagesTest();
  ramIs(TESTMETRICS::ram[TESTMETRICS::START_IDLE],NULL);

  checkLogs();

  Serial.println(F("Closing"));
  ramIs(TESTMETRICS::ram[TESTMETRICS::END_IDLE],NULL);
}

void loop() {
  delay(1000);
  return;
}
