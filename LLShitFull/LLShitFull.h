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

class LLSLogger:
    public ShitLoggerBase
{
    public:
        LLSLogger();
        ~LLSLogger();
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
        LLSLoggerEventList* getRecentEventList(LLSLoggerEventList*,uint8_t);
        bool setAverageMessageLength(uint16_t);

    private:
        void ClearAllLogsByWilcard(const char*,const char*);
        int wildcmp(const char*, const char*);
        uint8_t getLogNumberOnly();
        uint8_t getLogNumberOnly(uint8_t);
        char* formatDateToFullLogName(char*, uint8_t);
        LLSLoggerEventList* logComber(LLSLoggerEventList*,File&,uint32_t);

    protected:
        char *logPath;
        uint32_t timeSync = 0;
        uint32_t lastMillisLogged = 0;
        uint8_t newLog = 1;
        uint8_t lastLogDay = 0;
        uint16_t avgMessageLength = 50;
};

#endif
