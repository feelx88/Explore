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

#include <PythonTools.h>
#include <LoggerSingleton.h>

void PythonTools::initPython( const StringVector &args )
{
    //Initialize python
    Py_Initialize();

    //import sys module to python and create argv list
    execString( "import sys\n"
                "sys.argv = []" );

    //Append each command line argument to sys.argv
    foreach_( const std::string &x, args )
    {
        std::stringstream str;
        str << "sys.argv.append( '" << x << "' )";
        execString( str.str() );
    }

    //Add current runtime path to module search path to allow importing from
    //current directory
    execString( "sys.path.append( '.' )" );
}

void PythonTools::execString( const std::string &script, bool useMainNamespace )
{
    try
    {
        if( useMainNamespace )
        {
            boost::python::api::object main( boost::python::import( "__main__" ) );
            boost::python::api::object globals = main.attr( "__dict__" );
            boost::python::exec( script.c_str(), globals, globals );
        }
        else
            boost::python::exec( script.c_str() );
    }
    catch( boost::python::error_already_set& )
    {
        _LOG( pythonErrorDescription() );
        boost::python::handle_exception();
    }
}

void PythonTools::execFile( const std::string &file, bool useMainNamespace )
{
    try
    {
        if( useMainNamespace )
        {
            boost::python::api::object main( boost::python::import( "__main__" ) );
            boost::python::api::object globals = main.attr( "__dict__" );
            boost::python::exec_file( file.c_str(), globals, globals );
        }
        else
            boost::python::exec_file( file.c_str() );
    }
    catch( boost::python::error_already_set& )
    {
        _LOG( pythonErrorDescription() );
        boost::python::handle_exception();
    }
}

std::string PythonTools::pythonErrorDescription()
{
    std::string error;
    if( PyErr_Occurred() )
    {
        PyObject *type = 0, *value = 0, *traceback = 0;
        PyErr_Fetch( &type, &value, &traceback );
        if( value )
        {
            #if PY_MAJOR_VERSION < 3
                error = std::string( PyString_AsString( value ) );
            #else
                PyObject *description = PyUnicode_AsUTF8String( value );
                if( description )
                {
                    const char *c = PyBytes_AsString( description );
                    error = std::string( c );
                }
            #endif
        }
    }
    return error;
}
