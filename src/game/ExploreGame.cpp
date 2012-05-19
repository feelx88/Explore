#include "ExploreGame.h"
#include <engine/LoggerSingleton.h>

ExploreGame::ExploreGame(IrrlichtDevicePtr device, PropTreePtr config,
                         EventReceiverPtr eventReceiver )
    : mDevice( device ),
      mVideoDriver( device->getVideoDriver() ),
      mEventReceiver( eventReceiver ),
      mConfig( config )
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
