#include "ExploreGame.h"
#include "Explore.h"
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

ExploreGame::ExploreGame( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mSceneManager( mDevice->getSceneManager() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    E_GAME_STATE state = EGS_GAME;

    mBulletWorld->setGravity( btVector3( 0.f, -10.f, 0.f ) );

    btTransform trans;
    trans.setOrigin( btVector3( 0.f, 10.f, 0.f ) );

    btRigidBody::btRigidBodyConstructionInfo i1(
                10.f, new btDefaultMotionState( trans ),
                new btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) ) );
    i1.m_collisionShape->calculateLocalInertia( i1.m_mass, i1.m_localInertia );
    RigidBodyPtr b1 = RigidBodyPtr( new btRigidBody( i1 ) );
    b1->applyTorqueImpulse( btVector3( 0.f, 0.f, 10.f ) );

    btRigidBody::btRigidBodyConstructionInfo i2(
                0.f, new btDefaultMotionState(),
                new btBoxShape( btVector3( 50.f, 0.5f, 50.f ) ) );
    RigidBodyPtr b2 = RigidBodyPtr( new btRigidBody( i2 ) );

    mSceneManager->addCubeSceneNode( 1.f, 0, -1, vector3df( 0.f, 0.f, 10.f ) )
            ->addAnimator( new BulletSceneNodeAnimator( mBulletWorld, b1 ) );
    mSceneManager->addCubeSceneNode( 1.f, 0, -1, vector3df( 0.f, -1.f, 0.f),
                                     vector3df(), vector3df( 100.f, 1.f, 100.f ) )
            ->addAnimator( new BulletSceneNodeAnimator( mBulletWorld, b2 ) );;
    mSceneManager->addLightSceneNode(
                mSceneManager->addCameraSceneNodeFPS( 0, 100.f, 0.01f ),
                vector3df(), SColorf( 1, 1, 1 ), 1000.f );

    mBulletWorld->addRigidBody( b1.get() );
    mBulletWorld->addRigidBody( b2.get() );

    btClock clock;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 255 ) );
        mSceneManager->drawAll();

        if( mEventReceiver->keyPressed( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            state = EGS_MAIN_MENU;
            running = false;
        }

        mBulletWorld->stepSimulation( float( clock.getTimeMilliseconds() ) / 1000.f );
        clock.reset();
        mVideoDriver->endScene();
    }

    mSceneManager->clear();

    return state;
}
