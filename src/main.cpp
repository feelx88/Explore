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

#include <engine/PythonTools.h>
#include <iostream>

int main( int argc, char** argv )
{
    StringVector args;

    for( int x = 0; x < argc; ++x )
    {
        args.push_back( argv[x] );
    }

    PythonTools::initPython( args );
    PythonTools::execFile( "init.py" );

    return 0;
}

