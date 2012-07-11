#ifndef LUABINDER_H
#define LUABINDER_H

#include "EngineTypedefs.h"
#include "LuaTools.h"

class LuaBinder;
typedef boost::shared_ptr<LuaBinder> LuaBinderPtr;

class LuaBinder
{
public:
    static void registerAll( LuaStatePtr state );

protected:
    virtual void reg( LuaStatePtr state ) = 0;

    static int registerBinder( LuaBinder *binder );

    static std::vector<LuaBinderPtr> sBinders;
};

#endif // LUABINDER_H
