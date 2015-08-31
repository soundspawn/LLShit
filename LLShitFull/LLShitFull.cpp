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

uint8_t LLSLogger::getLogNumberOnly(){
    uint8_t curDay = max(1,day());
    if(curDay != this->lastLogDay){
        this->lastLogDay = curDay;
        this->freshBoot = 1;
    }
    return curDay;
}

char* LLSLogger::getLogName(char* logName, uint16_t logsBack){
    //Ignore logsBack for now

    //Ensure char array is of suitable size
    logName = (char*)realloc(logName,sizeof(char)*13);//12345678.123\n

    sprintf_P(logName,PSTR("%u.log"),this->getLogNumberOnly());

    return logName;
}
char* LLSLogger::getLogName(char* ret){return this->getLogName(ret,0);}

char* LLSLogger::getFullCurrentLog(char* ret){
    uint8_t nameLength;
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
    SD.mkdir(this->logPath);

    return true;
}

int LLSLogger::wildcmp(const char *wild, const char *string){
    const char *cp = NULL, *mp = NULL;

    while((*string) && (*wild != '*')){
        if((*wild != *string) && (*wild != '?')){
            return 0;
        }
        wild++;
        string++;
    }

    while(*string){
        if(*wild == '*'){
            if(!*++wild){
                return 1;
            }
            mp = wild;
            cp = string+1;
        }else if((*wild == *string) || (*wild == '?')){
            wild++;
            string++;
        }else{
            wild = mp;
            string = cp++;
        }
    }
    while(*wild == '*'){
        wild++;
    }
    return !*wild;
}

void LLSLogger::ClearAllLogsByWilcard(const char* root,const char* target){
    File dir = SD.open(root);
    char curFile[13];
    char buffer[strlen(root)+14];

    while(true){
        File entry = dir.openNextFile();
        if(!entry){
            dir.close();
            return;
        }
        if(!entry.isDirectory()){
            strncpy(curFile,entry.name(),13);
            if(this->wildcmp(target,curFile)){
                sprintf_P(buffer,PSTR("%s%s"),root,curFile);
                SD.remove(buffer);
            }
        }
        entry.close();
    }
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
    sprintf_P(timestamp,PSTR("%lu"),this->getEventTimestamp());

    //First log message to new file (first boot or date change)
    if(this->freshBoot){
        //Clear out future logs
        char incrementString[13];
        uint8_t logNum = this->getLogNumberOnly();
        //28th or beyond let's wipe through the 31st
        if(logNum > 27){
            //From "tomorrow" through the 31st
            for(uint8_t i = logNum+1;i<=31;i++){
                sprintf_P(incrementString,PSTR("%u.LOG"),i);
                this->ClearAllLogsByWilcard(this->logPath,incrementString);
            }
            //Plan on clearing the 1st as well (this could be the last day of the month)
            logNum = 1;
        }else{
            //The real "tomorrow"
            logNum++;
        }
        sprintf_P(incrementString,PSTR("%u.LOG"),logNum);
        this->ClearAllLogsByWilcard(this->logPath,incrementString);
        this->freshBoot = 0;
    }

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

/**
  * Sets the loggers expectation of message length
  * This is not strictly required but optimizes the number of log scans needed
  *     to retrieve the desired number of messages
  */
bool LLSLogger::setAverageMessageLength(uint16_t avgLength){
    this->avgMessageLength = avgLength;
    return true;
}

bool LLSLogger::getRecentEventArray(uint8_t count){
    //Open current file
    //Calculate estimated length of (count) messages - avgMessageLength*count
    //If file size is less than calc'd value, seek to beginning of log
    //  Else seek to (log_length - calc) skipping the first portion of the file
    //Save this seek to byte offset (byteStart)
    //Call a combing function - read bytes in to string buffers (char*?) splitting at newline
    //  Function needs to receive the endOfFileRead byte offset.  Either EOF or when re-calling
    //      this function the point we started last time
    //  If seeked to beginning - save first message (byte 0 to first newline)
    //      Else skip first message (assume we were midway in to the message)
    //  Each newline decrease the remaining messages to comb (decrease count?)
    //  Return on endOfFileRead - either EOF or hit a specified offset
    //      If specified offset, go forward to EOF or next newline first (capture that partial message)
    //  Return on EnoughMessagesCombed - make sure to keep reading to endOfFileRead and if there are more
    //      messages, keep shifting off the oldest.  The goal is not to stop when enough are hit, but to
    //      get the last X messages, so we have to make sure we don't stop short and get messages 1-10 of 11
    //If EnoughMessagesCombed we're GTG
    //If endOfFileRead then either go back another (calc'd bytes) or again if that's <0 for offset then BOF
    //If last call was BOF, then repeat this process but for the previous log
}
