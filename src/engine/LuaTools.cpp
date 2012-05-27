#include "LuaTools.h"

LuaStatePtr LuaTools::createLuaVM()
{
    lua_State *state;
    luaL_openlibs( state );
    return LuaStatePtr( state, specialDeleters::luaStateDeleter() );
}
