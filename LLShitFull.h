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
        char* getFullCurrentLog(char*);
        
    protected:
        char* logPath;
};

#endif
