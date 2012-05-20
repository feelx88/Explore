#ifndef ENGINETYPEDEFS_H
#define ENGINETYPEDEFS_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/property_tree/ptree.hpp>

typedef boost::shared_ptr<boost::property_tree::ptree> PropTreePtr;

#include <irrlicht/irrlicht.h>

typedef irr::IrrlichtDevice* IrrlichtDevicePtr;
typedef irr::video::IVideoDriver* IVideoDriverPtr;
typedef irr::scene::ISceneManager* ISceneManagerPtr;
typedef irr::gui::IGUIEnvironment* IGUIEnvironmentPtr;

typedef irr::gui::IGUIElement* IGUIElementPtr;

#include <bullet/btBulletDynamicsCommon.h>

typedef boost::shared_ptr<btDiscreteDynamicsWorld> BulletWorldPtr;
typedef boost::shared_ptr<btRigidBody> RigidBodyPtr;

#include "EventReceiver.h"

typedef boost::shared_ptr<EventReceiver> EventReceiverPtr;

#endif // ENGINETYPEDEFS_H
