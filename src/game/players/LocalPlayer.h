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

#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "VisualPlayer.h"

class APIEXPORT LocalPlayer : public VisualPlayer
{
public:
    enum E_PLAYER_KEY_MAPPINGS
    {
        EPKM_FORWARD = 0,
        EPKM_BACKWARD,
        EPKM_LEFT,
        EPKM_RIGHT,
        EPKM_JUMP,
        EPKM_MOUSECONTROL,
        EPKM_NEXTSLOT,
        EPKM_PREVIOUSSLOT,
        EPKM_COUNT
    };

    LocalPlayer( ExplorePtr explore, IPlayerPtr parent );

    ~LocalPlayer();

    void update();

    void addOwnedItem( ItemPtr item );

private:
    void switchItem( int index );

    void addItems();
    void createGUI();
    void setKeyMappings();
    void processControls();
    void drawCrosshair();

    ICameraSceneNodePtr mCamera;

    irr::EKEY_CODE mKeyMapping[EPKM_COUNT];

    int mCrossX, mCrossY;
    irr::video::SColor mCrossColor;

    IGUIElementPtr mItemWin;
    irr::gui::IGUIButton *mItemIcons[10];

    bool mJumped;
};

#endif // LOCALPLAYER_H
