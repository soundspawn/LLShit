#ifndef LLSHITCORELIB_H
#define LLSHITCORELIB_H

class ShitLoggerBase {
    public:
        ShitLoggerBase();
        ~ShitLoggerBase();
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
        void* getRecentEventList(void*,uint8_t);
        bool setAverageMessageLength(uint16_t);
};

#endif
