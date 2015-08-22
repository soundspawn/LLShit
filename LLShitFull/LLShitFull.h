#ifndef LLSHITFULLLIB_H
#define LLSHITFULLLIB_H

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
        char* getLogName(char*,unsigned int);
        char* getLogName(char*);
        bool setFakeRTC(uint32_t);
        bool detectMillisRollover(uint32_t);
        bool detectMillisRollover();
        uint32_t getEventTimestamp();

    protected:
        char* logPath;
        uint32_t timeSync = 0;
        uint32_t lastMillisLogged = 0;
};

#endif
