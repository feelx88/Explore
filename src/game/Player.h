#ifndef PLAYER_H
#define PLAYER_H

#include <engine/EngineTypedefs.h>
#include <engine/BulletSceneNodeAnimator.h>
#include "Explore.h"

class Player
{
public:
    Player( ExplorePtr explore );
    virtual ~Player();

    RigidBodyPtr getRigidBody();

    void update();

private:
    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;

    ICameraSceneNodePtr mCameraNode;
    BulletSceneNodeAnimatorPtr mPhysics;

    RigidBodyPtr mRigidBody;
    CollisionShapePtr mShape; //TODO: ShapeCache
    MotionStatePtr mMotionState;
};

#endif // PLAYER_H
