#include "Entity.h"
#include "IrrlichtTools.h"
#include "BulletTools.h"
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

Entity::Entity( IrrlichtDevicePtr device, BulletWorldPtr world,
                const PropTreePtr &properties )
    : mProperties( properties ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mWorld( world )
{
    create();
}

Entity::Entity(IrrlichtDevicePtr device, BulletWorldPtr world,
               const std::string &propFileName , const std::string &nodeName )
    : mProperties( new boost::property_tree::ptree() ),
      mDevice( device ),
      mSceneManager( device->getSceneManager() ),
      mWorld( world )
{
    std::string fileName = propFileName;
    if( propFileName.find( ".xml" ) == std::string::npos )
    {
        mBasePath = fileName + "/";
        std::string file = propFileName.substr( propFileName.find_last_of( '/' ) );
        fileName = propFileName + file + ".xml";
    }
    else
    {
        mBasePath = propFileName.substr( 0, propFileName.find_last_of( '/' ) ) + "/";
    }

    boost::property_tree::xml_parser::read_xml( fileName, *mProperties );

    if( !nodeName.empty() )
    {
        for( boost::property_tree::ptree::iterator x = mProperties->begin();
             x != mProperties->end(); ++x )
        {
            if( x->second.get<std::string>( "<xmlattr>.Name", "" ) != nodeName )
            {
                mProperties->erase( x );
                break;
            }
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
        BulletSceneNodeAnimator *ani = new BulletSceneNodeAnimator( mWorld, mRigidBody );
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
        std::string fileName = mBasePath + mesh;
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
                std::string fileName = mBasePath + x->second.data();

                ITexturePtr tex =
                        mDevice->getVideoDriver()->getTexture( fileName.c_str() );
                mSceneNode->getMaterial( 0 ).setTexture( layer, tex );
            }
        }

        mSceneNode->setMaterialFlag(
                    EMF_LIGHTING, mProperties->get( "Entity.Node.Material.Lighting", true ) );

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
        info.m_localInertia = VectorConverter::bt( *inertia );
    else
        mCollisionShape->calculateLocalInertia( mass, info.m_localInertia );

    mRigidBody.reset( new btRigidBody( info ) );

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
}

Entity::~Entity()
{
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
