#ifndef ENTITY_H
#define ENTITY_H

#include "EngineTypedefs.h"

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

class Entity
{
public:

    Entity( IrrlichtDevicePtr device, BulletWorldPtr world, const PropTreePtr &propeties );
    Entity( IrrlichtDevicePtr device, BulletWorldPtr world, const std::string &propFileName );
    virtual ~Entity();

protected:

    void create();
    virtual void preCreate();
    virtual void postCreate();

    IrrlichtDevicePtr mDevice;
    ISceneManagerPtr mSceneManager;
    BulletWorldPtr mWorld;

    PropTreePtr mProperties;
    ISceneNodePtr mSceneNode;
    ISceneNodeAnimatorPtr mAnimator;
    RigidBodyPtr mRigidBody;
    CollisionShapePtr mCollisionShape;
    MotionStatePtr mMotionState;

private:
    void internalCreate();
    void internalCreateSceneNode();
    void internalCreateRigidBody();
    void internalCreateCollisionShape();
};

#endif // ENTITY_H
