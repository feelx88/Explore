#ifndef EXPLOREGAME_H
#define EXPLOREGAME_H

#include "GameState.h"
#include <engine/EngineTypedefs.h>

class ExploreGame
{
public:
    ExploreGame( ExplorePtr explore );

    E_GAME_STATE run();

private:
    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;

    EventReceiverPtr mEventReceiver;
};

#endif // EXPLOREGAME_H
