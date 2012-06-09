/*
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
