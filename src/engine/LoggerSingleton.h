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


#ifndef LOGGERSINGLETON_H
#define LOGGERSINGLETON_H

#include <ostream>
#include <list>
#include <boost/lexical_cast.hpp>

#include "ScriptConsole.h"

#define _LOG LoggerSingleton::instance().log

class APIEXPORT LoggerSingleton
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

        std::string msg = str + ": ";

        msg += boost::lexical_cast<std::string>( param );
        message( msg );
        newLine();

        if( mScriptConsole )
        {
            irr::gui::IGUIListBox *output = mScriptConsole->mOutputBox;
            output->addItem( irr::core::stringw( msg.c_str() ).c_str() );
            output->setSelected( output->getItemCount() - 1 );
        }
    }

    void setScriptConsole( ScriptConsolePtr console );

protected:
    typedef std::list<std::ostream*> StreamList;
    LoggerSingleton();
    virtual ~LoggerSingleton();

    virtual void message( const std::string &message );
    virtual void newLine();

    static LoggerSingleton *sLogger;
    StreamList mStreams;
    ScriptConsolePtr mScriptConsole;
};

#endif // LOGGERSINGLETON_H
