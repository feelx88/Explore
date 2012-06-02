#include "Player.h"
#include <engine/EventReceiver.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Player::Player( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() )
{
    PropTreePtr prop( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                "data/Entities/Player/Player.xml", *prop );
    mEntity.reset( new Entity( mDevice, mBulletWorld, prop ) );
    mCamera = static_cast<ICameraSceneNodePtr>( mEntity->getSceneNode() );
    mEntity->getRigidBody()->setSleepingThresholds( 0.f, 0.f );
    mEntity->getRigidBody()->setAngularFactor( btVector3( 0.f, 0.f, 0.f ) );
}

Player::~Player()
{
}

EntityPtr Player::getEntity() const
{
    return mEntity;
}

void Player::update()
{
    vector3df rot( mEntity->getSceneNode()->getRotation() );
    rot.Z = 0.f;
    rot.X += float( mEventReceiver->mouseMoveY() ) / 10.f;
    rot.Y += float( mEventReceiver->mouseMoveX() ) / 10.f;

    mEntity->getSceneNode()->setRotation( rot );

    matrix4 m = mEntity->getSceneNode()->getAbsoluteTransformation();
    vector3df target( 0.f, 0.f, 10000.f );
    m.rotateVect( target );
    mCamera->setTarget( *( mEntity->getPosition() ) + target );

    vector3df vel( 0.f, 0.f, 0.f );

    if( mEventReceiver->keyPressed( KEY_KEY_W ) )
        vel.Z += 5;
    if( mEventReceiver->keyPressed( KEY_KEY_S ) )
        vel.Z -= 5;
    if( mEventReceiver->keyPressed( KEY_KEY_A ) )
        vel.X -= 5;
    if( mEventReceiver->keyPressed( KEY_KEY_D ) )
        vel.X += 5;

    m.rotateVect( vel );
    vel.Y = mEntity->getRigidBody()->getLinearVelocity().getY();

    if( mEventReceiver->keyPressed( KEY_SPACE ) && iszero( vel.Y ) )
        vel.Y = 5.f;

    mEntity->getRigidBody()->setLinearVelocity( VectorConverter::bt( vel ) );
}
