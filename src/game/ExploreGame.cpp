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

    EntityPtr e1( new Entity( mDevice, mBulletWorld, EPATH + "TestCube" ) );
    e1->setPosition( vector3df( 10.f, 10.f, 10.f ) );
    EntityPtr e2( new Entity( mDevice, mBulletWorld, EPATH + "TestCube" ) );
    e2->setPosition( vector3df( 10.f, 12.f, 10.f ) );
    EntityPtr e3( new Entity( mDevice, mBulletWorld, EPATH + "TestCube" ) );
    e3->setPosition( vector3df( -10.f, 10.f, 10.f ) );
    EntityPtr e4( new Entity( mDevice, mBulletWorld, EPATH + "TestCube" ) );
    e4->setPosition( vector3df( -10.f, 12.f, 10.f ) );

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
