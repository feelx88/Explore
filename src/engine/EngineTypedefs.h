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
typedef irr::scene::ISceneNode* ISceneNodePtr;
typedef boost::shared_ptr<irr::scene::ISceneNodeAnimator> ISceneNodeAnimatorPtr;

#include <bullet/btBulletDynamicsCommon.h>

typedef boost::shared_ptr<btDiscreteDynamicsWorld> BulletWorldPtr;
typedef boost::shared_ptr<btCollisionConfiguration> CollisionConfigurationPtr;
typedef boost::shared_ptr<btCollisionDispatcher> DispatcherPtr;
typedef boost::shared_ptr<btBroadphaseInterface> BroadphasePtr;
typedef boost::shared_ptr<btConstraintSolver> ConstraintSolverPtr;

typedef boost::shared_ptr<btRigidBody> RigidBodyPtr;
typedef boost::shared_ptr<btCollisionShape> CollisionShapePtr;
typedef boost::shared_ptr<btMotionState> MotionStatePtr;

#include "EventReceiver.h"

typedef boost::shared_ptr<EventReceiver> EventReceiverPtr;

#endif // ENGINETYPEDEFS_H
