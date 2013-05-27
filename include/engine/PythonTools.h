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

#ifndef PYTHONTOOLS_H
#define PYTHONTOOLS_H

#include <EngineTypedefs.h>
#include <boost/python.hpp>

class APIEXPORT PythonTools
{
public:
    static void initPython();
    static void execString( const std::string &script , bool useMainNamespace = true );
    static void execFile( const std::string &file , bool useMainNamespace = true );
    static std::string pythonErrorDescription();
};

#endif // PYTHONTOOLS_H
