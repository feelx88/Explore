#include "LoggerSingleton.h"
#include <iostream>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>

LoggerSingleton LoggerSingleton::sLogger;

void LoggerSingleton::log(const std::string &str )
{
    logPrefix();
    message( str );
    newLine();
}

LoggerSingleton::LoggerSingleton()
{
    mStreams.push_back( &std::cout );
    newLine();
    log( "Logger initialized." );
}

LoggerSingleton::~LoggerSingleton()
{
}

void LoggerSingleton::message( const std::string &message )
{
    for( StreamList::iterator x = mStreams.begin(); x != mStreams.end(); ++x  )
    {
        *( *x ) << message;
    }
}

void LoggerSingleton::newLine()
{
    for( StreamList::iterator x = mStreams.begin(); x != mStreams.end(); ++x  )
    {
        *( *x ) << std::endl;
    }
}

LoggerSingleton &LoggerSingleton::instance()
{
    return LoggerSingleton::sLogger;
}

void LoggerSingleton::addStream( std::ostream &stream )
{
    mStreams.push_back( &stream );
}

void LoggerSingleton::logPrefix()
{
    boost::posix_time::time_duration t = boost::posix_time::seconds(
                (int)time( 0 ) % ( 3600 * 24 ) );
    message( boost::posix_time::to_simple_string( t ) + " >> " );
}
