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
      mEventReceiver( explore->getEventReceiver() )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    E_GAME_STATE state = EGS_GAME;

    mSceneManager->addCameraSceneNodeFPS();
    mSceneManager->addCubeSceneNode( 1.f );
    mSceneManager->addLightSceneNode( 0, vector3df( 0, 50, 0 ) );
    mSceneManager->addTerrainSceneNode( "data/heightmap.png", 0, -1, vector3df( -256, -5, -256 ) );

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

        mVideoDriver->endScene();
    }

    mSceneManager->clear();

    return state;
}
