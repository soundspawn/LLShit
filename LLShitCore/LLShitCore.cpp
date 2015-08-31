#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "LLShitCore.h"

ShitLoggerBase::ShitLoggerBase(){return;}
ShitLoggerBase::~ShitLoggerBase(){return;}
bool ShitLoggerBase::writeEvent(const char* t){return false;}
bool ShitLoggerBase::writeEvent(String t){return false;}
bool ShitLoggerBase::writeEvent(const __FlashStringHelper* t){return false;}
bool ShitLoggerBase::setLogPath(const char* t){return false;}
char* ShitLoggerBase::getFullCurrentLog(char* t){return NULL;}
char* ShitLoggerBase::getLogName(char* t,uint8_t t2){return NULL;}
bool ShitLoggerBase::setRTC(uint32_t t){return false;}
bool ShitLoggerBase::detectMillisRollover(uint32_t t){return false;}
bool ShitLoggerBase::detectMillisRollover(){return false;}
uint32_t ShitLoggerBase::getEventTimestamp(){return 0;}
void* ShitLoggerBase::getRecentEventList(void* t,uint8_t t2){return NULL;}
bool ShitLoggerBase::setAverageMessageLength(uint16_t t){return false;}
