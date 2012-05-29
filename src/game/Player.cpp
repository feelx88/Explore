#include "Player.h"
#include <engine/LoggerSingleton.h>

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
    ISceneManagerPtr scene( mDevice->getSceneManager() );
    mCameraNode = scene->addCameraSceneNode();

    mShape.reset( new btBoxShape( btVector3( 1.f, 2.f, 1.f ) ) );
    mMotionState.reset( new btDefaultMotionState() );
    btRigidBody::btRigidBodyConstructionInfo info(
                10.f, mMotionState.get(),
                mShape.get() );
    info.m_collisionShape->calculateLocalInertia( 10.f, info.m_localInertia );

    mRigidBody.reset( new btRigidBody( info ) );
    mRigidBody->setSleepingThresholds( 0, 0  );

    mPhysics.reset( new BulletSceneNodeAnimator( mBulletWorld, mRigidBody ) );
    mCameraNode->addAnimator( mPhysics.get() );
}

Player::~Player()
{
}

RigidBodyPtr Player::getRigidBody()
{
    return mRigidBody;
}

void Player::update()
{
    if( mEventReceiver->keyPressed( KEY_KEY_W ) )
        mRigidBody->applyCentralImpulse( btVector3( 0, 0, 10 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_S ) )
        mRigidBody->applyCentralImpulse( btVector3( 0, 0, -10 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_A ) )
        mRigidBody->applyCentralImpulse( btVector3( -10, 0, 0 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_D ) )
        mRigidBody->applyCentralImpulse( btVector3( 10, 0, 0 ) );
}
