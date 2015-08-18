#ifndef LLSHITFULLLIB_H
#define LLSHITFULLLIB_H

class LLSLogger:
    public ShitLoggerBase
{
    public:
        LLSLogger();
        ~LLSLogger();
        bool writeEvent(char*);
        char* getFullCurrentLog(char*);
        
    protected:
        char* logPath;
};

#endif
