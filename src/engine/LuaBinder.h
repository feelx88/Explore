#ifndef LUABINDER_H
#define LUABINDER_H

#include "EngineTypedefs.h"
#include "LuaTools.h"
#include <luabind/luabind.hpp>

class LuaBinder;
typedef boost::shared_ptr<LuaBinder> LuaBinderPtr;

class LuaBinder
{
public:
    static void registerAll( LuaStatePtr state );

protected:
    virtual void reg( LuaStatePtr state ) = 0;

    static int registerBinder( LuaBinder *binder );

    static boost::shared_ptr<std::vector<LuaBinderPtr> > sBinders;
};

#define LUABINDER_REGISTER_MODULE_START( NAME ) class NAME : public LuaBinder\
{\
    public:\
    void reg( LuaStatePtr state )\
    {\
    using namespace luabind;\
    module( state.get() )[

#define LUABINDER_REGISTER_MODULE_END( NAME ) ];\
    }\
    private:\
    static int regDummy;\
};\
int NAME::regDummy = LuaBinder::registerBinder( new NAME );


#endif // LUABINDER_H
