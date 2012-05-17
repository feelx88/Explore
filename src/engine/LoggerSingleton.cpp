#include "LoggerSingleton.h"
#include <iostream>

LoggerSingleton LoggerSingleton::sLogger;

void LoggerSingleton::Log( std::string message )
{
    *mStream << message << std::endl;
}

LoggerSingleton::LoggerSingleton()
    : mStream( &std::cout )
{
    *mStream << "Logger initialized." << std::endl;
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
