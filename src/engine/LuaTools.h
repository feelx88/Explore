#ifndef LUATOOLS_H
#define LUATOOLS_H

#include "EngineTypedefs.h"
#include <lualib.h>

class LuaTools
{
public:
    static LuaStatePtr createLuaVM();
};

#endif // LUATOOLS_H
