#ifndef EXPLOREMENU_H
#define EXPLOREMENU_H

#include <irrlicht/irrlicht.h>
#include <engine/EngineTypedefs.h>
#include "GameState.h"

class ExploreMenu
{
public:
    ExploreMenu(IrrlichtDevicePtr device , PropTreePtr config );

    E_GAME_STATE run();

protected:

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;

    PropTreePtr mConfig;

    irr::gui::IGUIButton *mButtonQuit;
};

#endif // EXPLOREMENU_H
