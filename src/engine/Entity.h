#ifndef ENTITY_H
#define ENTITY_H

#include "EngineTypedefs.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

typedef boost::unordered::unordered_map<btRigidBody*, Entity*> BodyEntityMap;
typedef boost::unordered::unordered_map<irr::scene::ISceneNode*, Entity*> NodeEntityMap;
typedef boost::unordered::unordered_set<Entity*> EntitySet;

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

    static boost::optional<Entity*> getEntity( btRigidBody *body );
    static boost::optional<Entity*> getEntity( irr::scene::ISceneNode *node );

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

    static void globalInsertEntity( Entity *entity );
    static void globalRemoveEntity( Entity *entity );

    static EntitySet sEntitySet;
    static BodyEntityMap sBodyEntityMap;
    static NodeEntityMap sNodeEntityMap;
};

#endif // ENTITY_H
