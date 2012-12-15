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

#ifndef PYTHONSCRIPT_H
#define PYTHONSCRIPT_H

#include "PythonTools.h"

class PythonScript;
typedef boost::shared_ptr<PythonScript> PythonScriptPtr;

class PythonScript
{
public:
    PythonScript( std::string script, bool isFile = true,
                  bool doNotCache = false );

    PythonScript( std::string script, boost::python::api::object globals,
                  bool isFile = true, bool doNotCache = false );

    PythonScript( std::string script, boost::python::api::object globals,
                  boost::python::api::object locals, bool isFile = true,
                  bool doNotCache = false );

    void exec();

private:
    void construct( std::string script, boost::python::api::object globals,
                    boost::python::api::object locals, bool isFile, bool doNotCache );

    bool mIsCached;
    boost::python::api::object mGlobals, mLocals;
    std::string mScript;
};

#endif // PYTHONSCRIPT_H
