#include "ExploreGame.h"
#include "Explore.h"
#include "Player.h"
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/Entity.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#define EPATH std::string( "data/Entities/" )

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

    mEventReceiver->lockMouse( true );

    EntityPtr e[10];
    for( int x = 0; x < 10; ++x )
    {
        e[x].reset( new Entity( mDevice, mBulletWorld, EPATH + "TestCube" ) );
        e[x]->setPosition( vector3df( 0.f, 1.f + x, 10.f ) );
    }

    EntityPtr ground( new Entity( mDevice, mBulletWorld,
                                  EPATH + "TestCube", "Floor" ) );

    mSceneManager->addLightSceneNode( 0, vector3df( 0.f, 50.f, 0.f ),
                                      SColorf( 1, 1, 1 ), 1000.f );
    Player p( mExplore );
    p.getEntity()->setPosition( vector3df( 0.f, 5.f, -10.f ) );

    btClock clock;

    while( running && mDevice->run() )
    {
        mBulletWorld->stepSimulation(
                    float( clock.getTimeMilliseconds() ) / 1000.f, 100, 1.f / 120.f );
        clock.reset();

        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 255 ) );
        mSceneManager->drawAll();

        if( mEventReceiver->keyClicked( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            state = EGS_MAIN_MENU;
            running = false;
        }

        p.update();

        //mBulletWorld->debugDrawWorld();

        mVideoDriver->endScene();

        mDevice->setWindowCaption( stringw( mVideoDriver->getFPS() ).c_str() );
    }

    mSceneManager->clear();

    mEventReceiver->lockMouse( false );

    return state;
}
