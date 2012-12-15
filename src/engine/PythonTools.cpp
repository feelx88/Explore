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

#include "PythonTools.h"
#include "LoggerSingleton.h"

void PythonTools::initPython()
{
    Py_Initialize();
    const char *p = ".";
    PySys_SetPath( const_cast<char*>( p ) );
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

std::string PythonTools::pythonErrorDescription()
{
    std::string error;
    if( PyErr_Occurred() )
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch( &type, &value, &traceback );
        if( value )
            error = PyString_AsString( value );
    }
    return error;
}
