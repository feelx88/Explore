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

    CollisionShapePtr s1( new btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) ) );
    btTransform trans;
    trans.setOrigin( btVector3( 0.f, 10.f, 0.f ) );
    MotionStatePtr ms1( new btDefaultMotionState( trans ) );
    btRigidBody::btRigidBodyConstructionInfo i1(
                10.f, ms1.get(),
                s1.get() );
    i1.m_collisionShape->calculateLocalInertia( i1.m_mass, i1.m_localInertia );
    RigidBodyPtr b1( new btRigidBody( i1 ) );

    ISceneNodeAnimatorPtr anim1( new BulletSceneNodeAnimator( mBulletWorld, b1 ) );
    mSceneManager->addCubeSceneNode( 1.f, 0, -1, vector3df( 0.f, 0.f, 10.f ) )
            ->addAnimator( anim1.get() );

    CollisionShapePtr s2( new btBoxShape( btVector3( 50.f, 0.5f, 50.f ) ) );
    MotionStatePtr ms2( new btDefaultMotionState() );
    btRigidBody::btRigidBodyConstructionInfo i2(
                0.f, ms2.get(),
                s2.get() );
    RigidBodyPtr b2( new btRigidBody( i2 ) );

    ISceneNodeAnimatorPtr anim2( new BulletSceneNodeAnimator( mBulletWorld, b2 ) );
    mSceneManager->addCubeSceneNode( 1.f, 0, -1, vector3df( 0.f, -1.f, 0.f),
                                     vector3df(), vector3df( 100.f, 1.f, 100.f ) )
            ->addAnimator( anim2.get() );

    SKeyMap keyMap[4];
    keyMap[0].Action = EKA_MOVE_FORWARD;
    keyMap[0].KeyCode = KEY_KEY_W;
    keyMap[1].Action = EKA_MOVE_BACKWARD;
    keyMap[1].KeyCode = KEY_KEY_S;
    keyMap[2].Action = EKA_STRAFE_LEFT;
    keyMap[2].KeyCode = KEY_KEY_A;
    keyMap[3].Action = EKA_STRAFE_RIGHT;
    keyMap[3].KeyCode = KEY_KEY_D;

    ISceneNodePtr camera = mSceneManager->addCameraSceneNodeFPS( 0, 100.f, 0.01f, -1, keyMap, 4 );
    camera->setPosition( vector3df( 0.f, 5.f, -10.f ) );

    mSceneManager->addLightSceneNode( camera, vector3df(), SColorf( 1, 1, 1 ), 1000.f );

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
