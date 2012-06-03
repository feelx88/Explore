#ifndef ENTITY_H
#define ENTITY_H

#include "EngineTypedefs.h"

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

class Entity
{
public:

    Entity(IrrlichtDevicePtr device, BulletWorldPtr world,
           const PropTreePtr &propeties, std::string basePath = "" );
    Entity( IrrlichtDevicePtr device, BulletWorldPtr world,
            const std::string &propFileName, const std::string &nodeName = "" );
    virtual ~Entity();

    ISceneNodePtr getSceneNode() const;
    ISceneNodeAnimatorPtr getSceneNodeAnimator() const;
    RigidBodyPtr getRigidBody() const;
    CollisionShapePtr getCollisionShape() const;
    PropTreePtr getProperties() const;

    void setPosition( const irr::core::vector3df &pos );
    boost::optional<irr::core::vector3df> getPosition() const;

    void setRotation( const irr::core::vector3df &rot );
    boost::optional<irr::core::vector3df> getRotation() const;

protected:

    void create();
    virtual void preCreate();
    virtual void postCreate();

    IrrlichtDevicePtr mDevice;
    ISceneManagerPtr mSceneManager;
    BulletWorldPtr mBulletWorld;

    PropTreePtr mProperties;
    ISceneNodePtr mSceneNode, mChildNode;
    ISceneNodeAnimatorPtr mAnimator;
    RigidBodyPtr mRigidBody;
    CollisionShapePtr mCollisionShape;
    MotionStatePtr mMotionState;

    std::string mBasePath;

private:
    void internalCreate();
    void internalCreateSceneNode();
    void internalCreateRigidBody();
    void internalCreateCollisionShape();
};

#endif // ENTITY_H
