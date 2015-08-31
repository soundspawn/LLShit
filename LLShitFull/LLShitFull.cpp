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

/**
  * Return the log number (1-31) representing the date
  * If daysBack > 0 go back the appropriate number of days
  *     Rules when going back:
  *         Check for existence of log - never return a
  *             log number that fails SD.exists()
  *         Return 0 if there is no log (current day is 3, 2 doesn't exist etc)
  *         If we go back from 1, try 31, 30, and 29 without the above return 0
  *             it's possible we're coming around to Feb.  However if there's
  *             no 28-31 then we know to return 0
  */
uint8_t LLSLogger::getLogNumberOnly(uint8_t daysBack){
    uint8_t curDay;
    //Ensure we never get a 0... somehow we do when the function runs
    //  early in execution (first couple ms)
    curDay = max(1,day());
    //If we just want the standard "current log number"
    if(daysBack == 0){
        //Detect if this is a new log number relative to last logged event
        if(curDay != this->lastLogDay){
            //Reset flags if this is new
            this->lastLogDay = curDay;
            this->newLog = 1;
        }
    }else{
        char* buffer = new char;
        //We want a previous log, loop until we're back far enough
        while(daysBack > 0){
            //Go back one day
            curDay--;
            //If we need to wrap around
            if(curDay == 0){
                curDay = 31;
            }
            buffer = this->formatDateToFullLogName(buffer,curDay);
            if(!SD.exists(buffer)){
                if(curDay <= 28){
                    //See "Feb" rule in function notes
                    return 0;
                }
            }
            //Decrement
            daysBack--;
        }
        delete(buffer);
    }
    //Return the day
    return curDay;
}
uint8_t LLSLogger::getLogNumberOnly(){return this->getLogNumberOnly(0);}

char* LLSLogger::getLogName(char* logName, uint8_t date){
    //Ensure char array is of suitable size
    logName = (char*)realloc(logName,sizeof(char)*13);//12345678.123\n
    sprintf_P(logName,PSTR("%u.log"),date);

    return logName;
}

char* LLSLogger::formatDateToFullLogName(char* ret,uint8_t date){
    char* logName;
    logName = this->getLogName(logName,date);
    uint8_t nameLength;
    nameLength  = strlen(this->logPath)+14;

    char format[strlen(LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT_STRING)+1];
    strcpy_P(format, (PGM_P)pgm_read_word(&(LLSHITFULL_STRING::LLSHITFULL_STRING_TABLE[LLSHITFULL_STRING::FULL_CURRENT_LOG_FORMAT])));
    nameLength += strlen(format);

    ret = (char*)realloc(ret,sizeof(char)*nameLength);
    sprintf(ret,format,this->logPath,logName);

    delete(logName);
    return ret;
}

char* LLSLogger::getFullCurrentLog(char* ret){
    ret = this->formatDateToFullLogName(ret,this->getLogNumberOnly());
    return ret;
}

bool LLSLogger::setLogPath(const char* newLogPath){
    this->logPath = (char*)realloc(this->logPath,strlen(newLogPath)+1*sizeof(char));
    strcpy(this->logPath,newLogPath);
    if(!SD.exists(this->logPath)){
        SD.mkdir(this->logPath);
    }

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
    if(this->newLog){
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
        this->newLog = 0;
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

LLSLoggerEventList* LLSLogger::logComber(LLSLoggerEventList* list, File& logFile, uint32_t byteMax){
    char buffer[40];
    sprintf_P(buffer,PSTR("TEST"));
    list = LLSLoggerEvent::addMessage(list,buffer);
    list = LLSLoggerEvent::addMessage(list,buffer);
    //list = LLSLoggerEvent::addMessage(list,buffer);
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
    return list;
}

LLSLoggerEventList* LLSLogger::getRecentEventList(LLSLoggerEventList* list,uint8_t count){
    uint8_t daysBack = 0;
    uint8_t date;
    char* curLog;
    uint16_t readChunk;
    File logFile;
    uint32_t logSize;
    uint32_t byteOffset = 0;

    while(count > 0){
        date = this->getLogNumberOnly(daysBack);
        if(date > 0){
            //Calculate estimated length of (count) messages - avgMessageLength*count
            readChunk = count*this->avgMessageLength;
            //Open current file
            curLog = this->formatDateToFullLogName(curLog,date);
            logFile = SD.open(curLog);
            if(!logFile){
                break;
            }
            logSize = logFile.size();
            if(logSize-byteOffset < readChunk ){
                //If file size is less than calc'd value, seek to beginning of log
                // aka do nothing
            }else{
                //Seek to (byteOffset + readChunk) skipping the first portion of the file
                byteOffset += readChunk;
                logFile.seek(byteOffset);
            }
            //Call a combing function - read bytes in to string buffers splitting at newline
            list = logComber(list, logFile, readChunk);
            //If EnoughMessagesCombed (0) we're GTG
            //If endOfFileRead (1) then either go back another (calc'd bytes) or again if that's <0 for offset then BOF
            //If last call was BOF, then repeat this process but for the previous log - reset counters
            logFile.close();
        }else{
            //No log, game over man game over
            break;
        }

        //Temp
        break;
    }
    delete(curLog);
    return list;
}

LLSLoggerEventList* LLSLoggerEvent::removeMessage(LLSLoggerEventList* list,LLSLoggerEventList* target){
    LLSLoggerEventList* node = list;
    LLSLoggerEventList* trailer = NULL;
    while(node != NULL){
        if(node == target){
            if(trailer == NULL){
                //First node
                list = list->next;
                delete(node);
                break;
            }
            //Not first node
            trailer->next = node->next;
            delete(node);
            break;
        }
        trailer = node;
        node = node->next;
    }
    return list;
}

LLSLoggerEventList::LLSLoggerEventList(const char* message){
    this->message = (char*)malloc(sizeof(char)*(strlen(message)+1));
    strcpy(this->message, message);
    this->next = NULL;
}

LLSLoggerEventList::~LLSLoggerEventList(){
    delete(this->message);
}

bool LLSLoggerEvent::clearList(LLSLoggerEventList* list){
    LLSLoggerEventList* node = list->next;
    while(list != NULL){
        delete(list);
        list = node;
        node = node->next;
    }
    return true;
}

LLSLoggerEventList* LLSLoggerEvent::addMessage(LLSLoggerEventList* list,const char* message){
    if(list == NULL){
        return new LLSLoggerEventList(message);
    }

    //Place on the end
    LLSLoggerEventList* node = list;
    while(node->next != NULL){
        node = node->next;
    }
    node->next = new LLSLoggerEventList(message);

    return list;
}
