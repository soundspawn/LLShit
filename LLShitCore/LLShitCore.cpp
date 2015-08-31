#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "LLShitCore.h"

bool ShitLoggerBase::writeEvent(const __FlashStringHelper* event){return false;}
bool ShitLoggerBase::writeEvent(const char* event){return false;}
bool ShitLoggerBase::writeEvent(String event){return false;}
char* ShitLoggerBase::getLogName(char* ret,uint8_t logsBack){return false;}
char* ShitLoggerBase::getLogName(char* ret){return this->getLogName(ret,0);}
bool ShitLoggerBase::getFullCurrentLog(){return false;}
bool ShitLoggerBase::getRecentEventArray(uint8_t numEvents){return false;}
bool ShitLoggerBase::setAverageMessageLength(uint16_t len){return false;}
bool ShitLoggerBase::setLogMessagesToMemorize(uint8_t numMessages){return false;}
bool ShitLoggerBase::setLogPath(const char* path){return false;}
bool ShitLoggerBase::setMassiveBufferSize(uint16_t bufferSize){return false;}
bool ShitLoggerBase::deleteUpcomingLog(){return false;}
bool ShitLoggerBase::setRTC(uint16_t timestamp){return false;}
bool ShitLoggerBase::detectMillisRollover(uint16_t mills){return false;}
bool ShitLoggerBase::detectMillisRollover(){uint16_t mills = millis();return this->detectMillisRollover(mills);}
uint32_t ShitLoggerBase::getEventTimestamp(){return false;}
