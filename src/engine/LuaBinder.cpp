#include "LuaBinder.h"

int LuaBinder::registerBinder( LuaBinder *binder )
{
    sBinders.push_back( LuaBinderPtr( binder ) );
}
