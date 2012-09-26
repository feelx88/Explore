/*
    Copyright 2012 Felix Müller.

    This file is part of Explore.

    Explore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Explore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Explore.  If not, see <http://www.gnu.org/licenses/>.
*/


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
    enum E_GUI_IDS
    {
        EGID_MAINMENU = 10,
        EGID_NEW_GAME,
        EGID_LOAD_GAME,
        EGID_CONNECT,
        EGID_OPTIONS,
        EGID_QUIT,
        EGID_OPTIONS_WINDOW = 20
    };

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;
};

#endif // EXPLOREMENU_H
