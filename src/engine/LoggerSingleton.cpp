/*
    Copyright 2012 Felix Müller.

    This file is part of Explore.

    Explore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Explore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Explore.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "LoggerSingleton.h"
#include <iostream>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <luabind/luabind.hpp>

void logWrapper( const std::string &str )
{
    _LOG( str );
}

LUABINDER_REGISTER_MODULE_START( LoggerSingletonBinder )
    def( "log", logWrapper )
LUABINDER_REGISTER_MODULE_END( LoggerSingletonBinder )

LoggerSingleton *LoggerSingleton::sLogger = LoggerSingleton::sLogger;

void LoggerSingleton::log( const std::string &str )
{
    logPrefix();
    message( str );
    newLine();

    if( mScriptConsole )
    {
        irr::gui::IGUIListBox *output = mScriptConsole->mOutputBox;
        output->addItem( irr::core::stringw( str.c_str() ).c_str() );
        output->setSelected( -1 );
        mScriptConsole->mListIndex = output->getItemCount();
    }
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
    if( !sLogger )
        sLogger = new LoggerSingleton();

    return *sLogger;
}

void LoggerSingleton::addStream( std::ostream &stream )
{
    mStreams.push_back( &stream );
}

void LoggerSingleton::setScriptConsole( ScriptConsolePtr console )
{
    mScriptConsole = console;
}

void LoggerSingleton::logPrefix()
{
    boost::posix_time::time_duration t = boost::posix_time::seconds(
                (int)time( 0 ) % ( 3600 * 24 ) );
    message( boost::posix_time::to_simple_string( t ) + " >> " );
}
