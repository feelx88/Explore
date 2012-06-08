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
