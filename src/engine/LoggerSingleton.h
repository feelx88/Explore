#ifndef LOGGERSINGLETON_H
#define LOGGERSINGLETON_H

#include <ostream>
#include <list>
#include <boost/lexical_cast.hpp>

#define _LOG LoggerSingleton::instance().log

class LoggerSingleton
{
public:
    static LoggerSingleton& instance();

    void addStream( std::ostream &stream );
    virtual void logPrefix();

    virtual void log(const std::string &str );

    template <typename T>
    void log( const std::string &str, const T& param )
    {
        logPrefix();
        message( str + ": " );
        message( boost::lexical_cast<std::string>( param ) );
        newLine();
    }

protected:
    typedef std::list<std::ostream*> StreamList;
    LoggerSingleton();
    virtual ~LoggerSingleton();

    virtual void message( const std::string &message );
    virtual void newLine();

    static LoggerSingleton sLogger;
    StreamList mStreams;
};

#endif // LOGGERSINGLETON_H
