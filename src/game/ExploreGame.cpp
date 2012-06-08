#include "ExploreGame.h"
#include "Explore.h"
#include "Player.h"
#include <engine/IrrlichtTools.h>
#include <engine/PathTools.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/Entity.h>
#include <engine/EntityContainer.h>
#include <engine/LuaScript.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

#define EPATH std::string( "data/Entities/" )

ExploreGame::ExploreGame( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mSceneManager( mDevice->getSceneManager() ),
      mEventReceiver( explore->getEventReceiver() ),
      mGUI( mDevice->getGUIEnvironment() ),
      mBulletWorld( explore->getBulletWorld() ),
      mLua( explore->getLuaVM() )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    E_GAME_STATE state = EGS_GAME;

    mEventReceiver->lockMouse( true );

    mSceneManager->addLightSceneNode( 0, vector3df( 0.f, 50.f, 0.f ),
                                      SColorf( 1, 1, 1 ), 1000.f );

    EntityContainerPtr level( new EntityContainer(
                                  mDevice, mBulletWorld,
                                  PathTools::getAbsolutePath( "TestMap1" ) ) );

    vector3df spawnPos =
            level->getProperties()->get( "Spawn.Position", vector3df() );

    Player p( mExplore );
    p.getEntity()->setPosition( spawnPos );

    mBulletWorld->setGravity( btVector3( 0.f, -10.f, 0.f ) );

    btClock clock;

    while( running && mDevice->run() )
    {
        mBulletWorld->stepSimulation(
                    float( clock.getTimeMilliseconds() ) / 1000.f, 50 );
        clock.reset();

        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 0 ) );
        mSceneManager->drawAll();
        mGUI->drawAll();

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
