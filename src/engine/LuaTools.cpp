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


#include "LuaTools.h"
#include "LoggerSingleton.h"

LuaStatePtr LuaTools::createLuaVM()
{
    lua_State *state = luaL_newstate();
    luaL_openlibs( state );
    return LuaStatePtr( state, specialDeleters::luaStateDeleter() );
}

void LuaTools::execString( LuaStatePtr lua, const std::string &script )
{
    if( luaL_dostring( lua.get(), script.c_str() ) )
    {
        std::string error = "Error";
        _LOG( error, lua_tostring( lua.get(), -1 ) );
        lua_pop( lua.get(), 1 );
    }
}

void LuaTools::execFile( LuaStatePtr lua, const std::string &fileName )
{
    if( luaL_dofile( lua.get(), fileName.c_str() ) )
    {
        std::string error = "Error in script file ";
        error += fileName;
        _LOG( error, lua_tostring( lua.get(), -1 ) );
        lua_pop( lua.get(), 1 );
    }
}
