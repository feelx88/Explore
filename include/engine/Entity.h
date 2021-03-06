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


#ifndef ENTITY_H
#define ENTITY_H

#include <EngineTypedefs.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

typedef boost::unordered::unordered_map<const btRigidBody*, Entity*> BodyEntityMap;
typedef boost::unordered::unordered_map<const irr::scene::ISceneNode*, Entity*> NodeEntityMap;
typedef boost::unordered::unordered_set<Entity*> EntitySet;

class APIEXPORT Entity
{
public:

    Entity( IrrlichtDevicePtr device, BulletWorldPtr world,
            const PropTreePtr &propeties, std::string basePath = "" );
    Entity( IrrlichtDevicePtr device, BulletWorldPtr world,
            const std::string &propFileName, const std::string &nodeName = "" );
    virtual ~Entity();

    ISceneNodePtr getSceneNode() const;
    ISceneNodeAnimatorPtr getSceneNodeAnimator() const;
    CollisionObjectPtr getCollisionObject() const;
    RigidBodyPtr getRigidBody() const;
    CollisionShapePtr getCollisionShape() const;
    PropTreePtr getProperties() const;

    void setPosition( const irr::core::vector3df &pos );
    boost::optional<irr::core::vector3df> getPosition() const;

    void setRotation( const irr::core::vector3df &rot );
    boost::optional<irr::core::vector3df> getRotation() const;

    static boost::optional<Entity*> getEntity( const btRigidBody *body );
    static boost::optional<Entity*> getEntity( const irr::scene::ISceneNode *node );

protected:

    void create();
    virtual void preCreate();
    virtual void postCreate();

    PropTreePtr mProperties;

    IrrlichtDevicePtr mDevice;
    ISceneManagerPtr mSceneManager;
    BulletWorldPtr mBulletWorld;

    ISceneNodePtr mSceneNode, mChildNode;
    ISceneNodeAnimatorPtr mAnimator;
    CollisionObjectPtr mCollisionObject;
    RigidBodyPtr mRigidBody;
    CollisionShapePtr mCollisionShape;
    MotionStatePtr mMotionState;

    std::string mBasePath;

private:
    void internalCreate();
    void internalCreateSceneNode();
    void internalCreateCollisionObject();
    void internalCreateRigidBody();
    void internalCreateCollisionShape( const PropTree &subtree );

    void internalCreateCollisionFilter( const PropTree &subtree, short &group,
                                        short &mask );

    static void globalInsertEntity( Entity *entity );
    static void globalRemoveEntity( Entity *entity );

    static EntitySet sEntitySet;
    static BodyEntityMap sBodyEntityMap;
    static NodeEntityMap sNodeEntityMap;
};

#endif // ENTITY_H
