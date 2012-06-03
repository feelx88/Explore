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
      mBulletWorld( explore->getBulletWorld() ),
      mCurBullet( 0 )
{
    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                "data/Entities/Player/Player.xml", *mProperties );
    mEntity.reset( new Entity( mDevice, mBulletWorld, mProperties ) );
    mCamera = static_cast<ICameraSceneNodePtr>( mEntity->getSceneNode() );
    mEntity->getRigidBody()->setSleepingThresholds( 0.f, 0.f );
    mEntity->getRigidBody()->setAngularFactor( btVector3( 0.f, 0.f, 0.f ) );

    for( int x = 0; x < 10; ++x )
    {
        mBullets[x].reset( new Entity( mDevice, mBulletWorld,
                                       "data/Entities/TestCube", "Bullet" ) );
    }

    mCrossX = mDevice->getVideoDriver()->getScreenSize().Width / 2;
    mCrossY = mDevice->getVideoDriver()->getScreenSize().Height / 2;
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
    vector3df target( 0.f, 0.f, 1.f );
    m.rotateVect( target );
    mCamera->setTarget( *( mEntity->getPosition() ) + target * 10000.f );

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

    if( mCurBullet < 10 && mEventReceiver->mouseClicked( 0 ) )
    {
        mBullets[mCurBullet]->setPosition( mEntity->getSceneNode()->getAbsolutePosition() + target );
        mBullets[mCurBullet]->getRigidBody()->setLinearVelocity( btVector3( 0, 0, 0 ) );
        mBullets[mCurBullet]->getRigidBody()->applyCentralImpulse( VectorConverter::bt( target * 50.f ) );
        ++mCurBullet;
    }
    else if( mCurBullet >= 10 )
        mCurBullet = 0;

    mDevice->getVideoDriver()->draw2DLine(
                vector2di( mCrossX - 10, mCrossY ),
                vector2di( mCrossX + 10, mCrossY ) );
    mDevice->getVideoDriver()->draw2DLine(
                vector2di( mCrossX, mCrossY - 10 ),
                vector2di( mCrossX, mCrossY + 10 ) );
}
