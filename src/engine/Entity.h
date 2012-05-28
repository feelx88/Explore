#ifndef ENTITY_H
#define ENTITY_H

#include "EngineTypedefs.h"

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

class Entity
{
public:

    Entity( const PropTreePtr &propeties );
    Entity( const std::string &propFileName );
    virtual ~Entity();

protected:

    void create();
    virtual void preCreate();
    virtual void postCreate();

    PropTreePtr mProperties;
    ISceneNodePtr mSceneNode;
    ISceneNodeAnimatorPtr mAnimator;
    RigidBodyPtr mRigidBody;

private:
    void internalCreate();
    void internalCreateSceneNode();
    void internalCreateRigidBody();
};

#endif // ENTITY_H
