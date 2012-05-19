#ifndef EXPLOREGAME_H
#define EXPLOREGAME_H

#include "GameState.h"
#include <engine/EngineTypedefs.h>

class ExploreGame
{
public:
    ExploreGame( IrrlichtDevicePtr device, PropTreePtr config );

    E_GAME_STATE run();

private:
    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;

    PropTreePtr mConfig;
};

#endif // EXPLOREGAME_H
