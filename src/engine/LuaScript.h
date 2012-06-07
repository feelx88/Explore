#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include "EngineTypedefs.h"

class LuaScript;
typedef boost::shared_ptr<LuaScript> LuaScriptPtr;

class LuaScript
{
public:
    LuaScript(LuaStatePtr lua, const std::string &script, bool isFile = true );

    void exec();

private:
    LuaStatePtr mLuaState;
    std::string mScript;
};

#endif // LUASCRIPT_H
