/*
    Copyright 2012 Felix Müller.

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


#ifndef ENGINETYPEDEFS_H
#define ENGINETYPEDEFS_H

//Python include to remove warnings
#include <Python.h>

//Windows dll export/import define
#ifdef _WIN32
    #ifdef ENGINE_EXPORTS
    #define APIEXPORT __declspec(dllexport)
    #else
    #define APIEXPORT __declspec(dllimport)
    #endif

    #pragma warning(disable: 4251)
#else //_WIN32
    #define APIEXPORT
#endif

#include <boost/foreach.hpp>
#define foreach_ BOOST_FOREACH
#define foreach_r_ BOOST_REVERSE_FOREACH

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>

namespace specialDeleters
{
struct NullDeleter
{
    void operator()( void const * ) const
    {
    }
};
}

#include <boost/property_tree/ptree.hpp>

typedef boost::property_tree::ptree PropTree;
typedef boost::shared_ptr<PropTree> PropTreePtr;

#include <string>
#include <vector>
typedef std::vector<std::string> StringVector;

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
typedef irr::scene::ISceneCollisionManager* ISceneCollisionManagerPtr;

typedef irr::gui::IGUIElement* IGUIElementPtr;
typedef irr::gui::IGUIWindow* IGUIWindowPtr;
typedef irr::gui::IGUIButton* IGUIButtonPtr;
typedef irr::gui::IGUIEditBox* IGUIEditBoxPtr;
typedef irr::gui::IGUIListBox* IGUIListBoxPtr;
typedef irr::scene::ISceneNode* ISceneNodePtr;
typedef irr::scene::ICameraSceneNode* ICameraSceneNodePtr;
typedef boost::shared_ptr<irr::scene::ISceneNodeAnimator> ISceneNodeAnimatorPtr;
typedef irr::video::ITexture* ITexturePtr;

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

#include <boost/asio.hpp>

typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;
typedef boost::shared_ptr<boost::asio::ip::udp::socket> UDPSocketPtr;

class EventReceiver;

typedef boost::shared_ptr<EventReceiver> EventReceiverPtr;

#endif // ENGINETYPEDEFS_H
