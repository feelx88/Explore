#include "LoggerSingleton.h"
#include <iostream>
#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

LoggerSingleton LoggerSingleton::sLogger;

void LoggerSingleton::Log( std::string message )
{
    *mStream << logPrefix() << message << std::endl;
}

LoggerSingleton::LoggerSingleton()
    : mStream( &std::cout )
{
    *mStream << std::endl << logPrefix() << "Logger initialized." << std::endl;
}

LoggerSingleton::~LoggerSingleton()
{
}

LoggerSingleton &LoggerSingleton::instance()
{
    return LoggerSingleton::sLogger;
}

void LoggerSingleton::setStream( std::ostream &stream )
{
    mStream = &stream;
}

std::string LoggerSingleton::logPrefix()
{
    boost::posix_time::time_duration t = boost::posix_time::seconds(
                (int)time( 0 ) % ( 3600 * 24 ) );
    return boost::posix_time::to_simple_string( t ) + " >> ";
}
