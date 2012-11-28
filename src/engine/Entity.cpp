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


#include "Entity.h"
#include "IrrlichtTools.h"
#include "BulletTools.h"
#include "PathTools.h"
#include "VectorConverter.h"
#include "QuaternionConverter.h"
#include "LoggerSingleton.h"
#include "BulletSceneNodeAnimator.h"
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

EntitySet Entity::sEntitySet;
BodyEntityMap Entity::sBodyEntityMap;
NodeEntityMap Entity::sNodeEntityMap;

Entity::Entity( IrrlichtDevicePtr device, BulletWorldPtr world,
                const PropTreePtr &properties, std::string basePath )
    : mProperties( properties ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mBulletWorld( world ),
      mSceneNode( 0 ),
      mChildNode( 0 )
{
    mBasePath = PathTools::getAbsolutePath( basePath );
    create();
}

Entity::Entity(IrrlichtDevicePtr device, BulletWorldPtr world,
               const std::string &propFileName , const std::string &nodeName )
    : mProperties( new boost::property_tree::ptree() ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mBulletWorld( world ),
      mSceneNode( 0 ),
      mChildNode( 0 )
{
    std::string fileName =
            PathTools::getAbsoluteFileNameFromFolder( propFileName, "xml" );
    mBasePath = PathTools::getBasePathFromFile( fileName );

    boost::property_tree::xml_parser::read_xml( fileName, *mProperties );

    if( !nodeName.empty() )
    {
        for( boost::property_tree::ptree::iterator x = mProperties->begin();
             x != mProperties->end(); ++x )
        {
            if( x->second.get<std::string>( "<xmlattr>.Name", "" ) != nodeName )
                mProperties->erase( x );
            else
                continue;
        }
    }

    if( !mProperties->empty() )
        create();
}

void Entity::internalCreate()
{
    preCreate();

    internalCreateSceneNode();
    internalCreateRigidBody();

    if( mSceneNode && mRigidBody )
    {
        BulletSceneNodeAnimator *ani = new BulletSceneNodeAnimator( mBulletWorld, mRigidBody );
        mAnimator.reset( ani );
        mSceneNode->addAnimator( mAnimator.get() );
    }

    postCreate();
}

void Entity::internalCreateSceneNode()
{
    std::string mesh = mProperties->get( "Entity.Node.Mesh", std::string() );

    if( mesh.empty() )
        return;

    int id = mProperties->get( "Entity.ID", -1 );

    vector3df position = mProperties->get( "Entity.Node.Position", vector3df() );
    vector3df rotation = mProperties->get( "Entity.Node.Rotation", vector3df() );
    vector3df scale = mProperties->get( "Entity.Node.Scale", vector3df( 1.f ) );

    std::string type = mProperties->get(
                "Entity.Node.Mesh.<xmlattr>.Type", std::string() );

    if( type == "Primitive" )
    {
        if( mesh == "Box" )
            mSceneNode = mSceneManager->addCubeSceneNode( 1.f, 0, id );
        else if( mesh == "Sphere" )
            mSceneNode = mSceneManager->addSphereSceneNode( 0.5f, 16, 0, id );
        else if( mesh == "Camera" )
            mSceneNode = mSceneManager->addCameraSceneNode( 0, vector3df(), vector3df(), id );
    }
    else //File
    {
        std::string fileName = PathTools::getAbsolutePath( mesh, mBasePath );

        if( type == "Octree" )
            mSceneNode = mSceneManager->addOctreeSceneNode(
                        mSceneManager->getMesh( fileName.c_str() ), 0, id );
        else
            mSceneNode = mSceneManager->addMeshSceneNode(
                        mSceneManager->getMesh( fileName.c_str() ), 0, id );
    }

    if( mSceneNode )
    {
        mSceneNode->setPosition( position );
        mSceneNode->setRotation( rotation );
        mSceneNode->setScale( scale );

        boost::optional<boost::property_tree::ptree&> textures =
                mProperties->get_child_optional( "Entity.Node.Textures" );

        if( textures )
        {
            for( boost::property_tree::ptree::iterator x = textures->begin();
                 x != textures->end(); ++x )
            {
                int layer = x->second.get( "<xmlattr>.Layer", 0 );
                std::string fileName =
                        PathTools::getAbsolutePath( x->second.data(), mBasePath );

                ITexturePtr tex =
                        mDevice->getVideoDriver()->getTexture( fileName.c_str() );
                mSceneNode->getMaterial( 0 ).setTexture( layer, tex );
            }
        }

        mSceneNode->setMaterialFlag(
                    EMF_LIGHTING, mProperties->get( "Entity.Node.Material.Lighting", true ) );

        boost::optional<SColor> color =
                mProperties->get_optional<SColor>( "Entity.Node.Material.Color" );

        if( color )
        {
            mSceneNode->getMaterial( 0 ).DiffuseColor = *color;
            mSceneNode->getMaterial( 0 ).SpecularColor = *color;
        }

        bool visible = mProperties->get( "Entity.Node.Material.Visible", true );
        mSceneNode->setVisible( visible );

        if( mProperties->get( "Entity.Node.OffsetTransform", false ) )
        {
            mChildNode = mSceneNode;
            mSceneNode = mSceneManager->addEmptySceneNode();
            mChildNode->setParent( mSceneNode );
        }
    }
}

void Entity::internalCreateRigidBody()
{
    std::string type = mProperties->get( "Entity.Body.Shape.<xmlattr>.Type", std::string() );
    if( type.empty() || type == "none" )
        return;

    internalCreateCollisionShape();

    if( !mCollisionShape )
        return;

    vector3df nullVec( 0.f, 0.f, 0.f );

    btVector3 position =
            VectorConverter::bt( mProperties->get( "Entity.Body.Position", nullVec ) );
    btVector3 rotation =
            VectorConverter::bt( mProperties->get( "Entity.Body.Rotation", nullVec ) );

    rotation *= DEGTORAD;

    float mass = mProperties->get( "Entity.Body.Mass", float( 0.f ) );
    boost::optional<vector3df> inertia =
            mProperties->get_optional<vector3df>( "Entity.Body.Inertia" );

    btTransform trans;
    trans.setIdentity();
    trans.setOrigin( position );
    trans.setRotation( btQuaternion( rotation.getY(), rotation.getX(), rotation.getZ() ) );
    mMotionState.reset( new btDefaultMotionState( trans ) );

    btRigidBody::btRigidBodyConstructionInfo info(
                mass, mMotionState.get(), mCollisionShape.get() );

    if( inertia )
        info.m_localInertia = VectorConverter::bt( *inertia );
    else
        mCollisionShape->calculateLocalInertia( mass, info.m_localInertia );

    mRigidBody.reset( new btRigidBody( info ) );

    mRigidBody->setFriction( mProperties->get( "Entity.Body.Friction", 0.5f ) );

    //TODO:Apply more settings like damping etc.

}

void Entity::internalCreateCollisionShape()
{
    std::string type = mProperties->get( "Entity.Body.Shape.<xmlattr>.Type", std::string() );

    if( type == "Primitive" )
    {
        //Primitive
        std::string childType = mProperties->get( "Entity.Body.Shape.Child.Type", std::string() );

        if( childType == "Box" )
        {
            btVector3 extents = VectorConverter::bt(
                        mProperties->get( "Entity.Body.Shape.Child.Size", vector3df() ) );
            mCollisionShape.reset( new btBoxShape( extents / 2.f ) );
        }
        else if( childType == "Sphere" )
        {
            float radius = mProperties->get( "Entity.Body.Shape.Child.Size", 0.f );
            mCollisionShape.reset( new btSphereShape( radius / 2.f ) );
        }
        else if( childType == "Cylinder" )
        {
            btVector3 extents = VectorConverter::bt(
                        mProperties->get( "Entity.Body.Shape.Child.Size", vector3df() ) );
            std::string direction = mProperties->get( "Entity.Body.Shape.Child.Direction", "Y" );

            if( direction == "X" )
                mCollisionShape.reset( new btCylinderShapeX( extents / 2.f ) );
            else if( direction == "Z" )
                mCollisionShape.reset( new btCylinderShapeZ( extents / 2.f ) );
            else
                mCollisionShape.reset( new btCylinderShape( extents / 2.f ) );
        }
    }
    else if( type == "Compound" )
    {
        //Compound
        //TODO:Compound shape
    }
    else if( type =="Mesh" )
    {
        btTriangleMesh *collisionMesh = new btTriangleMesh(); //TODO:save & delete

        IMesh *mesh = static_cast<IMeshSceneNode*>( mSceneNode )->getMesh();

        for( unsigned int x = 0; x < mesh->getMeshBufferCount(); ++x )
        {
            scene::IMeshBuffer *buffer = mesh->getMeshBuffer( x );

            unsigned short *i = buffer->getIndices();

            video::S3DVertex *v = static_cast<video::S3DVertex*>( buffer->getVertices() );

            for( unsigned int y = 0; y < buffer->getIndexCount(); y += 3 )
            {
                btVector3 p0( v[i[y + 0]].Pos.X, v[i[y + 0]].Pos.Y, v[i[y + 0]].Pos.Z );
                btVector3 p1( v[i[y + 1]].Pos.X, v[i[y + 1]].Pos.Y, v[i[y + 1]].Pos.Z );
                btVector3 p2( v[i[y + 2]].Pos.X, v[i[y + 2]].Pos.Y, v[i[y + 2]].Pos.Z );
                collisionMesh->addTriangle( p0, p1, p2 );
            }
        }

        mCollisionShape.reset( new btBvhTriangleMeshShape( collisionMesh, true ) );
    }
    else if( type == "File" )
    {
        //File
        //TODO:load shape from file
    }
    else
        return;
}

void Entity::globalInsertEntity( Entity *entity )
{
    sEntitySet.insert( entity );
    sBodyEntityMap.insert( std::make_pair( entity->getRigidBody().get(), entity ) );
    sNodeEntityMap.insert( std::make_pair( entity->getSceneNode(), entity ) );
}

void Entity::globalRemoveEntity( Entity *entity )
{
    sBodyEntityMap.erase( entity->getRigidBody().get() );
    sNodeEntityMap.erase( entity->getSceneNode() );
    sEntitySet.erase( entity );
}

Entity::~Entity()
{
    globalRemoveEntity( this );

    if( mSceneNode )
    {
        if( mAnimator )
            mSceneNode->removeAnimator( mAnimator.get() );
        mSceneNode->removeAll();
        mSceneNode->remove();
    }
}

ISceneNodePtr Entity::getSceneNode() const
{
    if( !mChildNode )
        return mSceneNode;
    else
        return mChildNode;
}

ISceneNodeAnimatorPtr Entity::getSceneNodeAnimator() const
{
    return mAnimator;
}

RigidBodyPtr Entity::getRigidBody() const
{
    return mRigidBody;
}

CollisionShapePtr Entity::getCollisionShape() const
{
    return mCollisionShape;
}

PropTreePtr Entity::getProperties() const
{
    return mProperties;
}

void Entity::setPosition( const vector3df &pos )
{
    if( mRigidBody )
    {
        btTransform trans = mRigidBody->getWorldTransform();
        trans.setOrigin( VectorConverter::bt( pos ) );
        mRigidBody->setWorldTransform( trans );
    }
    else if( mSceneNode )
    {
        mSceneNode->setPosition( pos );
    }
}

boost::optional<vector3df> Entity::getPosition() const
{
    if( mRigidBody )
    {
        btTransform trans = mRigidBody->getWorldTransform();
        return VectorConverter::irr( trans.getOrigin() );
    }
    else if( mSceneNode )
    {
        return mSceneNode->getPosition();
    }
    else
        return boost::none;
}

void Entity::setRotation( const vector3df &rot )
{
    if( mRigidBody )
    {
        btTransform trans = mRigidBody->getWorldTransform();
        matrix4 m;
        m.setRotationDegrees( rot );
        trans.setRotation( QuaternionConverter::bt( quaternion( m ) ) );
        mRigidBody->setWorldTransform( trans );
    }
    else if( mSceneNode )
    {
        mSceneNode->setRotation( rot );
    }
}

boost::optional<vector3df> Entity::getRotation() const
{
    if( mRigidBody )
    {
        btTransform trans = mRigidBody->getWorldTransform();
        return VectorConverter::irr( trans.getOrigin() );
    }
    else if( mSceneNode )
    {
        return mSceneNode->getRotation();
    }
    else
        return boost::none;
}

boost::optional<Entity*> Entity::getEntity( btRigidBody *body )
{
    BodyEntityMap::iterator it = sBodyEntityMap.find( body );

    if( it == sBodyEntityMap.end() )
        return boost::none;
    else
        return it->second;
}

boost::optional<Entity*> Entity::getEntity( ISceneNode *node )
{
    NodeEntityMap::iterator it = sNodeEntityMap.find( node );
    if( it == sNodeEntityMap.end() )
        return boost::none;
    else
        return it->second;
}

void Entity::create()
{
    internalCreate();

    globalInsertEntity( this );
}

void Entity::preCreate()
{
}

void Entity::postCreate()
{
}
