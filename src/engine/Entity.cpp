#include "Entity.h"
#include "IrrlichtTools.h"
#include "BulletTools.h"
#include "VectorConverter.h"
#include "LoggerSingleton.h"
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Entity::Entity( IrrlichtDevicePtr device, BulletWorldPtr world, const PropTreePtr &properties )
    : mProperties( properties ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mWorld( world )
{
    create();
}

Entity::Entity(IrrlichtDevicePtr device, BulletWorldPtr world, const std::string &propFileName )
    : mProperties( new boost::property_tree::ptree() ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mWorld( world )
{
    boost::property_tree::xml_parser::read_xml( propFileName, *mProperties );
    create();
}

void Entity::internalCreate()
{
    preCreate();

    internalCreateSceneNode();
    internalCreateRigidBody();

    postCreate();
}

void Entity::internalCreateSceneNode()
{
    std::string mesh = mProperties->get( "Entity.Node.Mesh", std::string() );

    if( mesh.empty() )
        return;

    vector3df position = mProperties->get( "Entity.Node.Position", vector3df() );
    vector3df rotation = mProperties->get( "Entity.Node.Rotation", vector3df() );
    vector3df scale = mProperties->get( "Entity.Node.Scale", vector3df( 1.f ) );

    if( mesh.at( 0 ) == '$' )
    {
        //Primitive
        mesh = mesh.substr( 1 );

        if( mesh == "Box" )
            mSceneNode = mSceneManager->addCubeSceneNode( 1.f );
        else if( mesh == "Sphere" )
            mSceneNode = mSceneManager->addSphereSceneNode( 0.5f );
    }
    else //File
        mSceneNode = mSceneManager->addMeshSceneNode( mSceneManager->getMesh( mesh.c_str() ) );

    if( mSceneNode )
    {
        mSceneNode->setPosition( position );
        mSceneNode->setRotation( rotation );
        mSceneNode->setScale( scale );
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
    float mass = mProperties->get( "Entity.Body.Mass", float( 0.f ) );
    boost::optional<vector3df> inertia =
            mProperties->get_optional<vector3df>( "Entity.Body.Inertia" );

    btTransform trans;
    trans.setIdentity();
    trans.setOrigin( position );
    trans.setRotation( btQuaternion( rotation.getZ(), rotation.getY(), rotation.getX() ) );
    mMotionState.reset( new btDefaultMotionState( trans ) );

    btRigidBody::btRigidBodyConstructionInfo info(
                mass, mMotionState.get(), mCollisionShape.get() );

    if( inertia )
    {
        info.m_localInertia = VectorConverter::bt( *inertia );
    }

    mRigidBody.reset( new btRigidBody( info ) );

}

void Entity::internalCreateCollisionShape()
{
    std::string type = mProperties->get( "Entity.Body.Shape.<xmlattr>.Type", std::string() );

    //No test, has been tested before

    btVector3 scale =
            VectorConverter::bt( mProperties->get( "Entity.Body.Scale", vector3df() ) );

    if( type == "Primitive" )
    {
        //Primitive
        std::string childType = mProperties->get( "Entity.Body.Shape.Child.Type", std::string() );

        if( childType == "Box" )
        {
            btVector3 extents = VectorConverter::bt(
                        mProperties->get( "Entity.Body.Shape.Child.Size", vector3df() ) );
            mCollisionShape.reset( new btBoxShape( extents ) );
            return;
        }
        else if( childType == "Sphere" )
        {
            float radius = mProperties->get( "Entity.Body.Shape.Child.Size", 0.f );
            mCollisionShape.reset( new btSphereShape( radius ) );
            return;
        }
    }
    else if( type == "Compound" )
    {
        //Compound
        //TODO:Compound shape
    }
    else if( type == "File" )
    {
        //File
        //TODO:load shape from file
    }
    else
        return;

    mCollisionShape->setLocalScaling( scale );
}

Entity::~Entity()
{
}

void Entity::create()
{
    internalCreate();
}

void Entity::preCreate()
{
    _LOG( "Empty Entity::preCreate()-stub" );
}

void Entity::postCreate()
{
    _LOG( "Empty Entity::postCreate()-stub" );
}
