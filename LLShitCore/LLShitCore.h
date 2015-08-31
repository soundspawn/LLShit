#ifndef LLSHITCORELIB_H
#define LLSHITCORELIB_H

class ShitLoggerBase {
    public:
        bool writeEvent(const __FlashStringHelper*);
        bool writeEvent(const char*);
        bool writeEvent(String);
        char* getLogName(char*,unsigned int);
        char* getLogName(char*);
        bool getFullCurrentLog();
        bool getRecentEventArray(byte);
        bool setAverageMessageLength(unsigned int);
        bool setLogMessagesToMemorize(byte);
        bool setLogPath(const char*);
        bool setMassiveBufferSize(unsigned int);
        bool deleteUpcomingLog();
        bool setRTC(uint16_t);
        bool detectMillisRollover(uint16_t);
        bool detectMillisRollover();
        uint32_t getEventTimestamp();
};

#endif
