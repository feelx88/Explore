#include "ExploreGame.h"
#include "Explore.h"
#include "Player.h"
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/Entity.h>

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

    PropTreePtr prop( new boost::property_tree::ptree );
    prop->put( "Entity.Node.Mesh", "$Box" );
    prop->put( "Entity.Body.Shape.<xmlattr>.Type", "Primitive" );
    prop->put( "Entity.Body.Shape.Child.Type", "Box" );

    prop->put( "Entity.Body.Mass", "0" );
    prop->put( "Entity.Node.Scale", "100,1,100" );
    prop->put( "Entity.Body.Shape.Child.Size", "100,1,100" );

    prop->put( "Entity.Body.Position", "0,-1.5,0" );
    EntityPtr ground( new Entity( mDevice, mBulletWorld, prop ) );

    prop->put( "Entity.Body.Mass", "20" );
    prop->put( "Entity.Node.Scale", "4,1,4" );
    prop->put( "Entity.Body.Shape.Child.Size", "4,1,4" );

    prop->put( "Entity.Body.Position", "20,10,20" );
    EntityPtr e1( new Entity( mDevice, mBulletWorld, prop ) );
    prop->put( "Entity.Body.Position", "-20,10,20" );
    EntityPtr e2( new Entity( mDevice, mBulletWorld, prop ) );
    prop->put( "Entity.Body.Position", "20,11,20" );
    EntityPtr e3( new Entity( mDevice, mBulletWorld, prop ) );
    prop->put( "Entity.Body.Position", "-20,11,20" );
    EntityPtr e4( new Entity( mDevice, mBulletWorld, prop ) );

    mSceneManager->addLightSceneNode( 0, vector3df( 0.f, 50.f, 0.f ),
                                      SColorf( 1, 1, 1 ), 1000.f );
    Player p( mExplore );
    p.getRigidBody()->setWorldTransform( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( 0, 5, -10 ) ) );

    btClock clock;

    while( running && mDevice->run() )
    {
        mBulletWorld->stepSimulation(
                    float( clock.getTimeMilliseconds() ) / 1000.f, 100, 1.f / 120.f );
        clock.reset();

        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 255 ) );
        mSceneManager->drawAll();

        if( mEventReceiver->keyPressed( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            state = EGS_MAIN_MENU;
            running = false;
        }

        p.update();

        mVideoDriver->endScene();
    }

    mSceneManager->clear();

    return state;
}
