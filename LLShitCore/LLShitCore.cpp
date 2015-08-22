#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "LLShitCore.h"

bool ShitLoggerBase::writeEvent(const __FlashStringHelper* event){return false;}
bool ShitLoggerBase::writeEvent(const char* event){return false;}
bool ShitLoggerBase::writeEvent(String event){return false;}
char* ShitLoggerBase::getLogName(char* ret,unsigned int logsBack){return false;}
char* ShitLoggerBase::getLogName(char* ret){return this->getLogName(ret,0);}
bool ShitLoggerBase::getFullCurrentLog(){return false;}
bool ShitLoggerBase::getRecentEventArray(byte numEvents){return false;}
bool ShitLoggerBase::setAverageMessageLength(unsigned int len){return false;}
bool ShitLoggerBase::setLogMessagesToMemorize(byte numMessages){return false;}
bool ShitLoggerBase::setLogPath(const char* path){return false;}
bool ShitLoggerBase::setMassiveBufferSize(unsigned int bufferSize){return false;}
bool ShitLoggerBase::deleteUpcomingLog(){return false;}
bool ShitLoggerBase::setFakeRTC(uint16_t timestamp){return false;}
bool ShitLoggerBase::detectMillisRollover(uint16_t mills){return false;}
bool ShitLoggerBase::detectMillisRollover(){uint16_t mills = millis();return this->detectMillisRollover(mills);}
uint32_t ShitLoggerBase::getEventTimestamp(){return false;}
