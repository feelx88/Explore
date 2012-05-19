#ifndef EXPLOREMENU_H
#define EXPLOREMENU_H

#include <irrlicht/irrlicht.h>
#include <engine/EngineTypedefs.h>
#include "GameState.h"

class ExploreMenu
{
public:
    ExploreMenu( ExplorePtr explore );

    E_GAME_STATE run();

protected:
    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;
};

#endif // EXPLOREMENU_H
