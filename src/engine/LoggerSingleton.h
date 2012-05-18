#ifndef LOGGERSINGLETON_H
#define LOGGERSINGLETON_H

#include <ostream>

#define _LOG LoggerSingleton::instance().Log

class LoggerSingleton
{
public:
    static LoggerSingleton& instance();

    void setStream( std::ostream &stream );
    virtual std::string logPrefix();

    virtual void Log( std::string message );

    template <typename T>
    void Log( std::string message, const T& param )
    {
        *mStream << logPrefix() << message << ":" << param << std::endl;
    }

protected:
    LoggerSingleton();
    virtual ~LoggerSingleton();

    static LoggerSingleton sLogger;
    std::ostream *mStream;
};

#endif // LOGGERSINGLETON_H
