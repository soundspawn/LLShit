#ifndef LLSHITCORELIB_H
#define LLSHITCORELIB_H

class ShitLoggerBase {
	public:
        bool writeEvent(const __FlashStringHelper*);
        bool writeEvent(const char*);
        bool writeEvent(String);
        bool getLogName(char*,unsigned int);
        bool getLogName(char*);
        bool getFullCurrentLog();
        bool getRecentEventArray(byte);
        bool setAverageMessageLength(unsigned int);
        bool setLoggingParadigm(byte);
        bool setLogMessagesToMemorize(byte);
        bool setLogPath(const char*);
        bool setMassiveBufferSize(unsigned int);
        bool setConfigFile(const char*);
        bool deleteUpcomingLog();
        bool setFakeRTC(int);
};

#endif
