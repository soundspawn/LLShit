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
        char* getLogName(char*,uint16_t);
        char* getLogName(char*);
        bool setRTC(uint32_t);
        bool detectMillisRollover(uint32_t);
        bool detectMillisRollover();
        uint32_t getEventTimestamp();
        bool getRecentEventArray(uint8_t);
        bool setAverageMessageLength(uint16_t);

    private:
        void ClearAllLogsByWilcard(const char*,const char*);
        int wildcmp(const char*, const char*);
        uint8_t getLogNumberOnly();

    protected:
        char *logPath;
        uint32_t timeSync = 0;
        uint32_t lastMillisLogged = 0;
        uint8_t freshBoot = 1;
        uint8_t lastLogDay = 0;
        uint16_t avgMessageLength = 50;
};

#endif
