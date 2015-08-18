#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "LLShitCore.h"
#include "LLShitFull.h"

namespace LLSHITFULL_STRING {
    // String Table
	enum{
	  DEFAULT_BASE_PATH,
      LLSHITFULL_STRINGS_ENUM_COUNT
	};
	const char DEFAULT_BASE_PATH_STRING[] PROGMEM = "./";

	PGM_P const LLSHITFULL_STRING_TABLE[] PROGMEM = {
	  DEFAULT_BASE_PATH_STRING
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
    char* logName = "Test";
    this->getLogName(logName);

    nameLength  = strlen(this->logPath);
    nameLength += strlen(logName);
    ret = new char[nameLength+1];
    sprintf(ret,"%s%s",this->logPath,logName);
    return ret;
}

bool LLSLogger::writeEvent(char* event){
    char* logFile;
    logFile = this->getFullCurrentLog(logFile);
    //File myFile;

    char buffer[strlen(event)+3+strlen(logFile)+2];
    sprintf(buffer,"%s - %s",logFile,event);
    Serial.println(buffer);

    delete(logFile);
    return true;
}