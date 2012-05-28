#ifndef ENTITY_H
#define ENTITY_H

#include "EngineTypedefs.h"

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

class Entity
{
public:

    static EntityPtr create( const PropTreePtr &properties );
    static EntityPtr create( const std::string &propFileName );
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
    Entity( const PropTreePtr &propeties );

    void internalCreate();
    void internalCreateSceneNode();
    void internalCreateRigidBody();
};

#endif // ENTITY_H
