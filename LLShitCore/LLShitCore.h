#ifndef LLSHITCORELIB_H
#define LLSHITCORELIB_H

class ShitLoggerBase {
    public:
        bool writeEvent(const __FlashStringHelper*);
        bool writeEvent(const char*);
        bool writeEvent(String);
        char* getLogName(char*,uint16_t);
        char* getLogName(char*);
        bool getFullCurrentLog();
        bool getRecentEventArray(uint8_t);
        bool setAverageMessageLength(uint16_t);
        bool setLogMessagesToMemorize(uint8_t);
        bool setLogPath(const char*);
        bool setMassiveBufferSize(uint16_t);
        bool deleteUpcomingLog();
        bool setFakeRTC(uint16_t);
        bool detectMillisRollover(uint16_t);
        bool detectMillisRollover();
        uint32_t getEventTimestamp();
};

#endif
