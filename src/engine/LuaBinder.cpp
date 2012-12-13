#include "LuaBinder.h"

/*boost::shared_ptr<std::vector<LuaBinderPtr> > LuaBinder::sBinders( LuaBinder::sBinders );

void LuaBinder::registerAll( LuaStatePtr state )
{
    for( std::vector<LuaBinderPtr>::iterator x = sBinders->begin();
         x != sBinders->end(); ++x )
        ( *x )->reg( state );
}

int LuaBinder::registerBinder( LuaBinder *binder )
{
    if( !sBinders )
        sBinders.reset( new std::vector<LuaBinderPtr>() );

    sBinders->push_back( LuaBinderPtr( binder ) );
    return 0;
}*/
