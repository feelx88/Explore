#include "LuaScript.h"
#include "LuaTools.h"
#include <fstream>

LuaScript::LuaScript( LuaStatePtr lua, const std::string &script, bool isFile )
    : mLuaState( lua ),
      mScript( "" )
{
    if( isFile )
    {
        std::ifstream file( script.c_str() );

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
