#include "ExploreGame.h"
#include "Explore.h"
#include <engine/LoggerSingleton.h>

ExploreGame::ExploreGame( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mEventReceiver( explore->getEventReceiver() )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    E_GAME_STATE state = EGS_QUIT;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene();

        if( mEventReceiver->keyPressed( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            state = EGS_MAIN_MENU;
            running = false;
        }

        mVideoDriver->endScene();
    }

    return state;
}
