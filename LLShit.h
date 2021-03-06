#ifndef LLSHITFULLLIB_H
#define LLSHITFULLLIB_H

class LLSLoggerEventList{
    public:
        char* message;
        LLSLoggerEventList* next;
        LLSLoggerEventList(const char*);
        ~LLSLoggerEventList();
};

namespace LLSLoggerEvent {
    LLSLoggerEventList* removeMessage(LLSLoggerEventList*,LLSLoggerEventList*);
    bool clearList(LLSLoggerEventList*);
    LLSLoggerEventList* addMessage(LLSLoggerEventList*,const char*);
}

class LLSLogger
{
    public:
        LLSLogger();
        ~LLSLogger();
        bool (LLSLogger::*writeEventPtr)(const char*);
        LLSLoggerEventList* (LLSLogger::*getRecentEventListPtr)(LLSLoggerEventList*,int16_t);
        LLSLoggerEventList* getRecentEventList(LLSLoggerEventList*,int16_t);
        bool writeEvent(const char*);
        bool writeEvent(String);
        bool writeEvent(const __FlashStringHelper*);
        bool setLogPath(const char*);
        char* getFullCurrentLog(char*);
        char* getLogName(char*,uint8_t);
        bool setRTC(uint32_t);
        bool detectMillisRollover(uint32_t);
        bool detectMillisRollover();
        uint32_t getEventTimestamp();
        bool setAverageMessageLength(uint16_t);
        bool ramMode(uint8_t);
        bool fileMode();
        bool offMode();

    private:
        void loadDefaults();
        bool writeEventNone(const char*);
        LLSLoggerEventList* getRecentEventListNone(LLSLoggerEventList*,int16_t);
        bool writeEventRam(const char*);
        LLSLoggerEventList* getRecentEventListRam(LLSLoggerEventList*,int16_t);
        bool writeEventFile(const char*);
        LLSLoggerEventList* getRecentEventListFile(LLSLoggerEventList*,int16_t);
        void ClearAllLogsByWilcard(const char*,const char*);
        int wildcmp(const char*, const char*);
        uint8_t getLogNumberOnly();
        uint8_t getLogNumberOnly(uint8_t);
        char* formatDateToFullLogName(char*, uint8_t);
        LLSLoggerEventList* logComber(LLSLoggerEventList*,File&,uint32_t&,int16_t&,uint32_t,uint8_t&);

    protected:
        char *logPath;
        uint32_t timeSync;
        uint32_t lastMillisLogged;
        uint8_t newLog;
        uint8_t lastLogDay;
        uint16_t avgMessageLength;
        uint8_t ramLines;
        LLSLoggerEventList* ramList;
};

#endif
