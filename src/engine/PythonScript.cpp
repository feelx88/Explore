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

#include <PythonScript.h>
#include <PythonTools.h>
#include <PathTools.h>
#include <fstream>

PythonScript::PythonScript( std::string script, bool isFile, bool doNotCache )
{
    boost::python::api::object main( boost::python::import( "__main__" ) );
    boost::python::api::object globalsAndLocals( main.attr( "__dict__" ) );
    construct( script, globalsAndLocals, globalsAndLocals, isFile, doNotCache );
}

PythonScript::PythonScript( std::string script, boost::python::api::object globals,
                            bool isFile, bool doNotCache )
{
    construct( script, globals, globals, isFile, doNotCache );
}

PythonScript::PythonScript( std::string script, boost::python::api::object globals,
                            boost::python::api::object locals,
                            bool isFile, bool doNotCache )
{
    construct( script, globals, locals, isFile, doNotCache );
}

void PythonScript::exec()
{
    boost::python::exec( mScript.c_str(), mGlobals, mLocals );
}

void PythonScript::construct( std::string script, boost::python::api::object globals,
                              boost::python::api::object locals,
                              bool isFile, bool doNotCache)
{
    mIsCached = !doNotCache;
    mGlobals = globals;
    mLocals = locals;

    if( isFile )
    {
        std::string fileName = PathTools::getAbsolutePath( script );
        std::ifstream file( fileName.c_str() );

        while( file.good() )
        {
            std::string tmp;
            file >> tmp;
            mScript += tmp + "\n";
        }
    }
    else
        mScript = script;
}
