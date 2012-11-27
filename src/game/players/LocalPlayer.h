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

#include "../IPlayer.h"

class LocalPlayer : public IPlayer
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

    void switchItem( int index );

    ItemPtr getActiveItem() const;

    EntityPtr getEntity() const;

    irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const;
    irr::core::vector3df getPosition() const;
    irr::core::quaternion getRotation() const;

private:
    void addItems();
    void createGUI();
    void setKeyMappings();
    void processControls();
    void drawCrosshair();

    ICameraSceneNodePtr mCamera;
    EntityPtr mEntity;

    irr::EKEY_CODE mKeyMapping[EPKM_COUNT];

    int mActiveItem, mNumItems;

    int mCrossX, mCrossY;
    irr::video::SColor mCrossColor;

    IGUIElementPtr mItemWin;
    irr::gui::IGUIButton *mItemIcons[10];

    bool mJumped;
};

#endif // LOCALPLAYER_H
