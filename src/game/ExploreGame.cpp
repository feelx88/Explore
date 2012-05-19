#include "ExploreGame.h"

ExploreGame::ExploreGame( IrrlichtDevicePtr device, PropTreePtr config )
    : mDevice( device ),
      mVideoDriver( device->getVideoDriver() ),
      mConfig( config )
{
}

E_GAME_STATE ExploreGame::run()
{
    while( mDevice->run() )
    {
        mVideoDriver->beginScene();
        mVideoDriver->endScene();
    }
}
