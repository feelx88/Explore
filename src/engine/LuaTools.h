#ifndef LUATOOLS_H
#define LUATOOLS_H

#include "EngineTypedefs.h"
#include <lualib.h>

class LuaTools
{
public:
    static LuaStatePtr createLuaVM();
    static void execString(LuaStatePtr lua, const std::string &script );
};

#endif // LUATOOLS_H
