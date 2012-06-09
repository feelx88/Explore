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


#include "LuaScript.h"
#include "LuaTools.h"
#include "PathTools.h"
#include <fstream>

LuaScript::LuaScript( LuaStatePtr lua, const std::string &script, bool isFile )
    : mLuaState( lua ),
      mScript( "" )
{
    if( isFile )
    {
        std::string fileName = PathTools::getAbsolutePath( script );
        std::ifstream file( fileName.c_str() );

        while( file.good() )
        {
            std::string tmp;
            file >> tmp;
            mScript += tmp + "\n";
        }
    }
    else
        mScript = script;
}

void LuaScript::exec()
{
    LuaTools::execString( mLuaState, mScript );
}
