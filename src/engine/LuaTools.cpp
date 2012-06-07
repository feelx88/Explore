#include "LuaTools.h"

LuaStatePtr LuaTools::createLuaVM()
{
    lua_State *state = luaL_newstate();
    luaL_openlibs( state );
    return LuaStatePtr( state, specialDeleters::luaStateDeleter() );
}

void LuaTools::execString( LuaStatePtr lua, const std::string &script )
{
    luaL_dostring( lua.get(), script.c_str() );
}
