#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SD.h>
#include <Time.h>
#include "LLShitCore.h"
#include "LLShitFull.h"

#define DEBUG

namespace LLSHITFULL_STRING {
    // String Table
	enum{
	  DEFAULT_BASE_PATH,
      FULL_CURRENT_LOG_FORMAT,
      WRITE_EVENT_FORMAT,
      LLSHITFULL_STRINGS_ENUM_COUNT
	};
	const char DEFAULT_BASE_PATH_STRING[] PROGMEM = "";
    const char FULL_CURRENT_LOG_FORMAT_STRING[] PROGMEM = "%s%s";
    #ifdef DEBUG
        const char WRITE_EVENT_FORMAT_STRING[] PROGMEM = "%s - %s\t%s\n";
    #else
        const char WRITE_EVENT_FORMAT_STRING[] PROGMEM = "%s\t%s\n";
    #endif
    const char DUMMY_STRING[] PROGMEM = "";

	PGM_P const LLSHITFULL_STRING_TABLE[] PROGMEM = {
	  DEFAULT_BASE_PATH_STRING,
      FULL_CURRENT_LOG_FORMAT_STRING,
      WRITE_EVENT_FORMAT_STRING,
      DUMMY_STRING
	};
}

LLSLogger::LLSLogger(){
    this->logPath = new char[sizeof(char)*(strlen(LLSHITFULL_STRING::DEFAULT_BASE_PATH_STRING)+1)];
    strcpy_P(this->logPath, (PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::DEFAULT_BASE_PATH])));
}

LLSLogger::~LLSLogger(){
    delete(this->logPath);
}

char* LLSLogger::getLogName(char* logName, unsigned int logsBack){
    //Ignore logsBack for now

    //Ensure char array is of suitable size
    logName = (char*)realloc(logName,sizeof(char)*13);//12345678.123\n

    //Hardcode for now
    strcpy_P(logName,PSTR("0.log"));

    return logName;
}
char* LLSLogger::getLogName(char* ret){return this->getLogName(ret,0);}

char* LLSLogger::getFullCurrentLog(char* ret){
    byte nameLength;
    char* logName;
    logName = this->getLogName(logName);

    nameLength  = strlen(this->logPath);
    nameLength += strlen(logName);

    char format[strlen(LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT_STRING)+1];
    strcpy_P(format, (PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT])));
    nameLength += strlen(format);

    ret = new char[nameLength+1];
    sprintf(ret,format,this->logPath,logName);

    delete(logName);
    return ret;
}

bool LLSLogger::setLogPath(const char* newLogPath){
    this->logPath = (char*)realloc(this->logPath,strlen(newLogPath)+1*sizeof(char));
    strcpy(this->logPath,newLogPath);

    return true;
}

bool LLSLogger::writeEvent(String event){
    //Convert to char*
    char eventChar[event.length()+1];
    event.toCharArray(eventChar,event.length()+1);
    return writeEvent(eventChar);
}

bool LLSLogger::writeEvent(const __FlashStringHelper* event){
    //Convert to char*
    char eventChar[strlen_P((PGM_P)event)];
    strcpy_P(eventChar,(PGM_P)event);
    return writeEvent(eventChar);
}

bool LLSLogger::writeEvent(const char* event){
    char* logFile;
    File fileHandle;
    char timestampFormat[5];
    char timestamp[11];

    logFile = this->getFullCurrentLog(logFile);
    sprintf(timestamp,"%lu",this->getEventTimestamp());

    char format[strlen(LLSHITFULL_STRING::WRITE_EVENT_FORMAT_STRING)+1];
    strcpy_P(format,(PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::WRITE_EVENT_FORMAT])));
    #ifdef DEBUG
        char buffer[strlen(event)+strlen(format)+strlen(logFile)+strlen(timestamp)+1];
        sprintf(buffer,format,logFile,timestamp,event);
    #else
        char buffer[strlen(event)+strlen(format)+strlen(timestamp)+1];
        sprintf(buffer,format,timestamp,event);
    #endif

    //Produce event
    fileHandle = SD.open(logFile,FILE_WRITE);
    fileHandle.print(buffer);
    fileHandle.close();
    Serial.print(buffer);

    delete(logFile);
    return true;
}

/**
  * timestamp should be the current time in epoch format (seconds since 1970)
  * This saves the unix time as of our programs beginning.  We can add millis
  * to this timestamp to get to the actual time
  * If this is run past the ~59 days so millis() has rolled over, things will be off
  */
bool LLSLogger::setRTC(uint32_t timestamp){
    //Set the current timestamp
    setTime(timestamp);
    //Store the latest millis (used for rollovers)
    this->lastMillisLogged = millis();
    //We'll use seconds though to comply with epoch format
    uint32_t runtime = this->lastMillisLogged / 1000;
    //Save the offset, ignoring the current run time
    this->timeSync = timestamp - runtime;
}

/**
  * We are attempting to see when millis overflows and adjust our timeSync accordingly
  */
bool LLSLogger::detectMillisRollover(uint32_t mills){
    //millis() has rolled over, add
    if(this->lastMillisLogged > mills){
        this->timeSync += (uint32_t)-1 / 1000;
    }
    this->lastMillisLogged = mills;
    return true;
}

bool LLSLogger::detectMillisRollover(){
    uint32_t tmp = millis();
    return detectMillisRollover(tmp);
}

uint32_t LLSLogger::getEventTimestamp(){
    //Get current millis
    uint32_t mills = millis();
    //Detect rollover,
    this->detectMillisRollover(mills);
    return this->timeSync + (mills / 1000);
}
