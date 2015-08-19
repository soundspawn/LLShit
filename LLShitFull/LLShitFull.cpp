#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "LLShitCore.h"
#include "LLShitFull.h"
#include <SD.h>

namespace LLSHITFULL_STRING {
    // String Table
	enum{
	  DEFAULT_BASE_PATH,
      FULL_CURRENT_LOG_FORMAT,
      WRITE_EVENT_FORMAT,
      LLSHITFULL_STRINGS_ENUM_COUNT
	};
	const char DEFAULT_BASE_PATH_STRING[] PROGMEM = "./";
    const char FULL_CURRENT_LOG_FORMAT_STRING[] PROGMEM = "%s%s";
    const char WRITE_EVENT_FORMAT_STRING[] PROGMEM = "%s - %s";
    const char DUMMY_STRING[] PROGMEM = "";

	PGM_P const LLSHITFULL_STRING_TABLE[] PROGMEM = {
	  DEFAULT_BASE_PATH_STRING,
      FULL_CURRENT_LOG_FORMAT_STRING,
      WRITE_EVENT_FORMAT_STRING,
      DUMMY_STRING
	};
}

LLSLogger::LLSLogger(){
    char d[strlen(LLSHITFULL_STRING::DEFAULT_BASE_PATH_STRING)+1];
    strcpy_P(d, (PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::DEFAULT_BASE_PATH])));
    this->logPath = new char(strlen(d));
}

LLSLogger::~LLSLogger(){
    delete(this->logPath);
}

char* LLSLogger::getFullCurrentLog(char* ret){
    byte nameLength;
    char* logName;
    this->getLogName(logName);

    nameLength  = strlen(this->logPath);
    nameLength += strlen(logName);

    char format[strlen(LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT_STRING)+1];
    strcpy_P(format, (PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT])));
    nameLength += strlen(format);

    ret = new char[nameLength+1];
    sprintf(ret,format,this->logPath,logName);

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
    logFile = this->getFullCurrentLog(logFile);
    File fileHandle;

    char format[strlen(LLSHITFULL_STRING::WRITE_EVENT_FORMAT_STRING)+1];
    strcpy_P(format,(PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::WRITE_EVENT_FORMAT])));
    char buffer[strlen(event)+strlen(format)+strlen(logFile)+1];
    sprintf(buffer,format,logFile,event);
    
    //Produce event
    Serial.println(buffer);

    delete(logFile);
    return true;
}
