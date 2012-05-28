#ifndef ENGINETYPEDEFS_H
#define ENGINETYPEDEFS_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/property_tree/ptree.hpp>

typedef boost::shared_ptr<boost::property_tree::ptree> PropTreePtr;

#include <irrlicht/irrlicht.h>

namespace specialDeleters
{
struct IrrlichtDeviceDeleter
{
    void operator ()( irr::IrrlichtDevice *device )
    {
        device->closeDevice();
        device->drop();
    }
};
}

typedef boost::shared_ptr<irr::IrrlichtDevice> IrrlichtDevicePtr;
typedef irr::video::IVideoDriver* IVideoDriverPtr;
typedef irr::scene::ISceneManager* ISceneManagerPtr;
typedef irr::gui::IGUIEnvironment* IGUIEnvironmentPtr;

typedef irr::gui::IGUIElement* IGUIElementPtr;
typedef irr::scene::ISceneNode* ISceneNodePtr;
typedef irr::scene::ICameraSceneNode* ICameraSceneNodePtr;
typedef boost::shared_ptr<irr::scene::ISceneNodeAnimator> ISceneNodeAnimatorPtr;

#include <bullet/btBulletDynamicsCommon.h>

typedef boost::shared_ptr<btDiscreteDynamicsWorld> BulletWorldPtr;
typedef boost::shared_ptr<btCollisionConfiguration> CollisionConfigurationPtr;
typedef boost::shared_ptr<btCollisionDispatcher> DispatcherPtr;
typedef boost::shared_ptr<btBroadphaseInterface> BroadphasePtr;
typedef boost::shared_ptr<btConstraintSolver> ConstraintSolverPtr;

namespace specialDeleters
{
struct RigidBodyDeleter
{
    RigidBodyDeleter( BulletWorldPtr world )
        : mWorld( world )
    {}

    void operator()( btRigidBody *body )
    {
        mWorld->removeRigidBody( body );
        delete body;
    }

private:
    BulletWorldPtr mWorld;
};
}

typedef boost::shared_ptr<btRigidBody> RigidBodyPtr;
typedef boost::shared_ptr<btCollisionShape> CollisionShapePtr;
typedef boost::shared_ptr<btMotionState> MotionStatePtr;

#include <lua.hpp>

namespace specialDeleters
{
struct luaStateDeleter
{
    void operator()( lua_State *state )
    {
        lua_close( state );
    }
};
}

typedef boost::shared_ptr<lua_State> LuaStatePtr;

#include "EventReceiver.h"

typedef boost::shared_ptr<EventReceiver> EventReceiverPtr;

#endif // ENGINETYPEDEFS_H
