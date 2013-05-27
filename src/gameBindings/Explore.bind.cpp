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

#include <engine/PythonBinder.h>
#include <Explore.h>
#include <ExploreGame.h>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

PYTHONBINDER_REGISTER_MODULE( Explore )
{
    using namespace boost::python;

    class_<StringVector>( "StringVector" )
            .def( vector_indexing_suite<StringVector>() );

    class_<Explore, boost::noncopyable>( "Explore", init<StringVector>() )
        .def( "readConfigValue", &Explore::readConfigValue<std::string> )
        .def( "writeConfigValue", &Explore::writeConfigValue<std::string> )
        .def( "saveConfig", &Explore::saveConfig )
        .def( "getExploreGame", &Explore::getExploreGame )
        .def( "getExploreServer", &Explore::getExploreServer )
        .def( "getKeyCode", &Explore::getKeyCode )
            .staticmethod( "getKeyCode" )
        .def( "run", &Explore::run )
        .def( "setGameState", &Explore::setGameState );
    enum_<E_GAME_STATE>( "E_GAME_STATE" )
        .value( "EGS_QUIT", EGS_QUIT )
        .value( "EGS_MAIN_MENU", EGS_MAIN_MENU )
        .value( "EGS_CONNECT", EGS_CONNECT )
        .value( "EGS_LOAD", EGS_LOAD )
        .value( "EGS_GAME", EGS_GAME )
        .export_values();
}

BOOST_PYTHON_MODULE( ExploreBind )
{
    PythonBinder::registerAll();
}
